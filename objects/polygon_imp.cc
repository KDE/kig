// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

//
// note (mp): there are now two boolean flags:
//   minside: tells if we want a "filled polygon"
//   mopen: in case of boundary, if we want an open polygonal curve
//
// a much more clean solution would be to have an AbstractPolygon class
// from which to inherit "Polygon", "ClosedPolygonal" and "OpenPolygonal"
// we should think of the possibility of doing this...
//
#include "polygon_imp.h"

#include <math.h>

#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"
#include "bezier_imp.h"

#include "../misc/common.h"
#include "../misc/coordinate.h"
#include "../misc/kigpainter.h"
#include "../misc/kigtransform.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"

#include <cmath>

AbstractPolygonImp::AbstractPolygonImp( const uint npoints, const std::vector<Coordinate>& points,
                        const Coordinate& centerofmass )
  : mnpoints( npoints ), mpoints( points ), mcenterofmass( centerofmass )
{
}

AbstractPolygonImp::AbstractPolygonImp( const std::vector<Coordinate>& points )
{
  uint npoints = points.size();
  Coordinate centerofmassn = Coordinate( 0, 0 );

  for ( uint i = 0; i < npoints; ++i )
  {
    centerofmassn += points[i];
  }
  mpoints = points;
  mcenterofmass = centerofmassn/npoints;
  mnpoints = npoints;
}

AbstractPolygonImp::~AbstractPolygonImp()
{
}

Coordinate AbstractPolygonImp::attachPoint() const
{
  return mcenterofmass;
}

std::vector<Coordinate> AbstractPolygonImp::ptransform( const Transformation& t ) const
{
/*mp:
 * any projective transformation makes sense for a polygon,
 * since segments transform into segments (but see below...)
 * of course regular polygons will no longer be
 * regular if t is not homothetic.
 * for projective transformations the polygon could transform to
 * an unbounded nonconnected polygon; this happens if some side
 * of the polygon crosses the critical line that maps to infinity
 * this can be easily checked using the getProjectiveIndicator
 * function
 */
  std::vector<Coordinate> np;
//  if ( ! t.isHomothetic() )
//    return new InvalidImp();

  if ( ! t.isAffine() )     /* in this case we need a more extensive test */
  {
    double maxp = -1.0;
    double minp = 1.0;
    for ( uint i = 0; i < mpoints.size(); ++i )
    {
      double p = t.getProjectiveIndicator( mpoints[i] );
      if ( p > maxp ) maxp = p;
      if ( p < minp ) minp = p;
    }
    if ( maxp > 0 && minp < 0 ) return np;
  }
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    Coordinate nc = t.apply( mpoints[i] );
    if ( !nc.valid() )
      return np;
    np.push_back( nc );
  }
  return np;
}

ObjectImp* FilledPolygonImp::transform( const Transformation& t ) const
{
  std::vector<Coordinate> np = ptransform( t );
  if ( np.size() != mnpoints ) return new InvalidImp;
  return new FilledPolygonImp( np );
}

ObjectImp* ClosedPolygonalImp::transform( const Transformation& t ) const
{
  std::vector<Coordinate> np = ptransform( t );
  if ( np.size() != mnpoints ) return new InvalidImp;
  return new ClosedPolygonalImp( np );
}

ObjectImp* OpenPolygonalImp::transform( const Transformation& t ) const
{
  std::vector<Coordinate> np = ptransform( t );
  if ( np.size() != mnpoints ) return new InvalidImp;
  return new OpenPolygonalImp( np );
}

bool AbstractPolygonImp::isInPolygon( const Coordinate& p ) const
{
  // (algorithm sent to me by domi)
  // We intersect with the horizontal ray from point to the right and
  // count the number of intersections.  That, along with some
  // minor optimalisations of the intersection test..
  bool inside_flag = false;
  double cx = p.x;
  double cy = p.y;

  Coordinate prevpoint = mpoints.back();
  bool prevpointbelow = mpoints.back().y >= cy;
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    Coordinate point = mpoints[i];
    bool pointbelow = point.y >= cy;
    if ( prevpointbelow != pointbelow )
    {
      // possibility of intersection: points on different side from
      // the X axis
      //bool rightofpt = point.x >= cx;
      // mp: we need to be a little bit more conservative here, in
      // order to treat properly the case when the point is on the
      // boundary
      //if ( rightofpt == ( prevpoint.x >= cx ) )
      if ( ( point.x - cx )*(prevpoint.x - cx ) > 0 )
      {
        // points on same side of Y axis -> easy to test intersection
        // intersection iff one point to the right of c
        if ( point.x >= cx )
          inside_flag = !inside_flag;
      }
      else
      {
        // points on different sides of Y axis -> we need to calculate
        // the intersection.
        // mp: we want to check if the point is on the boundary, and
        // return false in such case
        double num = ( point.y - cy )*( prevpoint.x - point.x );
        double den = prevpoint.y - point.y;
	if ( num == den*( point.x - cx ) ) return false;
        if ( num/den <= point.x - cx )
          inside_flag = !inside_flag;
      }
    }
    prevpoint = point;
    prevpointbelow = pointbelow;
  }
  return inside_flag;
}

bool AbstractPolygonImp::isOnCPolygonBorder( const Coordinate& p, double dist, const KigDocument& doc ) const
{
  uint reduceddim = mpoints.size() - 1;

  if ( isOnSegment( p, mpoints[reduceddim], mpoints[0], dist ) )
    return true;

  return isOnOPolygonBorder( p, dist, doc );
}

bool AbstractPolygonImp::isOnOPolygonBorder( const Coordinate& p, double dist, const KigDocument& ) const
{
  bool ret = false;
  uint reduceddim = mpoints.size() - 1;
  for ( uint i = 0; i < reduceddim; ++i )
  {
    ret |= isOnSegment( p, mpoints[i], mpoints[i+1], dist );
  }

  return ret;
}

bool AbstractPolygonImp::inRect( const Rect& r, int width, const KigWidget& w ) const
{
  bool ret = false;
  uint reduceddim = mpoints.size() - 1;
  for ( uint i = 0; !ret && i < reduceddim; ++i )
  {
    SegmentImp s( mpoints[i], mpoints[i+1] );
    ret = lineInRect( r, mpoints[i], mpoints[i+1], width, &s, w );
  }
  if ( !ret )
  {
    SegmentImp s( mpoints[reduceddim], mpoints[0] );
    ret = lineInRect( r, mpoints[reduceddim], mpoints[0], width, &s, w );
  }

  return ret;
}

bool AbstractPolygonImp::valid() const
{
  return true;
}

int AbstractPolygonImp::numberOfProperties() const
{
  return Parent::numberOfProperties();
}

int FilledPolygonImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 7;
}

int ClosedPolygonalImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 7;
}

int OpenPolygonalImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 5;
}

const QByteArrayList AbstractPolygonImp::propertiesInternalNames() const
{
  return Parent::propertiesInternalNames();
}

const QByteArrayList FilledPolygonImp::propertiesInternalNames() const
{
  QByteArrayList l = Parent::propertiesInternalNames();
  l += "polygon-number-of-sides";
  l += "polygon-perimeter";
  l += "polygon-surface";
  l += "closed-polygonal";
  l += "polygonal";
  l += "polygon-center-of-mass";
  l += "polygon-winding-number";
  assert( l.size() == FilledPolygonImp::numberOfProperties() );
  return l;
}

const QByteArrayList ClosedPolygonalImp::propertiesInternalNames() const
{
  QByteArrayList l = Parent::propertiesInternalNames();
  l += "number-of-sides";
  l += "polygon-perimeter";
  l += "polygon-surface";
  l += "polygon";
  l += "polygonal";
  l += "polygon-center-of-mass";
  l += "polygon-winding-number";
  assert( l.size() == ClosedPolygonalImp::numberOfProperties() );
  return l;
}

const QByteArrayList OpenPolygonalImp::propertiesInternalNames() const
{
  QByteArrayList l = Parent::propertiesInternalNames();
  l += "number-of-sides";
  l += "length";
  l += "bezier-curve";
  l += "polygon";
  l += "closed-polygonal";
  assert( l.size() == OpenPolygonalImp::numberOfProperties() );
  return l;
}

const QByteArrayList AbstractPolygonImp::properties() const
{
  return Parent::properties();
}

const QByteArrayList FilledPolygonImp::properties() const
{
  QByteArrayList l = Parent::properties();
  l += I18N_NOOP( "Number of sides" );
  l += I18N_NOOP( "Perimeter" );
  l += I18N_NOOP( "Surface" );
  l += I18N_NOOP( "Boundary Polygonal" );
  l += I18N_NOOP( "Open Boundary Polygonal" );
  l += I18N_NOOP( "Center of Mass of the Vertices" );
  l += I18N_NOOP( "Winding Number" );
  assert( l.size() == FilledPolygonImp::numberOfProperties() );
  return l;
}

const QByteArrayList ClosedPolygonalImp::properties() const
{
  QByteArrayList l = Parent::properties();
  l += I18N_NOOP( "Number of sides" );
  l += I18N_NOOP( "Perimeter" );
  l += I18N_NOOP( "Surface" );
  l += I18N_NOOP( "Inside Polygon" );
  l += I18N_NOOP( "Open Polygonal Curve" );
  l += I18N_NOOP( "Center of Mass of the Vertices" );
  l += I18N_NOOP( "Winding Number" );
  assert( l.size() == ClosedPolygonalImp::numberOfProperties() );
  return l;
}

const QByteArrayList OpenPolygonalImp::properties() const
{
  QByteArrayList l = Parent::properties();
  l += I18N_NOOP( "Number of sides" );
  l += I18N_NOOP( "Length" );
  l += I18N_NOOP( "Bézier Curve" );
  l += I18N_NOOP( "Associated Polygon" );
  l += I18N_NOOP( "Closed Polygonal Curve" );
  assert( l.size() == OpenPolygonalImp::numberOfProperties() );
  return l;
}

const ObjectImpType* AbstractPolygonImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return AbstractPolygonImp::stype();
}

const ObjectImpType* FilledPolygonImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return FilledPolygonImp::stype();
}

const ObjectImpType* ClosedPolygonalImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return ClosedPolygonalImp::stype();
}

const ObjectImpType* OpenPolygonalImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return OpenPolygonalImp::stype();
}

const char* AbstractPolygonImp::iconForProperty( int which ) const
{
  assert( which < AbstractPolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else assert( false );
  return "";
}

const char* FilledPolygonImp::iconForProperty( int which ) const
{
  assert( which < FilledPolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "en"; // number of sides
  else if ( which == Parent::numberOfProperties() + 1 )
    return "circumference"; // perimeter
  else if ( which == Parent::numberOfProperties() + 2 )
    return "areaCircle"; // surface
  else if ( which == Parent::numberOfProperties() + 3 )
    return "kig_polygon"; // closed polygonal (minside = true) or polygon
  else if ( which == Parent::numberOfProperties() + 4 )
    return "openpolygon"; // open polygonal
  else if ( which == Parent::numberOfProperties() + 5 )
    return "point"; // center of mass
  else if ( which == Parent::numberOfProperties() + 6 )
    return "w"; // winding number
  else assert( false );
  return "";
}

const char* ClosedPolygonalImp::iconForProperty( int which ) const
{
  assert( which < ClosedPolygonalImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "en"; // number of sides
  else if ( which == Parent::numberOfProperties() + 1 )
    return "circumference"; // perimeter
  else if ( which == Parent::numberOfProperties() + 2 )
    return "areaCircle"; // surface
  else if ( which == Parent::numberOfProperties() + 3 )
    return "kig_polygon"; // closed polygonal (minside = true) or polygon
  else if ( which == Parent::numberOfProperties() + 4 )
    return "openpolygon"; // open polygonal
  else if ( which == Parent::numberOfProperties() + 5 )
    return "point"; // center of mass
  else if ( which == Parent::numberOfProperties() + 6 )
    return "w"; // winding number
  else assert( false );
  return "";
}

const char* OpenPolygonalImp::iconForProperty( int which ) const
{
  assert( which < OpenPolygonalImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "en"; // number of sides
  else if ( which == Parent::numberOfProperties() + 1 )
    return "circumference"; // perimeter
  else if ( which == Parent::numberOfProperties() + 2 )
    return "bezierN"; // Bezier curve
  else if ( which == Parent::numberOfProperties() + 3 )
    return "kig_polygon"; // closed polygonal (minside = true) or polygon
  else if ( which == Parent::numberOfProperties() + 4 )
    return "kig_polygon"; // closed polygonal
  else assert( false );
  return "";
}

ObjectImp* AbstractPolygonImp::property( int which, const KigDocument& w ) const
{
  assert( which < AbstractPolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else assert( false );
  return new InvalidImp;
}

ObjectImp* FilledPolygonImp::property( int which, const KigDocument& w ) const
{
  assert( which < FilledPolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else if ( which == Parent::numberOfProperties() )
  {
    // number of sides
    return new IntImp( mnpoints );
  }
  else if ( which == Parent::numberOfProperties() + 1)
  {
    // perimeter
    return new DoubleImp( cperimeter () );
  }
  else if ( which == Parent::numberOfProperties() + 2)
  {
    int wn = windingNumber ();  // not able to compute area for such polygons...
    if ( wn < 0 ) wn = -wn;
    if ( wn != 1 ) return new InvalidImp;
    return new DoubleImp( fabs( area () ) );
  }
  else if ( which == Parent::numberOfProperties() + 3)
  {
    return new ClosedPolygonalImp( mpoints ); // polygon boundary
  }
  else if ( which == Parent::numberOfProperties() + 4)
  {
    return new OpenPolygonalImp( mpoints ); // open polygonal curve
  }
  else if ( which == Parent::numberOfProperties() + 5 )
  {
    return new PointImp( mcenterofmass );
  }
  else if ( which == Parent::numberOfProperties() + 6 )
  {
    // winding number
    return new IntImp( windingNumber() );
  }
  else assert( false );
  return new InvalidImp;
}

ObjectImp* ClosedPolygonalImp::property( int which, const KigDocument& w ) const
{
  assert( which < ClosedPolygonalImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else if ( which == Parent::numberOfProperties() )
  {
    // number of sides
    return new IntImp( mnpoints );
  }
  else if ( which == Parent::numberOfProperties() + 1)
  {
    // perimeter
    return new DoubleImp( cperimeter () );
  }
  else if ( which == Parent::numberOfProperties() + 2)
  {
    int wn = windingNumber ();  // not able to compute area for such polygons...
    if ( wn < 0 ) wn = -wn;
    if ( wn != 1 ) return new InvalidImp;
    return new DoubleImp( fabs( area () ) );
  }
  else if ( which == Parent::numberOfProperties() + 3)
  {
    return new FilledPolygonImp( mpoints ); // filled polygon
  }
  else if ( which == Parent::numberOfProperties() + 4)
  {
    return new OpenPolygonalImp( mpoints ); // open polygonal curve
  }
  else if ( which == Parent::numberOfProperties() + 5 )
  {
    return new PointImp( mcenterofmass );
  }
  else if ( which == Parent::numberOfProperties() + 6 )
  {
    // winding number
    return new IntImp( windingNumber() );
  }
  else assert( false );
  return new InvalidImp;
}

ObjectImp* OpenPolygonalImp::property( int which, const KigDocument& w ) const
{
  assert( which < OpenPolygonalImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else if ( which == Parent::numberOfProperties() )
  {
    // number of sides
    return new IntImp( mnpoints - 1 );
  }
  else if ( which == Parent::numberOfProperties() + 1)
  {
    // perimeter
    return new DoubleImp( operimeter () );
  }
  else if ( which == Parent::numberOfProperties() + 2)
  {
    return new BezierImp( mpoints ); // bezier curve
  }
  else if ( which == Parent::numberOfProperties() + 3)
  {
    return new FilledPolygonImp( mpoints ); // filled polygon
  }
  else if ( which == Parent::numberOfProperties() + 4)
  {
    return new ClosedPolygonalImp( mpoints ); // polygon boundary
  }
  else assert( false );
  return new InvalidImp;
}

const std::vector<Coordinate> AbstractPolygonImp::points() const
{
  return mpoints;
}

uint AbstractPolygonImp::npoints() const
{
  return mnpoints;
}

double AbstractPolygonImp::operimeter() const
{
  double perimeter = 0.;
  for ( uint i = 0; i < mpoints.size() - 1; ++i )
  {
    perimeter += ( mpoints[i+1] - mpoints[i] ).length();
  }
  return perimeter;
}

double AbstractPolygonImp::cperimeter() const
{
  return operimeter() + ( mpoints[0] - mpoints[mpoints.size()-1] ).length();
}

/*
 * returns the *signed* area, this has a result even if the
 * polygon is selfintersecting.  On the contrary, the "area"
 * property returns an InvalidObject in such case.
 */

double AbstractPolygonImp::area() const
{
  double surface2 = 0.0;
  Coordinate prevpoint = mpoints.back();
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    Coordinate point = mpoints[i];
    surface2 += ( point.x - prevpoint.x ) * ( point.y + prevpoint.y ); 
    prevpoint = point;
  }
  return -surface2/2;  /* positive if counterclockwise */
}

FilledPolygonImp* FilledPolygonImp::copy() const
{
  return new FilledPolygonImp( mpoints );
}

ClosedPolygonalImp* ClosedPolygonalImp::copy() const
{
  return new ClosedPolygonalImp( mpoints );
}

OpenPolygonalImp* OpenPolygonalImp::copy() const
{
  return new OpenPolygonalImp( mpoints );
}

void FilledPolygonImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

void ClosedPolygonalImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

void OpenPolygonalImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool AbstractPolygonImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( AbstractPolygonImp::stype() ) &&
    static_cast<const AbstractPolygonImp&>( rhs ).points() == mpoints;
}

const ObjectImpType* AbstractPolygonImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "abstractpolygon",
    I18N_NOOP( "polygon" ),
    I18N_NOOP( "Select this polygon" ), 0, 0, 0, 0, 0, 0, 0 );
  return &t;
}

const ObjectImpType* FilledPolygonImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "polygon",
    I18N_NOOP( "polygon" ),
    I18N_NOOP( "Select this polygon" ),
    I18N_NOOP( "Select polygon %1" ),
    I18N_NOOP( "Remove a Polygon" ),
    I18N_NOOP( "Add a Polygon" ),
    I18N_NOOP( "Move a Polygon" ),
    I18N_NOOP( "Attach to this polygon" ),
    I18N_NOOP( "Show a Polygon" ),
    I18N_NOOP( "Hide a Polygon" )
    );

  return &t;
}

const ObjectImpType* ClosedPolygonalImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "closedpolygonal",
    I18N_NOOP( "closed polygonal curve" ),
    I18N_NOOP( "Select this closed polygonal curve" ),
    I18N_NOOP( "Select closed polygonal curve %1" ),
    I18N_NOOP( "Remove a closed polygonal curve" ),
    I18N_NOOP( "Add a closed polygonal curve" ),
    I18N_NOOP( "Move a closed polygonal curve" ),
    I18N_NOOP( "Attach to this closed polygonal curve" ),
    I18N_NOOP( "Show a closed polygonal curve" ),
    I18N_NOOP( "Hide a closed polygonal curve" )
    );

  return &t;
}

const ObjectImpType* OpenPolygonalImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "polygonal",
    I18N_NOOP( "polygonal curve" ),
    I18N_NOOP( "Select this polygonal curve" ),
    I18N_NOOP( "Select polygonal curve %1" ),
    I18N_NOOP( "Remove a polygonal curve" ),
    I18N_NOOP( "Add a polygonal curve" ),
    I18N_NOOP( "Move a polygonal curve" ),
    I18N_NOOP( "Attach to this polygonal curve" ),
    I18N_NOOP( "Show a polygonal curve" ),
    I18N_NOOP( "Hide a polygonal curve" )
    );

  return &t;
}

const ObjectImpType* FilledPolygonImp::stype3()
{
  static const ObjectImpType t3(
    FilledPolygonImp::stype(), "triangle",
    I18N_NOOP( "triangle" ),
    I18N_NOOP( "Select this triangle" ),
    I18N_NOOP( "Select triangle %1" ),
    I18N_NOOP( "Remove a Triangle" ),
    I18N_NOOP( "Add a Triangle" ),
    I18N_NOOP( "Move a Triangle" ),
    I18N_NOOP( "Attach to this triangle" ),
    I18N_NOOP( "Show a Triangle" ),
    I18N_NOOP( "Hide a Triangle" )
    );

  return &t3;
}

const ObjectImpType* FilledPolygonImp::stype4()
{
  static const ObjectImpType t4(
    FilledPolygonImp::stype(), "quadrilateral",
    I18N_NOOP( "quadrilateral" ),
    I18N_NOOP( "Select this quadrilateral" ),
    I18N_NOOP( "Select quadrilateral %1" ),
    I18N_NOOP( "Remove a Quadrilateral" ),
    I18N_NOOP( "Add a Quadrilateral" ),
    I18N_NOOP( "Move a Quadrilateral" ),
    I18N_NOOP( "Attach to this quadrilateral" ),
    I18N_NOOP( "Show a Quadrilateral" ),
    I18N_NOOP( "Hide a Quadrilateral" )
    );

  return &t4;
}

const ObjectImpType* FilledPolygonImp::type() const
{
  uint n = mnpoints;

  if ( n == 3 ) return FilledPolygonImp::stype3();
  if ( n == 4 ) return FilledPolygonImp::stype4();
  return FilledPolygonImp::stype();
}

const ObjectImpType* ClosedPolygonalImp::type() const
{
  return ClosedPolygonalImp::stype();
}

const ObjectImpType* OpenPolygonalImp::type() const
{
  return OpenPolygonalImp::stype();
}

bool AbstractPolygonImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  assert( which < AbstractPolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

bool FilledPolygonImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  assert( which < FilledPolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

bool ClosedPolygonalImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  assert( which < ClosedPolygonalImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

bool OpenPolygonalImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  assert( which < OpenPolygonalImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

Rect AbstractPolygonImp::surroundingRect() const
{
  Rect r( 0., 0., 0., 0. );
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    r.setContains( mpoints[i] );
  }
  return r;
}

int AbstractPolygonImp::windingNumber() const
{
  /*
   * this is defined as the sum of the external angles while at
   * all vertices, then normalized by 2pi.  The external angle
   * is the angle we steer at each vertex while we walk along the
   * boundary of the polygon.
   * In the end we only need to count how many time we cross the (1,0)
   * direction (positive x-axis) with a positive sign if we cross while
   * steering left and a negative sign viceversa
   */

  int winding = 0;
  uint npoints = mpoints.size();
  Coordinate prevside = mpoints[0] - mpoints[npoints-1];
  for ( uint i = 0; i < npoints; ++i )
  {
    uint nexti = i + 1;
    if ( nexti >= npoints ) nexti = 0;
    Coordinate side = mpoints[nexti] - mpoints[i];
    double vecprod = side.x*prevside.y - side.y*prevside.x;
    int steeringdir = ( vecprod > 0 ) ? 1 : -1;
    if ( vecprod == 0.0 || side.y*prevside.y > 0 )
    {
      prevside = side;
      continue;   // cannot cross the (1,0) direction
    }
    if ( side.y*steeringdir < 0 && prevside.y*steeringdir >= 0 )
      winding -= steeringdir;
    prevside = side;
  }
  return winding;
}

bool AbstractPolygonImp::isTwisted() const
{
  /*
   * returns true if this is a "twisted" polygon, i.e.
   * with selfintersecting sides
   */

  std::vector<Coordinate>::const_iterator ia, ib, ic, id;
  double abx, aby, cdx, cdy, acx, acy, adx, ady, cax, cay, cbx, cby;
  bool pointbelow, prevpointbelow;

  if ( mpoints.size() <= 3 ) return false;
  ia = mpoints.end() - 1;

  for ( ib = mpoints.begin(); ib + 1 != mpoints.end(); ++ib)
  {
    abx = ib->x - ia->x;
    aby = ib->y - ia->y;
    ic = ib + 1;
    acx = ic->x - ia->x;
    acy = ic->y - ia->y;
    prevpointbelow = ( abx*acy <= aby*acx );

    for ( id = ib + 2; id != mpoints.end(); ++id)
    {
      if ( id == ia ) break;
      adx = id->x - ia->x;
      ady = id->y - ia->y;
      pointbelow = ( abx*ady <= aby*adx );
      if ( prevpointbelow != pointbelow )
      {
        /* il segmento cd interseca il supporto di ab */
        cdx = id->x - ic->x;
        cdy = id->y - ic->y;
        cax = ia->x - ic->x;
        cay = ia->y - ic->y;
        cbx = ib->x - ic->x;
        cby = ib->y - ic->y;
        if ( ( cdx*cay <= cdy*cax ) != ( cdx*cby <= cdy*cbx ) ) return true;
      }
      prevpointbelow = pointbelow;
      ic = id;
    }
    ia = ib;
  }
  return false;
}

bool AbstractPolygonImp::isMonotoneSteering() const
{
  /*
   * returns true if while walking along the boundary,
   * steering is always in the same direction
   */

  uint npoints = mpoints.size();
  Coordinate prevside = mpoints[0] - mpoints[npoints-1];
  int prevsteeringdir = 0;
  for ( uint i = 0; i < npoints; ++i )
  {
    uint nexti = i + 1;
    if ( nexti >= npoints ) nexti = 0;
    Coordinate side = mpoints[nexti] - mpoints[i];
    double vecprod = side.x*prevside.y - side.y*prevside.x;
    int steeringdir = ( vecprod > 0 ) ? 1 : -1;
    if ( vecprod == 0.0 )
    {
      prevside = side;
      continue;   // going straight
    }
    if ( prevsteeringdir*steeringdir < 0 ) return false;
    prevside = side;
    prevsteeringdir = steeringdir;
  }
  return true;
}

bool AbstractPolygonImp::isConvex() const
{
  if ( ! isMonotoneSteering() ) return false;
  int winding = windingNumber();
  if ( winding < 0 ) winding = -winding;
  assert ( winding > 0 );
  return winding == 1;
}

/*
 * end of abstract type, start three real types
 */

FilledPolygonImp::FilledPolygonImp( const std::vector<Coordinate>& points )
  : AbstractPolygonImp( points )
{
}

void FilledPolygonImp::draw( KigPainter& p ) const
{
  p.drawPolygon( mpoints );
}

bool FilledPolygonImp::contains( const Coordinate& p, int,
         const KigWidget& ) const
{
  return isInPolygon( p );
}

ClosedPolygonalImp::ClosedPolygonalImp( const std::vector<Coordinate>& points )
  : AbstractPolygonImp( points )
{
}

void ClosedPolygonalImp::draw( KigPainter& p ) const
{
  for ( unsigned int i = 0; i < mnpoints - 1; i++ )
    p.drawSegment( mpoints[i], mpoints[i+1] );
  p.drawSegment( mpoints[mnpoints-1], mpoints[0] );
}

bool ClosedPolygonalImp::contains( const Coordinate& p, int width,
         const KigWidget& w ) const
{
  return isOnCPolygonBorder( p, w.screenInfo().normalMiss( width ),  w.document() );
}

OpenPolygonalImp::OpenPolygonalImp( const std::vector<Coordinate>& points )
  : AbstractPolygonImp( points )
{
}

void OpenPolygonalImp::draw( KigPainter& p ) const
{
  for ( unsigned int i = 0; i < mnpoints - 1; i++ )
    p.drawSegment( mpoints[i], mpoints[i+1] );
}

bool OpenPolygonalImp::contains( const Coordinate& p, int width,
         const KigWidget& w ) const
{
  return isOnOPolygonBorder( p, w.screenInfo().normalMiss( width ),  w.document() );
}

/*
 *
 */

std::vector<Coordinate> computeConvexHull( const std::vector<Coordinate>& points )
{
  /*
   * compute the convex hull of the set of points, the resulting list
   * is the vertices of the resulting polygon listed in a counter clockwise
   * order.  This algorithm is on order n^2, probably suboptimal, but
   * we don't expect to have large values for n.
   */

  if ( points.size() < 3 ) return points;
  std::vector<Coordinate> worklist = points;
  std::vector<Coordinate> result;

  double ymin = worklist[0].y;
  uint imin = 0;
  for ( uint i = 1; i < worklist.size(); ++i )
  {
    if ( worklist[i].y < ymin )
    {
      ymin = worklist[i].y;
      imin = i;
    }
  }

  // worklist[imin] is definitely on the convex hull, let's start from there
  result.push_back( worklist[imin] );
  Coordinate startpoint = worklist[imin];
  Coordinate apoint = worklist[imin];
  double aangle = 0.0;

  while ( ! worklist.empty() )
  {
    int besti = -1;
    double anglemin = 10000.0;
    for ( uint i = 0; i < worklist.size(); ++i )
    {
      if ( worklist[i] == apoint ) continue;
      Coordinate v = worklist[i] - apoint;
      double angle = std::atan2( v.y, v.x );
      while ( angle < aangle ) angle += 2*M_PI;
      if ( angle < anglemin )
      {   // found a better point
        besti = i;
        anglemin = angle;
      }
    }

    if ( besti < 0 ) return result;   // this happens, e.g. if all points coincide
    apoint = worklist[besti];
    aangle = anglemin;
    if ( apoint == startpoint )
    {
      return result;
    }
    result.push_back( apoint );
    worklist.erase( worklist.begin() + besti, worklist.begin() + besti + 1 );
  }
  assert( false );
  return result;
}
