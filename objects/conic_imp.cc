// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "conic_imp.h"

#include "bogus_imp.h"
#include "point_imp.h"

#include "../misc/kigpainter.h"
#include "../misc/common.h"
#include "../misc/coordinate_system.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"

#include <klocale.h>

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

const uint ConicImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 5;
}

const QCStringList ConicImp::propertiesInternalNames() const
{
  QCStringList l = Parent::propertiesInternalNames();
  l << "type";
  l << "first-focus";
  l << "second-focus";
  l << "cartesian-equation";
  l << "polar-equation";
  assert( l.size() == ConicImp::numberOfProperties() );
  return l;
}

const QCStringList ConicImp::properties() const
{
  QCStringList l = Parent::properties();
  l << I18N_NOOP( "Conic Type" );
  l << I18N_NOOP( "First Focus" );
  l << I18N_NOOP( "Second Focus" );
  l << I18N_NOOP( "Cartesian Equation" );
  l << I18N_NOOP( "Polar Equation" );
  assert( l.size() == ConicImp::numberOfProperties() );
  return l;
}

const ObjectImpType* ConicImp::impRequirementForProperty( uint which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return ConicImp::stype();
}

const char* ConicImp::iconForProperty( uint which ) const
{
  int pnum = 0;
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  if ( which == Parent::numberOfProperties() + pnum++ )
    return "kig_text"; // conic type string
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

ObjectImp* ConicImp::property( uint which, const KigDocument& w ) const
{
  int pnum = 0;

  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  if ( which == Parent::numberOfProperties() + pnum++ )
    return new StringImp( conicTypeString() );
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
    return "";
  }
}

QString ConicImp::cartesianEquationString( const KigDocument& ) const
{
  QString ret = i18n( "%1 x² + %2 y² + %3 xy + %4 x + %5 y + %6 = 0" );
  ConicCartesianData data = cartesianData();
  ret = ret.arg( data.coeffs[0], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[1], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[2], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[3], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[4], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[5], 0, 'g', 3 );
  return ret;
}

QString ConicImp::polarEquationString( const KigDocument& w ) const
{
  QString ret = i18n( "rho = %1/(1 + %2 cos theta + %3 sin theta)\n    [centered at %4]" );
  const ConicPolarData data = polarData();

  ret = ret.arg( data.pdimen, 0, 'g', 3 );
  ret = ret.arg( -data.ecostheta0, 0, 'g', 3 );
  ret = ret.arg( -data.esintheta0, 0, 'g', 3 );

  ret = ret.arg( w.coordinateSystem().fromScreen( data.focus1, w ) );
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
  assert( data.valid() );
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

bool ConicImp::isPropertyDefinedOnOrThroughThisImp( uint which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

Rect ConicImp::surroundingRect() const
{
  // it's prolly possible to calculate this ( in the case that the
  // conic is limited in size ), but for now we don't.

  return Rect::invalidRect();
}
