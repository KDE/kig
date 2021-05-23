// SPDX-FileCopyrightText: 2003 Maurizio Paolini <paolini@dmf.unicatt.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "polygon_type.h"

#include <math.h>

#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"
#include "polygon_imp.h"
#include "object_calcer.h"

#include "../misc/common.h"

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
  // return new PolygonImp( 3, points, centerofmass3/3 );
  return new FilledPolygonImp( points );
}

const ObjectImpType* TriangleB3PType::resultId() const
{
  return FilledPolygonImp::stype();
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
  //return new PolygonImp( npoints, points, centerofmassn/npoints );
  return new FilledPolygonImp( points );
}

const ObjectImpType* PolygonBNPType::resultId() const
{
  return FilledPolygonImp::stype();
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
 * Added by Petr Gajdos <pgajdos@suse.cz>
 * opened polygon
 */

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( OpenPolygonType )

OpenPolygonType::OpenPolygonType()
  : ObjectType( "OpenPolygon" )
{
}

OpenPolygonType::~OpenPolygonType()
{
}

const OpenPolygonType* OpenPolygonType::instance()
{
  static const OpenPolygonType s;
  return &s;
}

ObjectImp* OpenPolygonType::calc( const Args& parents, const KigDocument& ) const
{
  uint count = parents.size();
  assert (count >= 3);
  std::vector<Coordinate> points;

  uint npoints = 0;
  for ( uint i = 0; i < count; ++i )
  {
    npoints++;
    if ( ! parents[i]->inherits( PointImp::stype() ) ) return new InvalidImp;
    Coordinate point = static_cast<const PointImp*>( parents[i] )->coordinate();
    points.push_back( point );
  }
  return new OpenPolygonalImp( points );  // minside = false, mopen = true
}

const ObjectImpType* OpenPolygonType::resultId() const
{
  return OpenPolygonalImp::stype();
}

const ObjectImpType* OpenPolygonType::impRequirement( const ObjectImp*, const Args& ) const
{
  return PointImp::stype();
}

bool OpenPolygonType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  return true;
}

std::vector<ObjectCalcer*> OpenPolygonType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  return args;  /* should already be in correct order */
}

Args OpenPolygonType::sortArgs( const Args& args ) const
{
  return args;
}

bool OpenPolygonType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
}

bool OpenPolygonType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  for ( uint i = 0; i < parents.size(); ++i )
  {
    if ( !parents[i]->isFreelyTranslatable() ) return false;
  }
  return true;
}

void OpenPolygonType::move( ObjectTypeCalcer& o, const Coordinate& to,
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

const Coordinate OpenPolygonType::moveReferencePoint( const ObjectTypeCalcer& o
) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> OpenPolygonType::movableParents( const ObjectTypeCalcer& ourobj ) const
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

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PolygonBCVType )

PolygonBCVType::PolygonBCVType()
  : ObjectType( "PoligonBCV" )
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
  //return new PolygonImp( uint (sides), vertexes, center );
  return new FilledPolygonImp( vertexes );
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
  { FilledPolygonImp::stype(), I18N_NOOP( "Intersect this polygon with a line" ),
    I18N_NOOP( "Select the polygon of which you want the intersection with a line..." ), false },
  { AbstractLineImp::stype(), 
    I18N_NOOP( "Intersect this line with a polygon" ), 
    I18N_NOOP( "Select the line of which you want the intersection with a polygon..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PolygonLineIntersectionType )

PolygonLineIntersectionType::PolygonLineIntersectionType()
  : ArgsParserObjectType( "PolygonLineIntersection", argsspecPolygonLineIntersection, 2 )
{
}

PolygonLineIntersectionType::PolygonLineIntersectionType( const char fulltypename[],
                        const struct ArgsParser::spec argsspec[],
                        int n )
  : ArgsParserObjectType( fulltypename, argsspec, n )
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

const ObjectImpType* PolygonLineIntersectionType::resultId() const
{
  return SegmentImp::stype();
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

  const AbstractPolygonImp* polygon = static_cast<const AbstractPolygonImp*>( parents[0] );
  const std::vector<Coordinate> ppoints = polygon->points();
  const LineData line = static_cast<const AbstractLineImp*>( parents[1] )->data();
  double t1, t2;
  int side = 0;
  uint numintersections;
  std::vector<Coordinate>::const_iterator intersectionside;

  // since we use the same constructor as for ConicLine, we get the values -1 and +1
  if ( parents.size() >= 3 ) side = static_cast<const IntImp*>( parents[2] )->data();
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

  bool openpolygon = false;
  bool inside = false;
  if ( parents[0]->inherits( OpenPolygonalImp::stype() ) ) openpolygon = true;
  if ( parents[0]->inherits( FilledPolygonImp::stype() ) ) inside = true;

  numintersections = polygonlineintersection( ppoints, line.a, line.b,
                         boundleft, boundright, inside, openpolygon, t1, t2, intersectionside );

  bool filledpolygon = false;
  if ( parents[0]->inherits( FilledPolygonImp::stype() ) ) filledpolygon = true;

  if ( ! filledpolygon )
  {
    if ( side ==  -1 && numintersections >= 1) return new PointImp( line.a + t1*(line.b - line.a) );
    if ( side == 1 && numintersections >= 2) return new PointImp( line.a + t2*(line.b - line.a) );
    return new InvalidImp;
  } 

  if (numintersections > 2) return new InvalidImp;

  switch (numintersections)
  {
    case 1:   /* just for completeness: this should never happen */
      return new PointImp( line.a + t1*(line.b - line.a) );
      break;
    case 2:
      return new SegmentImp( line.a + t1*(line.b - line.a),
                             line.a + t2*(line.b - line.a) );
      break;
  }
  return new InvalidImp;
}

/*
 * polygonal curve--line intersection, this has an extra parameter
 * indicating which intersection point we want
 */

static const ArgsParser::spec argsspecOPolygonalLineIntersection[] =
{
  { OpenPolygonalImp::stype(), I18N_NOOP( "Intersect this polygonal curve with a line" ),
    I18N_NOOP( "Select the polygonal curve of which you want the intersection with a line..." ), false },
  { AbstractLineImp::stype(),
    I18N_NOOP( "Intersect this line with a polygonal curve" ),
    I18N_NOOP( "Select the line of which you want the intersection with a polygonal curve..." ), false },
  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( OPolygonalLineIntersectionType )

OPolygonalLineIntersectionType::OPolygonalLineIntersectionType()
  : PolygonLineIntersectionType( "OPolygonalLineIntersection", argsspecOPolygonalLineIntersection, 3 )
{
}

OPolygonalLineIntersectionType::~OPolygonalLineIntersectionType()
{
}

const OPolygonalLineIntersectionType* OPolygonalLineIntersectionType::instance()
{
  static const OPolygonalLineIntersectionType t;
  return &t;
}

const ObjectImpType* OPolygonalLineIntersectionType::resultId() const
{
  return PointImp::stype();
}

static const ArgsParser::spec argsspecCPolygonalLineIntersection[] =
{
  { ClosedPolygonalImp::stype(), I18N_NOOP( "Intersect this polygonal curve with a line" ),
    I18N_NOOP( "Select the polygonal curve of which you want the intersection with a line..." ), false },
  { AbstractLineImp::stype(),
    I18N_NOOP( "Intersect this line with a polygonal curve" ),
    I18N_NOOP( "Select the line of which you want the intersection with a polygonal curve..." ), false },
  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CPolygonalLineIntersectionType )

CPolygonalLineIntersectionType::CPolygonalLineIntersectionType()
  : PolygonLineIntersectionType( "CPolygonalLineIntersection", argsspecCPolygonalLineIntersection, 3 )
{
}

CPolygonalLineIntersectionType::~CPolygonalLineIntersectionType()
{
}

const CPolygonalLineIntersectionType* CPolygonalLineIntersectionType::instance()
{
  static const CPolygonalLineIntersectionType t;
  return &t;
}

const ObjectImpType* CPolygonalLineIntersectionType::resultId() const
{
  return PointImp::stype();
}

/*
 * this function computes the intersection of a polygon (given its
 * list of vertices) with a line/ray/segment described by two points;
 * the booleans boundleft, boundright discriminates between the
 * line/ray/segment case.
 * the integer returned is the number of "endpoints" in the computed
 * intersection, 0 means no intersection, 1 should never be returned
 * 2 means that the intersection is just a single segment,
 * 3 or more means that the intersection consists of more than one segment,
 * in which case only the intersection with smallest values of the parameter
 * ent is returned.
 * t1 and t2 are the two values of the parameter that produce
 * the two endpoints (in case the return value is >= 2) of the
 * computed intersection segment: a + t*(b-a).
 * The "intersectionside" parameter (needed for the computation of
 * the polygon-polygon intersection) is a pointer to the vertex of the
 * polygon preceding the second intersection (t2).
 *
 * this function is rather involved, mainly because of the special 
 * case when one (or both) the segment endpoints is also one of the
 * polygon vertices
 */

int
polygonlineintersection( const std::vector<Coordinate>& ppoints, 
                         const Coordinate &a, const Coordinate &b,
                         bool boundleft, bool boundright, bool inside,
			 bool openpolygon, double& t1, double& t2,
                         std::vector<Coordinate>::const_iterator& intersectionside )
{
  double intersections[2] = {0.0, 0.0};
//  uint whichintersection = 0;
  std::vector<Coordinate>::const_iterator i;
  std::vector<Coordinate>::const_iterator intersectionsides[2];
  uint numintersections = 0;

  double abx = b.x - a.x;
  double aby = b.y - a.y;

  bool leftendinside = false;
  bool rightendinside = false;
  Coordinate prevpoint = ppoints.back() - a;
  if ( openpolygon ) prevpoint = ppoints.front() - a;
  bool prevpointbelow = ( abx*prevpoint.y <= aby*prevpoint.x );
  for ( i = ppoints.begin(); i != ppoints.end(); ++i )
  {
    if ( openpolygon && i == ppoints.begin() ) continue;
    Coordinate point = *i - a;
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
        numintersections++;
        if ( t < intersections[1] || numintersections <= 2 )
        {
          intersections[1] = t;
          intersectionsides[1] = i;
        }
        if ( t < intersections[0] || numintersections <= 1 )
        {
          intersections[1] = intersections[0];
          intersections[0] = t;
          intersectionsides[1] = intersectionsides[0];
          intersectionsides[0] = i;
        }
      }
    }
    prevpoint = point;
    prevpointbelow = pointbelow;
  }

  if ( inside )
  {
    if ( leftendinside )
    {
      numintersections++;
      intersections[1] = intersections[0];
      intersectionsides[1] = intersectionsides[0];
      intersections[0] = 0.0;
      intersectionsides[0] = ppoints.end();
    }
    if ( rightendinside )
    {
      numintersections++;
      intersections[1] = 1.0;
      intersectionsides[1] = ppoints.end();
      if ( numintersections < 2 )
      {
        intersections[0] = 1.0;
        intersectionsides[1] = ppoints.end();
      }
    }
  }

  if (numintersections>= 1)
  {
    t1 = intersections[0];
    intersectionside = intersectionsides[0];
  }
  if (numintersections >= 2)
  {
    t2 = intersections[1];
    intersectionside = intersectionsides[1];
  }
  if ( intersectionside == ppoints.begin() ) intersectionside = ppoints.end();
  intersectionside--;
  return numintersections;
}

int
polygonlineintersection( const std::vector<Coordinate>& ppoints, 
                         const Coordinate &a, const Coordinate &b,
                         double& t1, double& t2,
                         std::vector<Coordinate>::const_iterator& intersectionside )
{
  return polygonlineintersection ( ppoints, a, b, true, true, true, false, t1, t2, intersectionside );
}

/* polygon-polygon intersection */

static const ArgsParser::spec argsspecPolygonPolygonIntersection[] =
{
  { FilledPolygonImp::stype(), I18N_NOOP( "Intersect this polygon with another polygon" ),
    I18N_NOOP( "Select the polygon of which you want the intersection with another polygon..." ), false },
  { FilledPolygonImp::stype(), I18N_NOOP( "Intersect with this polygon" ),
    I18N_NOOP( "Select the second polygon for the intersection..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PolygonPolygonIntersectionType )

PolygonPolygonIntersectionType::PolygonPolygonIntersectionType()
  : ArgsParserObjectType( "PolygonPolygonIntersection", argsspecPolygonPolygonIntersection, 2 )
{
}

PolygonPolygonIntersectionType::~PolygonPolygonIntersectionType()
{
}

const PolygonPolygonIntersectionType* PolygonPolygonIntersectionType::instance()
{
  static const PolygonPolygonIntersectionType t;
  return &t;
}

ObjectImp* PolygonPolygonIntersectionType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  const FilledPolygonImp* polygon1 = static_cast<const FilledPolygonImp*>( parents[0] );
  const std::vector<Coordinate> ppoints1 = polygon1->points();
  const FilledPolygonImp* polygon2 = static_cast<const FilledPolygonImp*>( parents[1] );
  const std::vector<Coordinate> ppoints2 = polygon2->points();
  std::vector<Coordinate> ppointsint;
  double t1, t2;
  uint numintersections;
  std::vector<Coordinate>::const_iterator ipoint, iprevpoint, iprevprevpoint,
                                          intersectionside, istartpoint;
  Coordinate point;
  const std::vector<Coordinate> *ppointsa, *ppointsb, *ppointsc, *ppointsstart;
  ppointsa = &ppoints1;
  ppointsb = &ppoints2;
  int direction = 1;

  if ( polygon1->isTwisted() || polygon2->isTwisted() ) return new InvalidImp;

  bool intersect = false;

  for (int k = 0; k <2; k++)
  {
    iprevpoint = ppointsa->end();
    --iprevpoint;
    for (ipoint = ppointsa->begin(); ipoint != ppointsa->end(); ++ipoint)
    {
      numintersections = polygonlineintersection( *ppointsb, 
                           *iprevpoint, *ipoint, t1, t2, intersectionside);
      if (numintersections >= 2)
      {
        intersect = true;
        break;
      }
      iprevpoint = ipoint;
    }
    if (intersect) break;
    ppointsa = &ppoints2;
    ppointsb = &ppoints1;
  }

  if ( ! intersect ) return new InvalidImp;
  ppointsstart = ppointsa;
  istartpoint = ipoint;
  point = *iprevpoint + t1*(*ipoint-*iprevpoint);
  ppointsint.push_back( point );
  point = *iprevpoint + t2*(*ipoint-*iprevpoint);
  ppointsint.push_back( point );

  do
  {
    if ( t2 == 1.0 )
    {
      /*
       * in this case I will continue cicling along the current polygon
       */
      iprevprevpoint = iprevpoint;  /* we need this in the special case */
      iprevpoint = ipoint;
      if ( direction < 0 && ipoint == ppointsa->begin() ) ipoint = ppointsa->end();
      ipoint += direction;
      if ( ipoint == ppointsa->end() ) ipoint = ppointsa->begin();
      numintersections = polygonlineintersection( *ppointsb, 
                           *iprevpoint, *ipoint, t1, t2, intersectionside);
      if ( numintersections < 2 )
      {
        /*
         * this is a special case. We are here e.g. if the two polygons have a 
         * common vertex
         * We treat this case in a tricky way :-(
         * Still, if the two polygons have some common vertices the
         * result is sometimes unpredictable :-(
         */
        point = 1e-10*(*iprevprevpoint) + (1 - 1e-10)*(*iprevpoint);
        numintersections = polygonlineintersection( *ppointsb, 
                             point, *ipoint, t1, t2, intersectionside);
      } else
      {
        if ( t1 != 0.0 ) return new InvalidImp;             /* can this happen? */
        point = *iprevpoint + t2*(*ipoint-*iprevpoint);
        ppointsint.push_back( point );
      }
    } else
    {
      /*
       * in this case I must cicle along the other polygon
       */
      ppointsc = ppointsa;
      ppointsa = ppointsb;
      ppointsb = ppointsc;
      ipoint = iprevpoint = intersectionside;
      ipoint++;
      if ( ipoint == ppointsa->end() ) ipoint = ppointsa->begin();
      point = ppointsint.back();
      direction = 1;
      numintersections = polygonlineintersection( *ppointsb, 
                           point, *ipoint, t1, t2, intersectionside);
      if ( numintersections < 2 || t2 < 1e-12)
      {
        direction = -1;
        ipoint = iprevpoint;
        numintersections = polygonlineintersection( *ppointsb, 
                             point, *ipoint, t1, t2, intersectionside);
        if (numintersections < 2) return new InvalidImp;
      }
      point = point + t2*(*ipoint-point);
      ppointsint.push_back( point );
    }
  } while ( ( ppointsstart != ppointsa || istartpoint != ipoint ) && ppointsint.size() < 1000 );

  if (ppointsint.size() < 2) return new InvalidImp;   /* should never happen */
  ppointsint.pop_back();
  ppointsint.pop_back();
  return new FilledPolygonImp (ppointsint);
}

const ObjectImpType* PolygonPolygonIntersectionType::resultId() const
{
  return FilledPolygonImp::stype();
}

/* polygon vertices  */

static const ArgsParser::spec argsspecPolygonVertex[] =
{
  { FilledPolygonImp::stype(), I18N_NOOP( "Construct the vertices of this polygon" ),
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

  const std::vector<Coordinate> ppoints = static_cast<const FilledPolygonImp*>( parents[0] )->points();
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
  { FilledPolygonImp::stype(), I18N_NOOP( "Construct the sides of this polygon" ),
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

  const std::vector<Coordinate> ppoints = static_cast<const FilledPolygonImp*>( parents[0] )->points();
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
  { AbstractPolygonImp::stype(), I18N_NOOP( "Construct the convex hull of this polygon" ),
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

  const std::vector<Coordinate> ppoints = static_cast<const AbstractPolygonImp*>( parents[0] )->points();

  if ( ppoints.size() < 3 ) return new InvalidImp;

  std::vector<Coordinate> hull = computeConvexHull( ppoints );
  if ( hull.size() < 3 ) return new InvalidImp;
  return new FilledPolygonImp( hull );
}

const ObjectImpType* ConvexHullType::resultId() const
{
  return FilledPolygonImp::stype();
}
