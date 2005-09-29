// Copyright (C)  2003  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#include "polygon_type.h"

#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"
#include "polygon_imp.h"
#include "object_calcer.h"

#include "../misc/common.h"

#include <klocale.h>
#include <cmath>
#include <vector>

/*
 * triangle by its vertices
 */

static const char triangle_constructstatement[] = I18N_NOOP( "Construct a triangle with this vertex" );
static const char triangle_constructstatement2[] = I18N_NOOP( "Select a point to be a vertex of the new triangle..." );

static const struct ArgsParser::spec argsspecTriangleB3P[] =
{
  { PointImp::stype(), triangle_constructstatement, triangle_constructstatement2, true },
  { PointImp::stype(), triangle_constructstatement, triangle_constructstatement2, true },
  { PointImp::stype(), triangle_constructstatement, triangle_constructstatement2, true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( TriangleB3PType )

TriangleB3PType::TriangleB3PType()
  : ArgsParserObjectType( "TriangleB3P", argsspecTriangleB3P, 3 )
{
}

TriangleB3PType::~TriangleB3PType()
{
}

const TriangleB3PType* TriangleB3PType::instance()
{
  static const TriangleB3PType s;
  return &s;
}

ObjectImp* TriangleB3PType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents, 1 ) ) return new InvalidImp;
  std::vector<Coordinate> points;

  Coordinate centerofmass3 = Coordinate( 0, 0 );
  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
  {
    Coordinate point = static_cast<const PointImp*>( *i )->coordinate();
    centerofmass3 += point;
    points.push_back( point );
  }
  return new PolygonImp( 3, points, centerofmass3/3 );
}

const ObjectImpType* TriangleB3PType::resultId() const
{
  return PolygonImp::stype();
}

bool TriangleB3PType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
}

bool TriangleB3PType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return parents[0]->isFreelyTranslatable() && 
         parents[1]->isFreelyTranslatable() &&
         parents[2]->isFreelyTranslatable();
}

void TriangleB3PType::move( ObjectTypeCalcer& o, const Coordinate& to,
                         const KigDocument& d ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  const Coordinate a = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  const Coordinate b = static_cast<const PointImp*>( parents[1]->imp() )->coordinate();
  const Coordinate c = static_cast<const PointImp*>( parents[2]->imp() )->coordinate();
  if ( parents[0]->canMove() )
    parents[0]->move( to, d );
  if ( parents[1]->canMove() )
    parents[1]->move( to + b - a, d );
  if ( parents[2]->canMove() )
    parents[2]->move( to + c - a, d );
}

const Coordinate TriangleB3PType::moveReferencePoint( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> TriangleB3PType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  std::set<ObjectCalcer*> ret;
  std::vector<ObjectCalcer*> tmp = parents[0]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[1]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[2]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  ret.insert( parents.begin(), parents.end() );
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}

/*
 * generic polygon
 */

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PolygonBNPType )

PolygonBNPType::PolygonBNPType()
  : ObjectType( "PolygonBNP" )
{
}

PolygonBNPType::~PolygonBNPType()
{
}

const PolygonBNPType* PolygonBNPType::instance()
{
  static const PolygonBNPType s;
  return &s;
}

ObjectImp* PolygonBNPType::calc( const Args& parents, const KigDocument& ) const
{
  uint count = parents.size();
  assert (count >= 3); /* non sono ammessi poligoni con meno di tre lati */
//  if ( parents[0] != parents[count] ) return new InvalidImp;
  std::vector<Coordinate> points;

  uint npoints = 0;
  Coordinate centerofmassn = Coordinate( 0, 0 );

  for ( uint i = 0; i < count; ++i )
  {
    npoints++;
    if ( ! parents[i]->inherits( PointImp::stype() ) ) return new InvalidImp;
    Coordinate point = static_cast<const PointImp*>( parents[i] )->coordinate();
    centerofmassn += point;
    points.push_back( point );
  }
  return new PolygonImp( npoints, points, centerofmassn/npoints );
}

const ObjectImpType* PolygonBNPType::resultId() const
{
  return PolygonImp::stype();
}

const ObjectImpType* PolygonBNPType::impRequirement( const ObjectImp*, const Args& ) const
{
  return PointImp::stype();
}

bool PolygonBNPType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  return false;  /* should be true? */
}

std::vector<ObjectCalcer*> PolygonBNPType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  return args;  /* should already be in correct order */
}

Args PolygonBNPType::sortArgs( const Args& args ) const
{
  return args;
}

bool PolygonBNPType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
}

bool PolygonBNPType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  for ( uint i = 0; i < parents.size(); ++i )
  {
    if ( !parents[i]->isFreelyTranslatable() ) return false;
  }
  return true;
}

void PolygonBNPType::move( ObjectTypeCalcer& o, const Coordinate& to,
                         const KigDocument& d ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  const Coordinate ref = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  for ( uint i = 0; i < parents.size(); ++i )
  {
     const Coordinate a = static_cast<const PointImp*>( parents[i]->imp() )->coordinate();
     parents[i]->move( to + a - ref, d );
  }
}

const Coordinate PolygonBNPType::moveReferencePoint( const ObjectTypeCalcer& o
) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> PolygonBNPType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  std::set<ObjectCalcer*> ret;
  for ( uint i = 0; i < parents.size(); ++i )
  {
    std::vector<ObjectCalcer*> tmp = parents[i]->movableParents();
    ret.insert( tmp.begin(), tmp.end() );
  }
  ret.insert( parents.begin(), parents.end() );
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}

/*
 * regular polygon by center and vertex
 */

//static const char constructpoligonthroughpointstat[] = I18N_NOOP( "Construct a polygon with this vertex" );
//
//static const char constructpoligonwithcenterstat[] = I18N_NOOP( "Construct a polygon with this center" );
//
//static const ArgsParser::spec argsspecPoligonBCV[] =
//{
//  { PointImp::stype(), constructpoligonwithcenterstat,
//    I18N_NOOP( "Select the center of the new polygon..." ), false },
//  { PointImp::stype(), constructpoligonthroughpointstat,
//    I18N_NOOP( "Select a vertex for the new polygon..." ), true },
//  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
//};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PolygonBCVType )

PolygonBCVType::PolygonBCVType()
  : ObjectType( "PoligonBCV" )
// we keep the name "PoligonBCV" although syntactically incorrect for
// compatibility reasons with old kig files
//  : ArgsParserObjectType( "PoligonBCV", argsspecPoligonBCV, 3 )
{
}

PolygonBCVType::~PolygonBCVType()
{
}

const PolygonBCVType* PolygonBCVType::instance()
{
  static const PolygonBCVType s;
  return &s;
}

ObjectImp* PolygonBCVType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() < 3 || parents.size() > 4 ) return new InvalidImp;

  if ( ( ! parents[0]->inherits( PointImp::stype() ) ) ||
       ( ! parents[1]->inherits( PointImp::stype() ) ) ||
       ( ! parents[2]->inherits( IntImp::stype() ) ) )
    return new InvalidImp;

  const Coordinate center = 
        static_cast<const PointImp*>( parents[0] )->coordinate();
  const Coordinate vertex = 
        static_cast<const PointImp*>( parents[1] )->coordinate();
  const int sides =
        static_cast<const IntImp*>( parents[2] )->data();
  int twist = 1;
  if ( parents.size() == 4 )
  {
    if ( ! parents[3]->inherits( IntImp::stype() ) ) return new InvalidImp;
    twist = static_cast<const IntImp*>( parents[3] )->data();
  }
  std::vector<Coordinate> vertexes;

  double dx = vertex.x - center.x;
  double dy = vertex.y - center.y;

  for ( int i = 1; i <= sides; i++ )
  {
    double alfa = 2*twist*M_PI/sides;
    double theta1 = alfa*i - alfa;
    double ctheta1 = cos(theta1);
    double stheta1 = sin(theta1);

    Coordinate v1 = center + Coordinate( ctheta1*dx - stheta1*dy,
                                         stheta1*dx + ctheta1*dy );
    vertexes.push_back( v1 );
  }
  return new PolygonImp( uint (sides), vertexes, center );
}

const ObjectImpType* PolygonBCVType::resultId() const
{
  return SegmentImp::stype();
}

const ObjectImpType* PolygonBCVType::impRequirement( const ObjectImp* obj, const Args& ) const
{
  if ( obj->inherits( PointImp::stype() ) )
    return PointImp::stype();

  if ( obj->inherits( IntImp::stype() ) )
    return IntImp::stype();

  return 0;
}

bool PolygonBCVType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  return false;  /* should be true? */
}

std::vector<ObjectCalcer*> PolygonBCVType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  return args;  /* should already be in correct order */
}

Args PolygonBCVType::sortArgs( const Args& args ) const
{
  return args;
}

bool PolygonBCVType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
}

bool PolygonBCVType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return parents[0]->isFreelyTranslatable() &&
         parents[1]->isFreelyTranslatable();
}

void PolygonBCVType::move( ObjectTypeCalcer& o, const Coordinate& to,
                         const KigDocument& d ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  // assert( margsparser.checkArgs( parents ) );
  if ( ! parents[0]->imp()->inherits( PointImp::stype() ) ||
       ! parents[1]->imp()->inherits( PointImp::stype() ) ) return;

  const Coordinate a = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  const Coordinate b = static_cast<const PointImp*>( parents[1]->imp() )->coordinate();
  parents[0]->move( to, d );
  parents[1]->move( to + b - a, d );
}

const Coordinate PolygonBCVType::moveReferencePoint( const ObjectTypeCalcer& o) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  // assert( margsparser.checkArgs( parents ) );
  if ( ! parents[0]->imp()->inherits( PointImp::stype() ) ) return Coordinate::invalidCoord();

  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> PolygonBCVType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  std::set<ObjectCalcer*> ret;
  std::vector<ObjectCalcer*> tmp = parents[0]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[1]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  ret.insert( &parents[0], &parents[1] );
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}

/* polygon-line intersection */

static const ArgsParser::spec argsspecPolygonLineIntersection[] =
{
  { PolygonImp::stype(), I18N_NOOP( "Intersect this polygon with a line" ),
    I18N_NOOP( "Select the polygon of which you want the intersection with a line..." ), false },
  { AbstractLineImp::stype(), "Intersect this line with a polygon", "Select the line of which you want the intersection with a polygon...", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PolygonLineIntersectionType )

PolygonLineIntersectionType::PolygonLineIntersectionType()
  : ArgsParserObjectType( "PolygonLineIntersection", argsspecPolygonLineIntersection, 2 )
{
}

PolygonLineIntersectionType::~PolygonLineIntersectionType()
{
}

const PolygonLineIntersectionType* PolygonLineIntersectionType::instance()
{
  static const PolygonLineIntersectionType t;
  return &t;
}

/*
 * Intersection of a polygon and a line/ray/segment.
 *
 * geometrically speaking the result is always a collection of
 * collinear nonintersecting open segments (at most one if the
 * polygon is convex).  Since we don't know in advance how many
 * segments will result, the obvious choice is to return an
 * InvalidImp in cases when the result is *not* a single segment
 *
 * computing the two ends of this segment is more tricky then one 
 * expects especially when intersecting segments/rays.
 *
 * particularly "difficult" situations are those where we intersect
 * a segment/ray with an/the endpoint coinciding with a vertex of
 * the polygon, especially if that vertex is a "reentrant" (concave)
 * vertex of the polygon.
 */

ObjectImp* PolygonLineIntersectionType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  const PolygonImp* polygon = static_cast<const PolygonImp*>( parents[0] );
  const std::vector<Coordinate> ppoints = polygon->points();
  const LineData line = static_cast<const AbstractLineImp*>( parents[1] )->data();
  Coordinate intersections[2];
  uint whichintersection = 0;

  bool boundleft = false;
  bool boundright = false;
  if ( parents[1]->inherits( SegmentImp::stype() ) )
  {
    boundleft = boundright = true;
  } 
  if ( parents[1]->inherits( RayImp::stype() ) )
  {
    boundleft = true;
  } 
  Coordinate a = line.a;
  double abx = line.b.x - a.x;
  double aby = line.b.y - a.y;

  double leftendinside = false;
  double rightendinside = false;
  Coordinate prevpoint = ppoints.back() - a;
  bool prevpointbelow = ( abx*prevpoint.y <= aby*prevpoint.x );
  for ( uint i = 0; i < ppoints.size(); ++i )
  {
    Coordinate point = ppoints[i] - a;
    bool pointbelow = ( abx*point.y <= aby*point.x );
    if ( pointbelow != prevpointbelow ) 
    {
      /* found an intersection with the support line
       * compute the value of the parameter...
       */
      double dcx = point.x - prevpoint.x;
      double dcy = point.y - prevpoint.y;
      double num = point.x*dcy - point.y*dcx;
      double den = abx*dcy - aby*dcx;
      if ( std::fabs( den ) <= 1.e-6*std::fabs( num ) ) continue; //parallel
      double t = num/den;
      if ( boundleft && t <= 0 )
      {
        leftendinside = !leftendinside;
      }
      else if ( boundright && t >= 1 )
      {
        rightendinside = !rightendinside;
      }
      else
      {
        if ( whichintersection >= 2 ) return new InvalidImp;
        intersections[whichintersection++] = a + t*Coordinate( abx, aby );
      }
    }
    prevpoint = point;
    prevpointbelow = pointbelow;
  }

  if ( leftendinside )
  {
    if ( whichintersection >= 2 ) return new InvalidImp;
    intersections[whichintersection++] = a;
  }

  if ( rightendinside )
  {
    if ( whichintersection >= 2 ) return new InvalidImp;
    intersections[whichintersection++] = line.b;
  }

  switch (whichintersection)
  {
    case 1:   /* just for completeness: this should never happen */
      return new PointImp( intersections[0] );
      break;
    case 2:
      return new SegmentImp( intersections[0], intersections[1] );
      break;
    case 0:
    default:
      return new InvalidImp;
      break;
  }
}

const ObjectImpType* PolygonLineIntersectionType::resultId() const
{
  return SegmentImp::stype();
}

/* polygon vertices  */

static const ArgsParser::spec argsspecPolygonVertex[] =
{
  { PolygonImp::stype(), I18N_NOOP( "Construct the vertices of this polygon" ),
    I18N_NOOP( "Select the polygon of which you want to construct the vertices..." ), true },
  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PolygonVertexType )

PolygonVertexType::PolygonVertexType()
  : ArgsParserObjectType( "PolygonVertex", argsspecPolygonVertex, 2 )
{
}

PolygonVertexType::~PolygonVertexType()
{
}

const PolygonVertexType* PolygonVertexType::instance()
{
  static const PolygonVertexType t;
  return &t;
}

ObjectImp* PolygonVertexType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  const std::vector<Coordinate> ppoints = static_cast<const PolygonImp*>( parents[0] )->points();
  const uint i = static_cast<const IntImp*>( parents[1] )->data();

  if ( i >= ppoints.size() ) return new InvalidImp;

  return new PointImp( ppoints[i] );
}

const ObjectImpType* PolygonVertexType::resultId() const
{
  return PointImp::stype();
}

/* polygon sides  */

static const ArgsParser::spec argsspecPolygonSide[] =
{
  { PolygonImp::stype(), I18N_NOOP( "Construct the sides of this polygon" ),
    I18N_NOOP( "Select the polygon of which you want to construct the sides..." ), false },
  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PolygonSideType )

PolygonSideType::PolygonSideType()
  : ArgsParserObjectType( "PolygonSide", argsspecPolygonSide, 2 )
{
}

PolygonSideType::~PolygonSideType()
{
}

const PolygonSideType* PolygonSideType::instance()
{
  static const PolygonSideType t;
  return &t;
}

ObjectImp* PolygonSideType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  const std::vector<Coordinate> ppoints = static_cast<const PolygonImp*>( parents[0] )->points();
  const uint i = static_cast<const IntImp*>( parents[1] )->data();

  if ( i >= ppoints.size() ) return new InvalidImp;

  uint nexti = i + 1;
  if ( nexti >= ppoints.size() ) nexti = 0;

  return new SegmentImp( ppoints[i], ppoints[nexti] );
}

const ObjectImpType* PolygonSideType::resultId() const
{
  return SegmentImp::stype();
}

/* convex hull of a polygon  */

static const ArgsParser::spec argsspecConvexHull[] =
{
  { PolygonImp::stype(), I18N_NOOP( "Construct the convex hull of this polygon" ),
    I18N_NOOP( "Select the polygon of which you want to construct the convex hull..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ConvexHullType )

ConvexHullType::ConvexHullType()
  : ArgsParserObjectType( "ConvexHull", argsspecConvexHull, 1 )
{
}

ConvexHullType::~ConvexHullType()
{
}

const ConvexHullType* ConvexHullType::instance()
{
  static const ConvexHullType t;
  return &t;
}

ObjectImp* ConvexHullType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  const std::vector<Coordinate> ppoints = static_cast<const PolygonImp*>( parents[0] )->points();

  if ( ppoints.size() < 3 ) return new InvalidImp;

  std::vector<Coordinate> hull = computeConvexHull( ppoints );
  if ( hull.size() < 3 ) return new InvalidImp;
  return new PolygonImp( hull );
}

const ObjectImpType* ConvexHullType::resultId() const
{
  return PolygonImp::stype();
}
