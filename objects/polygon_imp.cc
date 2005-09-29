// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>

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

#include "polygon_imp.h"

#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"

#include "../misc/common.h"
#include "../misc/coordinate.h"
#include "../misc/kigpainter.h"
#include "../misc/kigtransform.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"

#include <klocale.h>

#include <cmath>

PolygonImp::PolygonImp( uint npoints, const std::vector<Coordinate>& points,
                        const Coordinate& centerofmass )
  : mnpoints( npoints ), mpoints( points ), mcenterofmass( centerofmass )
{
//  mpoints = points;
}

PolygonImp::PolygonImp( const std::vector<Coordinate>& points )
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

PolygonImp::~PolygonImp()
{
}

Coordinate PolygonImp::attachPoint() const
{
  return mcenterofmass;
}

ObjectImp* PolygonImp::transform( const Transformation& t ) const
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
    if ( maxp > 0 && minp < 0 ) return new InvalidImp;
  }
  std::vector<Coordinate> np;
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    Coordinate nc = t.apply( mpoints[i] );
    if ( !nc.valid() )
      return new InvalidImp;
    np.push_back( nc );
  }
  return new PolygonImp( np );
}

void PolygonImp::draw( KigPainter& p ) const
{
  p.drawPolygon( mpoints );
}

bool PolygonImp::isInPolygon( const Coordinate& p ) const
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
#define selectpolygonwithinside 1
#ifdef selectpolygonwithinside
bool PolygonImp::contains( const Coordinate& p, int, const KigWidget& ) const
{
  return isInPolygon( p );
}
#else
bool PolygonImp::contains( const Coordinate& p, int width, const KigWidget& w ) const
{
  bool ret = false;
  uint reduceddim = mpoints.size() - 1;
  for ( uint i = 0; i < reduceddim; ++i )
  {
    ret |= isOnSegment( p, mpoints[i], mpoints[i+1], w.screenInfo().normalMiss( width ) );
  }
  ret |= isOnSegment( p, mpoints[reduceddim], mpoints[0], w.screenInfo().normalMiss( width ) );

  return ret;
}
#endif

bool PolygonImp::inRect( const Rect& r, int width, const KigWidget& w ) const
{
  bool ret = false;
  uint reduceddim = mpoints.size() - 1;
  for ( uint i = 0; i < reduceddim; ++i )
  {
    SegmentImp* s = new SegmentImp( mpoints[i], mpoints[i+1] );
    ret |= lineInRect( r, mpoints[i], mpoints[i+1], width, s, w );
    delete s;
    s = 0;
  }
  SegmentImp* t = new SegmentImp( mpoints[reduceddim], mpoints[0] );
  ret |= lineInRect( r, mpoints[reduceddim], mpoints[0], width, t, w );
  delete t;
  t = 0;

  return ret;
}

bool PolygonImp::valid() const
{
  return true;
}

const uint PolygonImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 5;
}

const QCStringList PolygonImp::propertiesInternalNames() const
{
  QCStringList l = Parent::propertiesInternalNames();
  l += "polygon-number-of-sides";
  l += "polygon-perimeter";
  l += "polygon-surface";
  l += "polygon-center-of-mass";
  l += "polygon-winding-number";
  assert( l.size() == PolygonImp::numberOfProperties() );
  return l;
}

const QCStringList PolygonImp::properties() const
{
  QCStringList l = Parent::properties();
  l += I18N_NOOP( "Number of sides" );
  l += I18N_NOOP( "Perimeter" );
  l += I18N_NOOP( "Surface" );
  l += I18N_NOOP( "Center of Mass of the Vertices" );
  l += I18N_NOOP( "Winding Number" );
  assert( l.size() == PolygonImp::numberOfProperties() );
  return l;
}

const ObjectImpType* PolygonImp::impRequirementForProperty( uint which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return PolygonImp::stype();
}

const char* PolygonImp::iconForProperty( uint which ) const
{
  assert( which < PolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "en"; // number of sides
  else if ( which == Parent::numberOfProperties() + 1 )
    return "circumference"; // perimeter
  else if ( which == Parent::numberOfProperties() + 2 )
    return "areaCircle"; // surface
  else if ( which == Parent::numberOfProperties() + 3 )
    return "point"; // center of mass
  else if ( which == Parent::numberOfProperties() + 4 )
    return "w"; // winding number
  else assert( false );
  return "";
}

ObjectImp* PolygonImp::property( uint which, const KigDocument& w ) const
{
  assert( which < PolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else if ( which == Parent::numberOfProperties() )
  {
    // number of points
    return new IntImp( mnpoints );
  }
  else if ( which == Parent::numberOfProperties() + 1)
  {
    double circumference = 0.;
    // circumference
    for ( uint i = 0; i < mpoints.size(); ++i )
    {
      uint prev = ( i + mpoints.size() - 1 ) % mpoints.size();
      circumference += ( mpoints[i] - mpoints[prev] ).length();
    }
    return new DoubleImp( circumference );
  }
  else if ( which == Parent::numberOfProperties() + 2)
  {
    int wn = windingNumber ();  // not able to compute area for such polygons...
    if ( wn < 0 ) wn = -wn;
    if ( wn != 1 ) return new InvalidImp;
    double surface2 = 0.0;
    Coordinate prevpoint = mpoints.back();
    for ( uint i = 0; i < mpoints.size(); ++i )
    {
      Coordinate point = mpoints[i];
      surface2 += ( point.x - prevpoint.x ) * ( point.y + prevpoint.y ); 
      prevpoint = point;
    }
    return new DoubleImp( fabs( surface2 / 2 ) );
  }
  else if ( which == Parent::numberOfProperties() + 3 )
  {
    return new PointImp( mcenterofmass );
  }
  else if ( which == Parent::numberOfProperties() + 4 )
  {
    // winding number
    return new IntImp( windingNumber() );
  }
  else assert( false );
  return new InvalidImp;
}

const std::vector<Coordinate> PolygonImp::points() const
{
  std::vector<Coordinate> np;
  np.reserve( mpoints.size() );
  std::copy( mpoints.begin(), mpoints.end(), std::back_inserter( np ) );
  return np;
}

const uint PolygonImp::npoints() const
{
  return mnpoints;
}

PolygonImp* PolygonImp::copy() const
{
  return new PolygonImp( mpoints );
}

void PolygonImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool PolygonImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( PolygonImp::stype() ) &&
    static_cast<const PolygonImp&>( rhs ).points() == mpoints;
}

const ObjectImpType* PolygonImp::stype()
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

const ObjectImpType* PolygonImp::stype3()
{
  static const ObjectImpType t3(
    PolygonImp::stype(), "triangle",
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

const ObjectImpType* PolygonImp::stype4()
{
  static const ObjectImpType t4(
    PolygonImp::stype(), "quadrilateral",
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

const ObjectImpType* PolygonImp::type() const
{
  uint n = mpoints.size();

  if ( n == 3 ) return PolygonImp::stype3();
  if ( n == 4 ) return PolygonImp::stype4();
  return PolygonImp::stype();
}

bool PolygonImp::isPropertyDefinedOnOrThroughThisImp( uint which ) const
{
  assert( which < PolygonImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

Rect PolygonImp::surroundingRect() const
{
  Rect r( 0., 0., 0., 0. );
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    r.setContains( mpoints[i] );
  }
  return r;
}

int PolygonImp::windingNumber() const
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

bool PolygonImp::isMonotoneSteering() const
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

bool PolygonImp::isConvex() const
{
  if ( ! isMonotoneSteering() ) return false;
  int winding = windingNumber();
  if ( winding < 0 ) winding = -winding;
  assert ( winding > 0 );
  return winding == 1;
}

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
