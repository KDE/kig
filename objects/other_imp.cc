// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "other_imp.h"

#include <math.h>

#include "bogus_imp.h"
#include "point_imp.h"
#include "line_imp.h"
#include "circle_imp.h"

#include "../misc/screeninfo.h"
#include "../misc/common.h"
#include "../misc/kigtransform.h"
#include "../misc/kigpainter.h"
#include "../misc/goniometry.h"
#include "../kig/kig_view.h"

#include <cmath>
#include <utility>
using namespace std;

const int AngleImp::radius = 30;

AngleImp::~AngleImp()
{
}

ObjectImp* AngleImp::transform( const Transformation& ) const
{
  // TODO ?
  return new InvalidImp;
}

void AngleImp::draw( KigPainter& p ) const
{
  if ( mangle == M_PI / 2 && mmarkRightAngle )
  {
    p.drawRightAngle( mpoint, mstartangle, AngleImp::radius );
  }
  else
  {
    p.drawAngle( mpoint, mstartangle, mangle, AngleImp::radius );
  }
}

AngleImp::AngleImp( const Coordinate& pt, double start_angle_in_radials,
                    double angle_in_radials, bool markRightAngle )
  : mpoint( pt ), mstartangle( start_angle_in_radials ),
    mangle( angle_in_radials ), mmarkRightAngle(markRightAngle)
{
}

bool AngleImp::contains( const Coordinate& p, int width, const KigWidget& w ) const
{
  double radius = AngleImp::radius * w.screenInfo().pixelWidth();

  if ( mangle == M_PI / 2 && mmarkRightAngle )
  {
    //rotate around -mstartangle
    double fixedX = cos( mstartangle ) * ( p.x - mpoint.x ) + sin( mstartangle ) * ( p.y - mpoint.y );
    double fixedY = -sin( mstartangle ) * ( p.x - mpoint.x ) + cos( mstartangle ) * ( p.y - mpoint.y );

    if ( fabs( fixedX - radius * sin( M_PI / 4 ) ) < w.screenInfo().normalMiss( width ) )
    {
      return ( fixedY <= radius * sin( M_PI / 4 ) ) && ( fixedY > 0 );
    }
    else if ( fabs( fixedY - radius * sin( M_PI / 4 ) ) < w.screenInfo().normalMiss( width ) )
    {
      return ( fixedX <= radius * sin( M_PI / 4 ) ) && ( fixedX > 0 );
    }

    return false;
  }
  else
  {
    if ( fabs( (p-mpoint).length() - radius ) > w.screenInfo().normalMiss( width ) )
      return false;

    Coordinate vect = p - mpoint;
    double angle = atan2( vect.y, vect.x );
    while ( angle < mstartangle ) angle += 2*M_PI;
    return angle <= mstartangle + mangle;
  }
}

bool AngleImp::inRect( const Rect& r, int width, const KigWidget& w ) const
{
  // TODO ?
  return r.contains( mpoint, w.screenInfo().normalMiss( width ) );
}

Coordinate AngleImp::attachPoint() const
{
  return mpoint;
}

int AngleImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 3;
}

const QByteArrayList AngleImp::propertiesInternalNames() const
{
  QByteArrayList l = Parent::propertiesInternalNames();
  l << "angle-radian";
  l << "angle-degrees";
  l << "angle-bisector";
  assert( l.size() == AngleImp::numberOfProperties() );
  return l;
}

const QByteArrayList AngleImp::properties() const
{
  QByteArrayList l = Parent::properties();
  l << I18N_NOOP( "Angle in Radians" );
  l << I18N_NOOP( "Angle in Degrees" );
  l << I18N_NOOP( "Angle Bisector" );
  assert( l.size() == AngleImp::numberOfProperties() );
  return l;
}

const ObjectImpType* AngleImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return AngleImp::stype();
}

const char* AngleImp::iconForProperty( int which ) const
{
  int numprop = 0;
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  if ( which == Parent::numberOfProperties() + numprop++ )
    return "angle_size"; // size in radians
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "angle_size"; // size in degrees
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "angle_bisector"; // angle bisector..
  else assert( false );
  return "";
}

ObjectImp* AngleImp::property( int which, const KigDocument& w ) const
{
  int numprop = 0;
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  if ( which == Parent::numberOfProperties() + numprop++ )
    return new DoubleImp( size() );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new DoubleImp( Goniometry::convert( size(), Goniometry::Rad, Goniometry::Deg ) );
  else if ( which == Parent::numberOfProperties() + numprop++ )
  {
    const double angle = mstartangle + mangle / 2;
    Coordinate p2 = mpoint + Coordinate( cos( angle ), sin( angle ) ) * 10;
    return new RayImp( mpoint, p2 );
  }
  else assert( false );
  return new InvalidImp;
}

double AngleImp::size() const
{
  return mangle;
}

ObjectImp* AngleImp::copy() const
{
  return new AngleImp( mpoint, mstartangle, mangle, mmarkRightAngle );
}

VectorImp::VectorImp( const Coordinate& a, const Coordinate& b )
  : mdata( a, b )
{
}

VectorImp::~VectorImp()
{
}

ObjectImp* VectorImp::transform( const Transformation& t ) const
{
  Coordinate ta = t.apply( mdata.a );
  Coordinate tb = t.apply( mdata.b );
  if ( ta.valid() && tb.valid() ) return new VectorImp( ta, tb );
  else return new InvalidImp;
}

void VectorImp::draw( KigPainter& p ) const
{
  p.drawVector( mdata.a, mdata.b );
}

bool VectorImp::contains( const Coordinate& o, int width, const KigWidget& w ) const
{
  return internalContainsPoint( o, w.screenInfo().normalMiss( width ) );
}

bool VectorImp::inRect( const Rect& r, int width, const KigWidget& w ) const
{
  return lineInRect( r, mdata.a, mdata.b, width, this, w );
}

int VectorImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 5;
}

const QByteArrayList VectorImp::propertiesInternalNames() const
{
  QByteArrayList ret = Parent::propertiesInternalNames();
  ret << "length";
  ret << "vect-mid-point";
  ret << "length-x";
  ret << "length-y";
  ret << "vector-opposite";
  assert( ret.size() == VectorImp::numberOfProperties() );
  return ret;
}

const QByteArrayList VectorImp::properties() const
{
  QByteArrayList ret = Parent::properties();
  ret << I18N_NOOP( "Length" );
  ret << I18N_NOOP( "Midpoint" );
  ret << I18N_NOOP( "X length" );
  ret << I18N_NOOP( "Y length" );
  ret << I18N_NOOP( "Opposite Vector" );
  assert( ret.size() == VectorImp::numberOfProperties() );
  return ret;
}

const ObjectImpType* VectorImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return VectorImp::stype();
}

const char* VectorImp::iconForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "distance"; // length
  else if ( which == Parent::numberOfProperties() + 1 )
    return "bisection"; // mid point
  else if ( which == Parent::numberOfProperties() + 2 )
    return "distance"; // length-x
  else if ( which == Parent::numberOfProperties() + 3 )
    return "distance"; // length-y
  else if ( which == Parent::numberOfProperties() + 4 )
    return "opposite-vector"; // opposite vector
  else assert( false );
  return "";
}

ObjectImp* VectorImp::property( int which, const KigDocument& w ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else if ( which == Parent::numberOfProperties() )
    return new DoubleImp( length() );
  else if ( which == Parent::numberOfProperties() + 1 )
    return new PointImp( ( mdata.a + mdata.b ) / 2 );
  else if ( which == Parent::numberOfProperties() + 2 )
    return new DoubleImp( fabs( mdata.a.x - mdata.b.x ) );
  else if ( which == Parent::numberOfProperties() + 3 )
    return new DoubleImp( fabs( mdata.a.y - mdata.b.y ) );
  else if ( which == Parent::numberOfProperties() + 4 ) // opposite
    return new VectorImp( mdata.a, 2*mdata.a-mdata.b );
  else assert( false );
  return new InvalidImp;
}

VectorImp* VectorImp::copy() const
{
  return new VectorImp( mdata.a, mdata.b );
}

const Coordinate VectorImp::dir() const
{
  return mdata.dir();
}

void AngleImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

void VectorImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

double VectorImp::length() const
{
  return ( mdata.a - mdata.b ).length();
}

ArcImp::ArcImp( const Coordinate& center, const double radius,
                const double startangle, const double angle )
  : CurveImp(), mcenter( center ), mradius( radius ),
    msa( startangle ), ma( angle )
{
  if ( ma < 0 )
  {
    // we want a positive angle..
    msa = msa + ma;
    ma = -ma;
  };
}

ArcImp::~ArcImp()
{
}

ArcImp* ArcImp::copy() const
{
  return new ArcImp( mcenter, mradius, msa, ma );
}

ObjectImp* ArcImp::transform( const Transformation& t ) const
{
  //
  // if the transformation is nonhomothetic, the result is a conic arc,
  // not a circular arc
  //
  if ( ! t.isHomothetic() )
  {
    //CircleImp support = CircleImp( mcenter, mradius );
    ConicCartesianData data = CircleImp( mcenter, fabs( mradius ) ).cartesianData();
    //return new InvalidImp();
    ConicArcImp conicarc = ConicArcImp( data, msa, ma );
    return conicarc.transform ( t );
  }

  Coordinate nc = t.apply( mcenter );
  double nr = t.apply( mradius );
  // transform msa...
  double nmsa = msa;
  if ( t.getAffineDeterminant() > 0 )
  {
    nmsa = msa - t.getRotationAngle();
  } else
  {
    Coordinate ar = t.apply2by2only( Coordinate( cos(msa), sin(msa) ) );
    nmsa = atan2( ar.y, ar.x );
    nmsa -= ma;
  }
  while ( nmsa < -M_PI ) nmsa += 2*M_PI;
  while ( nmsa > M_PI ) nmsa -= 2*M_PI;
  if ( nc.valid() ) return new ArcImp( nc, nr, nmsa, ma );
  else return new InvalidImp;
}

void ArcImp::draw( KigPainter& p ) const
{
  p.drawArc( mcenter, fabs( mradius ), msa, ma );
}

bool ArcImp::contains( const Coordinate& p, int width, const KigWidget& w ) const
{
  return internalContainsPoint( p, w.screenInfo().normalMiss( width ) );
}

bool ArcImp::inRect( const Rect&, int, const KigWidget& ) const
{
  // TODO
  return false;
}

bool ArcImp::valid() const
{
  return true;
}

int ArcImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 10;
}

const QByteArrayList ArcImp::properties() const
{
  QByteArrayList ret = Parent::properties();
  ret << I18N_NOOP( "Center" );
  ret << I18N_NOOP( "Radius" );
  ret << I18N_NOOP( "Angle" );
  ret << I18N_NOOP( "Angle in Degrees" );
  ret << I18N_NOOP( "Angle in Radians" );
  ret << I18N_NOOP( "Sector Surface" );
  ret << I18N_NOOP( "Arc Length" );
  ret << I18N_NOOP( "Support Circle" );
  ret << I18N_NOOP( "First End Point" );
  ret << I18N_NOOP( "Second End Point" );
  assert( ret.size() == ArcImp::numberOfProperties() );
  return ret;
}

const QByteArrayList ArcImp::propertiesInternalNames() const
{
  QByteArrayList ret = Parent::propertiesInternalNames();
  ret << "center";
  ret << "radius";
  ret << "angle";
  ret << "angle-degrees";
  ret << "angle-radians";
  ret << "sector-surface";
  ret << "arc-length";
  ret << "support";
  ret << "end-point-A";
  ret << "end-point-B";
  return ret;
}

const char* ArcImp::iconForProperty( int which ) const
{
  int numprop = 0;
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "arc_center"; // center
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "";
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "angle";
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "angle_size";
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "angle_size";
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "";
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "";
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "";
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "";
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return "";
  else assert( false );
  return "";
}

ObjectImp* ArcImp::property( int which, const KigDocument& d ) const
{
  int numprop = 0;
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, d );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new PointImp( mcenter );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new DoubleImp( fabs( mradius ) );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new AngleImp( mcenter, msa, ma, false );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new DoubleImp( Goniometry::convert( ma, Goniometry::Rad, Goniometry::Deg ) );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new DoubleImp( ma );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new DoubleImp( sectorSurface() );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new DoubleImp( fabs( mradius ) * ma );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new CircleImp( mcenter, mradius );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new PointImp( firstEndPoint() );
  else if ( which == Parent::numberOfProperties() + numprop++ )
    return new PointImp( secondEndPoint() );
  else assert( false );
  return new InvalidImp;
}

double ArcImp::sectorSurface() const
{
  return mradius * mradius * ma / 2;
}

const ObjectImpType* ArcImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else
    return ArcImp::stype();
}

void ArcImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

double ArcImp::getParam( const Coordinate& c, const KigDocument& ) const
{
  Coordinate d = (c - mcenter).normalize();
  double angle = atan2( d.y, d.x );
  angle -= msa;
// mp: problems with large arcs
  while ( angle > ma/2 + M_PI ) angle -= 2*M_PI;
  while ( angle < ma/2 - M_PI ) angle += 2*M_PI;
//
  angle = max( 0., min( angle, ma ) );
  angle /= ma;
  if ( mradius < 0 ) angle = 1.0 - angle;  // this is to avoid abrupt jumps when an ArcBTPType changes concavity
  return angle;
}

const Coordinate ArcImp::getPoint( double p, const KigDocument& ) const
{
  if ( mradius < 0 ) p = 1.0 - p;  // this is to avoid abrupt jumps when an ArcBTPType changes concavity
  double angle = msa + p * ma;
  Coordinate d = Coordinate( cos( angle ), sin( angle ) ) * fabs( mradius );
  return mcenter + d;
}

const Coordinate ArcImp::center() const
{
  return mcenter;
}

double ArcImp::radius() const
{
  return fabs( mradius );
}

double ArcImp::orientation() const
{
  return ( mradius >= 0)?1:(-1);
}

double ArcImp::startAngle() const
{
  return msa;
}

double ArcImp::angle() const
{
  return ma;
}

Coordinate ArcImp::firstEndPoint() const
{
  /**
   * mp: We take advantage of the arc orientation (mainly for the benefit of arc through 3 points)
   * in order to avoid abrupt jumps when moving points
   */
  const double angle = mradius >= 0 ? msa : msa+ma;
  return mcenter + Coordinate( cos( angle ), sin( angle ) ) * fabs( mradius );
}

Coordinate ArcImp::secondEndPoint() const
{
  const double angle = mradius >= 0 ? msa+ma : msa;
  return mcenter + Coordinate( cos( angle ), sin( angle ) ) * fabs( mradius );
}

const Coordinate VectorImp::a() const
{
  return mdata.a;
}

const Coordinate VectorImp::b() const
{
  return mdata.b;
}

bool ArcImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( ArcImp::stype() ) &&
    static_cast<const ArcImp&>( rhs ).radius() == radius() &&
    static_cast<const ArcImp&>( rhs ).startAngle() == startAngle() &&
    static_cast<const ArcImp&>( rhs ).angle() == angle();
}

bool AngleImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( AngleImp::stype() ) &&
    static_cast<const AngleImp&>( rhs ).point() == point() &&
    static_cast<const AngleImp&>( rhs ).startAngle() == startAngle() &&
    static_cast<const AngleImp&>( rhs ).angle() == angle();
}

bool VectorImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( VectorImp::stype() ) &&
    static_cast<const VectorImp&>( rhs ).a() == a() &&
    static_cast<const VectorImp&>( rhs ).b() == b();
}

const ObjectImpType* AngleImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "angle",
    I18N_NOOP( "angle" ),
    I18N_NOOP( "Select this angle" ),
    I18N_NOOP( "Select angle %1" ),
    I18N_NOOP( "Remove an Angle" ),
    I18N_NOOP( "Add an Angle" ),
    I18N_NOOP( "Move an Angle" ),
    I18N_NOOP( "Attach to this angle" ),
    I18N_NOOP( "Show an Angle" ),
    I18N_NOOP( "Hide an Angle" )
    );
  return &t;
}
const ObjectImpType* VectorImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "vector",
    I18N_NOOP( "vector" ),
    I18N_NOOP( "Select this vector" ),
    I18N_NOOP( "Select vector %1" ),
    I18N_NOOP( "Remove a Vector" ),
    I18N_NOOP( "Add a Vector" ),
    I18N_NOOP( "Move a Vector" ),
    I18N_NOOP( "Attach to this vector" ),
    I18N_NOOP( "Show a Vector" ),
    I18N_NOOP( "Hide a Vector" )
    );
  return &t;
}
const ObjectImpType* ArcImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "arc",
    I18N_NOOP( "arc" ),
    I18N_NOOP( "Select this arc" ),
    I18N_NOOP( "Select arc %1" ),
    I18N_NOOP( "Remove an Arc" ),
    I18N_NOOP( "Add an Arc" ),
    I18N_NOOP( "Move an Arc" ),
    I18N_NOOP( "Attach to this arc" ),
    I18N_NOOP( "Show an Arc" ),
    I18N_NOOP( "Hide an Arc" )
    );
  return &t;
}

const ObjectImpType* AngleImp::type() const
{
  return AngleImp::stype();
}

const ObjectImpType* VectorImp::type() const
{
  return VectorImp::stype();
}

const ObjectImpType* ArcImp::type() const
{
  return ArcImp::stype();
}

bool ArcImp::containsPoint( const Coordinate& p, const KigDocument& ) const
{
  return internalContainsPoint( p, test_threshold );
}

bool ArcImp::internalContainsPoint( const Coordinate& p, double threshold ) const
{
  return isOnArc( p, mcenter, fabs( mradius ), msa, ma, threshold );
}

bool AngleImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

bool VectorImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  return Parent::isPropertyDefinedOnOrThroughThisImp( which );
}

bool ArcImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  int pnum = 0;

  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return false;  // center
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return false;   // radius
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return false;   // angle
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return false;   // angle in degrees
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return false;   // angle in radians
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return false;   // sector surface
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return false;   // arc length
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return false;    // support
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return true;    // first end-point
  else if ( which == Parent::numberOfProperties() + pnum++ )
    return true;    // second end-point
  else return false;
  return false;
}

Rect AngleImp::surroundingRect() const
{
  return Rect( mpoint, 0, 0 );
}

Rect VectorImp::surroundingRect() const
{
  return Rect( mdata.a, mdata.b );
}

Rect ArcImp::surroundingRect() const
{
  // the returned rect should contain the center point(?), the two end
  // points, and all extreme x and y positions in between.
  //Rect ret( mcenter, 0, 0 );
  double a = msa;
  //ret.setContains( mcenter + fabs( mradius )*Coordinate( cos( a ), sin( a ) ) );
  Rect ret ( mcenter + fabs( mradius )*Coordinate( cos( a ), sin( a ) ), 0, 0 );
  a = msa + ma;
  ret.setContains( mcenter + fabs( mradius )*Coordinate( cos( a ), sin( a ) ) );
  for ( a = -2*M_PI; a <= 2*M_PI; a+=M_PI/2 )
  {
    Coordinate d = mcenter + fabs( mradius )*Coordinate( cos( a ), sin( a ) );
    if ( msa <= a && a <= msa + ma )
      ret.setContains( d );
  }
  return ret;
}

const Coordinate VectorImp::getPoint( double param, const KigDocument& ) const
{
  return mdata.a + mdata.dir() * param;
}

double VectorImp::getParam( const Coordinate& p, const KigDocument& ) const
{
  Coordinate pt = calcPointOnPerpend( mdata, p );
  pt = calcIntersectionPoint( mdata, LineData( p, pt ) );
  // if pt is over the end of the vector we set it to one of the end
  // points of the vector...
  if ( ( pt - mdata.a ).length() > dir().length() )
    pt = mdata.b;
  else if ( ( pt - mdata.b ).length() > dir().length() )
    pt = mdata.a;
  if ( mdata.b == mdata.a ) return 0;
  return ( ( pt - mdata.a ).length() ) / ( dir().length() );
}

bool VectorImp::containsPoint( const Coordinate& p, const KigDocument& ) const
{
  return internalContainsPoint( p, test_threshold );
}

bool VectorImp::internalContainsPoint( const Coordinate& p, double threshold ) const
{
  return isOnSegment( p, mdata.a, mdata.b, threshold );
}

LineData VectorImp::data() const
{
  return mdata;
}
