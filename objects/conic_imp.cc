// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "conic_imp.h"

#include <math.h>

#include "bogus_imp.h"
#include "point_imp.h"

#include "../misc/kigpainter.h"
#include "../misc/common.h"
#include "../misc/coordinate_system.h"
#include "../misc/equationstring.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"

ObjectImp* ConicImp::transform( const Transformation& t ) const
{
  bool valid = true;
  ConicCartesianData d = calcConicTransformation( cartesianData(), t, valid );
  if ( ! valid ) return new InvalidImp;
  else return new ConicImpCart( d );
}

void ConicImp::draw( KigPainter& p ) const
{
  p.drawCurve( this );
}

bool ConicImp::valid() const
{
  return true;
}

bool ConicImp::contains( const Coordinate& o, int width, const KigWidget& w ) const
{
  return internalContainsPoint( o, w.screenInfo().normalMiss( width ) );
}

bool ConicImp::inRect( const Rect&, int, const KigWidget& ) const
{
  // TODO
  return false;
}

int ConicImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 6;
}

const QByteArrayList ConicImp::propertiesInternalNames() const
{
  QByteArrayList l = Parent::propertiesInternalNames();
  l << "type";
  l << "center";
  l << "first-focus";
  l << "second-focus";
  l << "cartesian-equation";
  l << "polar-equation";
  assert( l.size() == ConicImp::numberOfProperties() );
  return l;
}

const QByteArrayList ConicImp::properties() const
{
  QByteArrayList l = Parent::properties();
  l << I18N_NOOP( "Conic Type" );
  l << I18N_NOOP( "Center" );
  l << I18N_NOOP( "First Focus" );
  l << I18N_NOOP( "Second Focus" );
  l << I18N_NOOP( "Cartesian Equation" );
  l << I18N_NOOP( "Polar Equation" );
  assert( l.size() == ConicImp::numberOfProperties() );
  return l;
}

const ObjectImpType* ConicImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return ConicImp::stype();
}

const char* ConicImp::iconForProperty( int which ) const
{
  int pnum = 0;
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  if ( which == Parent::numberOfProperties() + pnum++ )
    return "kig_text"; // conic type string
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return ""; // center
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return ""; // focus1
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return ""; // focus2
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return "kig_text"; // cartesian equation string
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return "kig_text"; // polar equation string
  else assert( false );
  return "";
}

ObjectImp* ConicImp::property( int which, const KigDocument& w ) const
{
  int pnum = 0;

  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  if ( which == Parent::numberOfProperties() + pnum++ )
    return new StringImp( conicTypeString() );
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return new PointImp( coniccenter() );
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return new PointImp( focus1() );
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return new PointImp( focus2() );
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return new StringImp( cartesianEquationString( w ) );
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return new StringImp( polarEquationString( w ) );
  else assert( false );
  return new InvalidImp;
}

double ConicImp::getParam( const Coordinate& p, const KigDocument& ) const
{
  return getParam( p );
}

double ConicImp::getParam( const Coordinate& p ) const
{
  const ConicPolarData d = polarData();
  Coordinate tmp = p - d.focus1;
  double l = tmp.length();
  double theta = atan2(tmp.y, tmp.x);
  double costheta = cos(theta);
  double sintheta = sin(theta);
  double ecosthetamtheta0 = costheta*d.ecostheta0 + sintheta*d.esintheta0;
  double esinthetamtheta0 = sintheta*d.ecostheta0 - costheta*d.esintheta0;
  double oneplus = 1.0 + d.ecostheta0*d.ecostheta0 + d.esintheta0*d.esintheta0;
  double fact = esinthetamtheta0*(1.0 - ecosthetamtheta0)/(oneplus - 2*ecosthetamtheta0);
// fact is sin(a)*cos(a) where a is the angle between the ray from the first
// focus and the normal to the conic.  We need it in order to adjust the
// angle according to the projection onto the conic of our point
  double rho1 = d.pdimen / (1 - ecosthetamtheta0);
  double rho2 = - d.pdimen / (1 + ecosthetamtheta0);
  if (fabs(rho1 - l) < fabs(rho2 - l))
  {
    theta += (rho1 - l)*fact/rho1;
    return fmod(theta / ( 2 * M_PI ) + 1, 1);
  } else {
    theta += (rho2 - l)*fact/rho2;
    return fmod(theta / ( 2 * M_PI ) + 0.5, 1);
  }
}

const Coordinate ConicImp::getPoint( double p, const KigDocument& ) const
{
  return getPoint( p );
}

const Coordinate ConicImp::getPoint( double p ) const
{
  const ConicPolarData d = polarData();

  double costheta = cos(p * 2 * M_PI);
  double sintheta = sin(p * 2 * M_PI);
  double rho = d.pdimen / (1 - costheta* d.ecostheta0 - sintheta* d.esintheta0);
  return d.focus1 + Coordinate (costheta, sintheta) * rho;
}

int ConicImp::conicType() const
{
  const ConicPolarData d = polarData();
  double ec = d.ecostheta0;
  double es = d.esintheta0;
  double esquare = ec*ec + es*es;
  const double parabolamiss = 1e-3;  // don't know what a good value could be

  if (esquare < 1.0 - parabolamiss) return 1;
  if (esquare > 1.0 + parabolamiss) return -1;

  return 0;
}

QString ConicImp::conicTypeString() const
{
  switch (conicType())
  {
  case 1:
    return i18n("Ellipse");
  case -1:
    return i18n("Hyperbola");
  case 0:
    return i18n("Parabola");
  default:
    assert( false );
    return QLatin1String("");
  }
}

QString ConicImp::cartesianEquationString( const KigDocument& ) const
{
  ConicCartesianData data = cartesianData();
  EquationString ret = EquationString( QLatin1String("") );
  bool needsign = false;
  if ( isVerticalParabola( data ) )
  {
    double f = - 1.0/data.coeffs[4];
    ret.addTerm( - f*data.coeffs[4], ret.y(), needsign );
    ret.append( " = " );
    needsign = false;
    ret.addTerm( f*data.coeffs[0], ret.x2(), needsign );
    ret.addTerm( f*data.coeffs[1], ret.y2(), needsign );
    ret.addTerm( f*data.coeffs[2], ret.xy(), needsign );
    ret.addTerm( f*data.coeffs[3], ret.x(), needsign );
    ret.addTerm( f*data.coeffs[5], QLatin1String(""), needsign );
    return ret;
  }
  ret.addTerm( data.coeffs[0], ret.x2(), needsign );
  ret.addTerm( data.coeffs[1], ret.y2(), needsign );
  ret.addTerm( data.coeffs[2], ret.xy(), needsign );
  ret.addTerm( data.coeffs[3], ret.x(), needsign );
  ret.addTerm( data.coeffs[4], ret.y(), needsign );
  ret.addTerm( data.coeffs[5], QLatin1String(""), needsign );
  ret.append( " = 0" );
  return ret;

//  QString ret = i18n( "%1 x² + %2 y² + %3 xy + %4 x + %5 y + %6 = 0" );
//  ConicCartesianData data = cartesianData();
//  ret = ret.arg( data.coeffs[0], 0, 'g', 3 );
//  ret = ret.arg( data.coeffs[1], 0, 'g', 3 );
//  ret = ret.arg( data.coeffs[2], 0, 'g', 3 );
//  ret = ret.arg( data.coeffs[3], 0, 'g', 3 );
//  ret = ret.arg( data.coeffs[4], 0, 'g', 3 );
//  ret = ret.arg( data.coeffs[5], 0, 'g', 3 );
//  return ret;
}

QString ConicImp::polarEquationString( const KigDocument& w ) const
{
//  QString ret = i18n( "rho = %1/(1 + %2 cos theta + %3 sin theta)\n    [centered at %4]" );
  const ConicPolarData data = polarData();

  EquationString ret = EquationString( i18n( "rho" ) );
  ret.append( " = " );
  if ( data.pdimen < 0 ) ret.append( "- " );
  bool needsign = false;
  ret.addTerm( fabs( data.pdimen ), QLatin1String(""), needsign );
  ret.append( "/(1" );
  needsign = true;
  ret.addTerm( -data.ecostheta0, i18n( "cos theta" ), needsign );
  ret.addTerm( -data.esintheta0, i18n( "sin theta" ), needsign );
  ret.append( ")\n" );
  ret.append( ki18n( "[centered at %1]" )
                   .subs( w.coordinateSystem().fromScreen( data.focus1, w ) )
//                   .subs( data.pdimen, 0, 'g', 3 );
//                   .subs( -data.ecostheta0, 0, 'g', 3 );
//                   .subs( -data.esintheta0, 0, 'g', 3 );
                   .toString() );

  ret.prettify();
  return ret;
}

const ConicCartesianData ConicImp::cartesianData() const
{
  return ConicCartesianData( polarData() );
}

Coordinate ConicImp::focus1() const
{
  return polarData().focus1;
}

Coordinate ConicImp::coniccenter() const
{
  const ConicPolarData d = polarData();
  double ec = d.ecostheta0;
  double es = d.esintheta0;

  double fact = d.pdimen/(1 - ec*ec - es*es);

  return d.focus1 + fact*Coordinate(ec, es);
}

Coordinate ConicImp::focus2() const
{
  const ConicPolarData d = polarData();
  double ec = d.ecostheta0;
  double es = d.esintheta0;

  double fact = 2*d.pdimen/(1 - ec*ec - es*es);

  return d.focus1 + fact*Coordinate(ec, es);
}

const ConicPolarData ConicImpCart::polarData() const
{
  return mpolardata;
}

const ConicCartesianData ConicImpCart::cartesianData() const
{
  return mcartdata;
}

ConicImpCart::ConicImpCart( const ConicCartesianData& data )
  : ConicImp(), mcartdata( data ), mpolardata( data )
{
  //assert( data.valid() );
}

ConicImpPolar::ConicImpPolar( const ConicPolarData& data )
  : ConicImp(), mdata( data )
{
}

ConicImpPolar::~ConicImpPolar()
{
}

const ConicPolarData ConicImpPolar::polarData() const
{
  return mdata;
}

ConicImpCart* ConicImpCart::copy() const
{
  return new ConicImpCart( mcartdata );
}

ConicImpPolar* ConicImpPolar::copy() const
{
  return new ConicImpPolar( mdata );
}

ConicImp::ConicImp()
{
}

ConicImp::~ConicImp()
{
}

ConicImpCart::~ConicImpCart()
{
}

void ConicImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool ConicImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( ConicImp::stype() ) &&
    static_cast<const ConicImp&>( rhs ).polarData() == polarData();
}

const ObjectImpType* ConicImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "conic",
    I18N_NOOP( "conic" ),
    I18N_NOOP( "Select this conic" ),
    I18N_NOOP( "Select conic %1" ),
    I18N_NOOP( "Remove a Conic" ),
    I18N_NOOP( "Add a Conic" ),
    I18N_NOOP( "Move a Conic" ),
    I18N_NOOP( "Attach to this conic" ),
    I18N_NOOP( "Show a Conic" ),
    I18N_NOOP( "Hide a Conic" )
    );
  return &t;
}

const ObjectImpType* ConicImp::type() const
{
  return ConicImp::stype();
}

bool ConicImp::containsPoint( const Coordinate& p, const KigDocument& ) const
{
  const ConicPolarData d = polarData();

// the threshold is relative to the size of the conic (mp)
  return internalContainsPoint( p, test_threshold*d.pdimen );
}

bool ConicImp::internalContainsPoint( const Coordinate& p, double threshold ) const
{
  const ConicPolarData d = polarData();

  Coordinate focus1 = d.focus1;
  double ecostheta0 = d.ecostheta0;
  double esintheta0 = d.esintheta0;
  double pdimen = d.pdimen;

  Coordinate pos = p - focus1;
  double len = pos.length();
  double costheta = pos.x / len;
  double sintheta = pos.y / len;

  double ecosthetamtheta0 = costheta*ecostheta0 + sintheta*esintheta0;
  double rho = pdimen / (1.0 - ecosthetamtheta0);

  double oneplus = 1.0 + ecostheta0*ecostheta0 + esintheta0*esintheta0;

// fact is the cosine of the angle between the ray from the first focus
// and the normal to the conic, so that we compute the real distance

  double fact = (1.0 - ecosthetamtheta0)/sqrt(oneplus - 2*ecosthetamtheta0);
  if ( fabs((len - rho)*fact) <= threshold ) return true;
  rho = - pdimen / ( 1.0 + ecosthetamtheta0 );
  fact = (1.0 + ecosthetamtheta0)/sqrt(oneplus + 2*ecosthetamtheta0);
  return fabs(( len - rho )*fact) <= threshold;
}

bool ConicImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

bool ConicImp::isVerticalParabola( ConicCartesianData& data ) const
{
  return (
           fabs( data.coeffs[1] ) < 1e-12 &&     // y^2
           fabs( data.coeffs[2] ) < 1e-12 &&     // xy
           fabs( data.coeffs[4] ) > 1e-5 );      // y
}

Rect ConicImp::surroundingRect() const
{
  // it's prolly possible to calculate this ( in the case that the
  // conic is limited in size ), but for now we don't.

  return Rect::invalidRect();
}

/* An arc of a conic is identified by a startangle and a size (angle);
 * both angles are measured with respect to the first focus of the conic
 * (the one used for the conic polar equation
 */

ConicArcImp::ConicArcImp( const ConicCartesianData& data,
                  const double startangle, const double angle )
  : ConicImpCart( data ), msa( startangle ), ma( angle )
{
}

ConicArcImp::~ConicArcImp()
{
}

ConicArcImp* ConicArcImp::copy() const
{
  return new ConicArcImp( mcartdata, msa, ma );
}

ObjectImp* ConicArcImp::transform( const Transformation& t ) const
{
  bool valid = true;
  ConicCartesianData d = calcConicTransformation( cartesianData(), t, valid );
  if ( ! valid ) return new InvalidImp;
  ConicArcImp* result = new ConicArcImp( d, 0.0, 2*M_PI );

  Coordinate a = t.apply( getPoint ( 0. ) );
  Coordinate b = t.apply( getPoint( 0.5 ) );
  Coordinate c = t.apply( getPoint( 1. ) );
  double anglea = 2*M_PI*result->getParam( a );
  double angleb = 2*M_PI*result->getParam( b );
  double anglec = 2*M_PI*result->getParam( c );
  double startangle = 0.;
  double angle = 2*M_PI;
  // anglea should be smaller than anglec
  if ( anglea > anglec )
  {
    double t = anglea;
    anglea = anglec;
    anglec = t;
  };
  if ( angleb > anglec || angleb < anglea )
  {
    startangle = anglec;
    angle = 2 * M_PI + anglea - startangle;
  }
  else
  {
    startangle = anglea;
    angle = anglec - anglea;
  };

  result->setStartAngle( startangle );
  result->setAngle( angle );
  return result;
}

bool ConicArcImp::contains( const Coordinate& o, int width, const KigWidget& w ) const
{
  return internalContainsPoint( o, w.screenInfo().normalMiss( width ),
     w.document() );
}

int ConicArcImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 3;
}

const QByteArrayList ConicArcImp::properties() const
{
  QByteArrayList ret = Parent::properties();
  ret << I18N_NOOP( "Supporting Conic" );
  ret << I18N_NOOP( "First End Point" );
  ret << I18N_NOOP( "Second End Point" );
  assert( ret.size() == ConicArcImp::numberOfProperties() );
  return ret;
}

const QByteArrayList ConicArcImp::propertiesInternalNames() const
{
  QByteArrayList ret = Parent::propertiesInternalNames();
  ret << "support";
  ret << "end-point-A";
  ret << "end-point-B";
  return ret;
}

const char* ConicArcImp::iconForProperty( int which ) const
{
  int numprop = 0;
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "";
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "";
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "";
  else assert( false );
  return "";
}

ObjectImp* ConicArcImp::property( int which, const KigDocument& d ) const
{
  int numprop = 0;
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, d );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new ConicImpCart( cartesianData() );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new PointImp( firstEndPoint());
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new PointImp( secondEndPoint());
  else return new InvalidImp;
  return new InvalidImp;
}

bool ConicArcImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  int pnum = 0;

  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return false;  // support
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return true;   // first end-point
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return true;   // second end-point
  else return false;
  return false;
}

Coordinate ConicArcImp::firstEndPoint() const
{
  return getPoint( 0. );
}

Coordinate ConicArcImp::secondEndPoint() const
{
  return getPoint( 1. );
}

const ObjectImpType* ConicArcImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "conic arc",
    I18N_NOOP( "conic arc" ),
    I18N_NOOP( "Select this conic arc" ),
    I18N_NOOP( "Select conic arc %1" ),
    I18N_NOOP( "Remove a Conic Arc" ),
    I18N_NOOP( "Add a Conic Arc" ),
    I18N_NOOP( "Move a Conic Arc" ),
    I18N_NOOP( "Attach to this conic arc" ),
    I18N_NOOP( "Show a Conic Arc" ),
    I18N_NOOP( "Hide a Conic Arc" )
    );
  return &t;
}

const ObjectImpType* ConicArcImp::type() const
{
  return ConicArcImp::stype();
}

bool ConicArcImp::containsPoint( const Coordinate& p, const KigDocument& doc) const
{
  const ConicPolarData d = polarData();

// the threshold is relative to the size of the conic (mp)
  return internalContainsPoint( p, test_threshold*d.pdimen, doc );
}

bool ConicArcImp::internalContainsPoint( const Coordinate& p, double threshold,
    const KigDocument& doc ) const
{
  // this is directly stolen from locus code...
  double param = getParam( p, doc );
  Coordinate p1 = getPoint( param, doc );
  double dist = (p1 - p).length();
  return fabs( dist ) <= threshold;
}

double ConicArcImp::getParam( const Coordinate& p, const KigDocument& ) const
{
  return getParam( p );
}

double ConicArcImp::getParam( const Coordinate& p ) const
{
  double thetarel = 2 * M_PI * ConicImpCart::getParam( p ) - msa;
  while ( thetarel < 0 ) thetarel += 2 * M_PI;
  if ( thetarel <= ma ) return ( thetarel / ma );

  double antipodo = ( 2 * M_PI + ma )/2;
  if ( thetarel < antipodo ) return (1.0);
  return (0.0);
}

const Coordinate ConicArcImp::getPoint( double p, const KigDocument& ) const
{
  return getPoint( p );
}

const Coordinate ConicArcImp::getPoint( double p ) const
{
  double pwide = ( p * ma + msa )/ (2*M_PI);
  return ConicImpCart::getPoint( pwide );
}

