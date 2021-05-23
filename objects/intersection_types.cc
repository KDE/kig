// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "intersection_types.h"

#include <math.h>

#include "bogus_imp.h"
#include "circle_imp.h"
#include "conic_imp.h"
#include "cubic_imp.h"
#include "line_imp.h"
#include "other_imp.h"
#include "point_imp.h"

static const char intersectlinestat[] = I18N_NOOP( "Intersect with this line" );

static const ArgsParser::spec argsspecConicLineIntersection[] =
{
  { ConicImp::stype(), I18N_NOOP( "Intersect with this conic" ),
    "SHOULD NOT BE SEEN", true },
  { AbstractLineImp::stype(), intersectlinestat, "SHOULD NOT BE SEEN", true },
  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ConicLineIntersectionType )

ConicLineIntersectionType::ConicLineIntersectionType()
  : ArgsParserObjectType( "ConicLineIntersection",
                         argsspecConicLineIntersection, 3 )
{
}

ConicLineIntersectionType::~ConicLineIntersectionType()
{
}

const ConicLineIntersectionType* ConicLineIntersectionType::instance()
{
  static const ConicLineIntersectionType t;
  return &t;
}

ObjectImp* ConicLineIntersectionType::calc( const Args& parents, const KigDocument& doc ) const
{
  /*
   * special case of a circle that degenerates into a line.  This is possible e.g. for
   * circles by three points when the points get aligned.
   */
  if ( parents.size() == 3 && parents[0]->inherits( AbstractLineImp::stype() ) &&
                              parents[1]->inherits( AbstractLineImp::stype() ) &&
                              parents[2]->inherits( IntImp::stype() ) )
  {
    int side = static_cast<const IntImp*>( parents[2] )->data();
    assert( side == 1 || side == -1 );
    const LineData degline = static_cast<const AbstractLineImp*>( parents[0] )->data();
    const LineData line = static_cast<const AbstractLineImp*>( parents[1] )->data();
    const double vecprod = degline.dir().y * line.dir().x - degline.dir().x * line.dir().y;
    /*
     * mp: In this case only one of the two points must be valid (the other is "pushed"
     * to infinity).  The choice of which one is done such that we avoid abrupt points exchange
     * when dinamically movint points
     */
    if (side*vecprod < 0)
    {
      Coordinate p = calcIntersectionPoint( degline, line );
      return new PointImp( p );
    }
    return new InvalidImp();
  }

  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  int side = static_cast<const IntImp*>( parents[2] )->data();
  assert( side == 1 || side == -1 );
  const AbstractLineImp* lineimp = static_cast<const AbstractLineImp*>( parents[1] );
  const LineData line = lineimp->data();

  Coordinate ret;
  if ( parents[0]->inherits( CircleImp::stype() ) )
  {
    // easy case..
    const CircleImp* c = static_cast<const CircleImp*>( parents[0] );
    ret = calcCircleLineIntersect(
      c->center(), c->squareRadius(), line, c->orientation()*side );
  }
  else
  {
    // harder case..
    ret = calcConicLineIntersect(
      static_cast<const ConicImp*>( parents[0] )->cartesianData(),
      line, 0.0, side );
  }
  if ( !ret.valid() ) return new InvalidImp;
  if ( !lineimp->containsPoint( ret, doc ) ) return new InvalidImp;
  return new PointImp( ret );
}

/*
 * This construction is authomatically invoked when the user 
 * intersects a line with a conic with one of the intersections
 * already present.  There are two positive side effects:
 * 1. The production of coincident points is greatly reduced
 * 2. The "other" intersection will remain the "other" one also
 * when dynamically moving the construction, which is what the
 * user expects
 */

static const ArgsParser::spec argsspecConicLineOtherIntersection[] =
{
  { ConicImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", true },
  { AbstractLineImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", true },
  { PointImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ConicLineOtherIntersectionType )

ConicLineOtherIntersectionType::ConicLineOtherIntersectionType()
  : ArgsParserObjectType( "ConicLineOtherIntersection",
                         argsspecConicLineOtherIntersection, 3 )
{
}

ConicLineOtherIntersectionType::~ConicLineOtherIntersectionType()
{
}

const ConicLineOtherIntersectionType* ConicLineOtherIntersectionType::instance()
{
  static const ConicLineOtherIntersectionType t;
  return &t;
}

ObjectImp* ConicLineOtherIntersectionType::calc( const Args& parents, const KigDocument& doc ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  Coordinate p = static_cast<const PointImp*>( parents[2] )->coordinate();
  const AbstractLineImp* line = static_cast<const AbstractLineImp*>( parents[1] );
  const ConicImp* conic = static_cast<const ConicImp*>( parents[0] );
  const LineData linedata = line->data();

  if ( !line->containsPoint( p, doc ) || !conic->containsPoint( p, doc ) )
  {
    return new InvalidImp;
  }

  Coordinate ret;
  double pax = p.x - linedata.a.x;
  double pay = p.y - linedata.a.y;
  double bax = linedata.b.x - linedata.a.x;
  double bay = linedata.b.y - linedata.a.y;
  double knownparam = (pax*bax + pay*bay)/(bax*bax + bay*bay);
  ret = calcConicLineIntersect(
    conic->cartesianData(),
    linedata, knownparam, 0 );
  if ( !ret.valid() ) return new InvalidImp;
  if ( !line->containsPoint( ret, doc ) ) return new InvalidImp;
  return new PointImp( ret );
}
// cubic line other intersection

static const ArgsParser::spec argsspecCubicLineOtherIntersection[] =
{
  { CubicImp::stype(), I18N_NOOP( "Intersect with this cubic"),  "SHOULD NOT BE SEEN", true },
  { AbstractLineImp::stype(),  intersectlinestat , "SHOULD NOT BE SEEN", true },
  { PointImp::stype(), I18N_NOOP( "Already computed intersection point"), "SHOULD NOT BE SEEN", true },
  { PointImp::stype(), I18N_NOOP( "Already computed intersection point"), "SHOULD NOT BE SEEN", true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CubicLineOtherIntersectionType )

CubicLineOtherIntersectionType::CubicLineOtherIntersectionType()
  : ArgsParserObjectType( "CubicLineOtherIntersection",
                         argsspecCubicLineOtherIntersection, 4 )
{
}

CubicLineOtherIntersectionType::~CubicLineOtherIntersectionType()
{
}

const CubicLineOtherIntersectionType* CubicLineOtherIntersectionType::instance()
{
  static const CubicLineOtherIntersectionType t;
  return &t;
}

ObjectImp* CubicLineOtherIntersectionType::calc( const Args& parents, const KigDocument& doc ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  Coordinate p = static_cast<const PointImp*>( parents[2] )->coordinate();
  Coordinate q = static_cast<const PointImp*>( parents[3] )->coordinate();
  const AbstractLineImp* line = static_cast<const AbstractLineImp*>( parents[1] );
  const CubicImp* cubic = static_cast<const CubicImp*>( parents[0] );
  const LineData linedata = line->data();
  const CubicCartesianData cubicData = cubic->data();

  if ( !line->containsPoint( p, doc ) || !cubic->containsPoint( p, doc ) )
  {
    return new InvalidImp;
  }
  if ( !line->containsPoint( q, doc ) || !cubic->containsPoint( q, doc ) )
  {
    return new InvalidImp;
  }
  Coordinate ret;
  double pax = p.x - linedata.a.x;
  double pay = p.y - linedata.a.y;
  double bax = linedata.b.x - linedata.a.x;
  double bay = linedata.b.y - linedata.a.y;
  double qax = q.x - linedata.a.x;
  double qay = q.y - linedata.a.y;
  double knownparam1 = (pax*bax + pay*bay)/(bax*bax + bay*bay);
  double knownparam2 = (qax*bax + qay*bay)/(bax*bax + bay*bay);
  double t, aa, bb, cc, dd;
  calcCubicLineRestriction ( cubicData, linedata.a, linedata.b-linedata.a, aa, bb, cc, dd );

  t = - bb/aa - knownparam1 - knownparam2;

  ret = linedata.a + t*(linedata.b - linedata.a);

  if ( !ret.valid() ) return new InvalidImp;
 // if ( !line->containsPoint( ret, doc ) ) return new InvalidImp;
  return new PointImp( ret );
}

// cubic line two intersection

static const ArgsParser::spec argsspecCubicLineTwoIntersection[] =
{
  { CubicImp::stype(), I18N_NOOP( "Intersect with this cubic"),  "SHOULD NOT BE SEEN", true },
  { AbstractLineImp::stype(),  intersectlinestat , "SHOULD NOT BE SEEN", true },
  { PointImp::stype(), I18N_NOOP( "Already computed intersection point"), "Already computed intersection point", true },
  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CubicLineTwoIntersectionType )

CubicLineTwoIntersectionType::CubicLineTwoIntersectionType()
  : ArgsParserObjectType( "CubicLineTwoIntersection",
                         argsspecCubicLineTwoIntersection, 4 )
{
}

CubicLineTwoIntersectionType::~CubicLineTwoIntersectionType()
{
}

const CubicLineTwoIntersectionType* CubicLineTwoIntersectionType::instance()
{
  static const CubicLineTwoIntersectionType t;
  return &t;
}

ObjectImp* CubicLineTwoIntersectionType::calc( const Args& parents, const KigDocument& doc ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  Coordinate p = static_cast<const PointImp*>( parents[2] )->coordinate();
  const AbstractLineImp* line = static_cast<const AbstractLineImp*>( parents[1] );
  const CubicImp* cubic = static_cast<const CubicImp*>( parents[0] );
  int side = static_cast<const IntImp*>( parents[3] )->data();
  assert( side == 1 || side == -1 );
  const LineData linedata = line->data();
  const CubicCartesianData cubicData = cubic->data();

  if ( !line->containsPoint( p, doc ) || !cubic->containsPoint( p, doc ) )
  {
    return new InvalidImp;
  }

  Coordinate ret;
  double pax = p.x - linedata.a.x;
  double pay = p.y - linedata.a.y;
  double bax = linedata.b.x - linedata.a.x;
  double bay = linedata.b.y - linedata.a.y;
  double knownparam = (pax*bax + pay*bay)/(bax*bax + bay*bay);
  double t, aa, bb, cc, dd, bbb, ccc;
  calcCubicLineRestriction ( cubicData, linedata.a, linedata.b-linedata.a, aa, bb, cc, dd );
  bbb = bb/aa + knownparam;
  ccc =  (cc/aa) + (bb/aa*knownparam) + (knownparam*knownparam);

  double discrim = bbb*bbb - 4*ccc;
  if (discrim < 0.0)
  {
    return new InvalidImp;
  }
  else
  {
    if ( side*bbb > 0 )
    {
      t = bbb + side*sqrt(discrim);
      t = - 2*ccc/t;
    } else {
      t = -bbb + side*sqrt(discrim);
      t /= 2;
    }

    ret= linedata.a + t*(linedata.b - linedata.a);
  }
  if ( !ret.valid() ) return new InvalidImp;
 // if ( !line->containsPoint( ret, doc ) ) return new InvalidImp;
  return new PointImp( ret );
}

/*
 * This construction is authomatically invoked when the user 
 * intersects two circles with one of the intersections
 * already present, see above...
 */

static const ArgsParser::spec argsspecCircleCircleOtherIntersection[] =
{
  { CircleImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", true },
  { CircleImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", true },
  { PointImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CircleCircleOtherIntersectionType )

CircleCircleOtherIntersectionType::CircleCircleOtherIntersectionType()
  : ArgsParserObjectType( "CircleCircleOtherIntersection",
                         argsspecCircleCircleOtherIntersection, 3 )
{
}

CircleCircleOtherIntersectionType::~CircleCircleOtherIntersectionType()
{
}

const CircleCircleOtherIntersectionType* CircleCircleOtherIntersectionType::instance()
{
  static const CircleCircleOtherIntersectionType t;
  return &t;
}

ObjectImp* CircleCircleOtherIntersectionType::calc( const Args& parents, const KigDocument& doc ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  Coordinate p = static_cast<const PointImp*>( parents[2] )->coordinate();
  const CircleImp* circle1 = static_cast<const CircleImp*>( parents[0] );
  const CircleImp* circle2 = static_cast<const CircleImp*>( parents[1] );

  if ( !circle1->containsPoint( p, doc ) || !circle2->containsPoint( p, doc ) )
  {
    return new InvalidImp;
  }

  Coordinate c1 = circle1->center();
  Coordinate c1c2 = circle2->center() - c1;
  Coordinate c1p = p - c1;
  Coordinate w = Coordinate( -c1c2.y, c1c2.x );   /* w is normal to the line through the centers */
  double wnormsq = w.x*w.x + w.y*w.y;
  if ( wnormsq < 1e-12 ) return new InvalidImp;
  double pc1c2dist = ( c1p.x*w.x + c1p.y*w.y )/wnormsq;

  Coordinate ret = p - 2*pc1c2dist*w;
  return new PointImp( ret );
}

/* LineLineIntersection */

static const ArgsParser::spec argsspecLineLineIntersection[] =
{
  { AbstractLineImp::stype(), intersectlinestat, "SHOULD NOT BE SEEN", true },
  { AbstractLineImp::stype(), intersectlinestat, "SHOULD NOT BE SEEN", true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( LineLineIntersectionType )

LineLineIntersectionType::LineLineIntersectionType()
  : ArgsParserObjectType( "LineLineIntersection",
                         argsspecLineLineIntersection, 2 )
{
}

LineLineIntersectionType::~LineLineIntersectionType()
{
}

const LineLineIntersectionType* LineLineIntersectionType::instance()
{
  static const LineLineIntersectionType t;
  return &t;
}

ObjectImp* LineLineIntersectionType::calc( const Args& parents, const KigDocument& d ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  Coordinate p =
    calcIntersectionPoint(
      static_cast<const AbstractLineImp*>( parents[0] )->data(),
      static_cast<const AbstractLineImp*>( parents[1] )->data() );
  if ( static_cast<const AbstractLineImp*>( parents[0] )->containsPoint( p, d ) &&
       static_cast<const AbstractLineImp*>( parents[1] )->containsPoint( p, d ) )
    return new PointImp( p );
  else return new InvalidImp();
}

static const ArgsParser::spec argsspecCubicLineIntersection[] =
{
  { CubicImp::stype(), I18N_NOOP( "Intersect with this cubic curve" ),
    "SHOULD NOT BE SEEN", true },
  { AbstractLineImp::stype(), intersectlinestat, "SHOULD NOT BE SEEN", true },
  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CubicLineIntersectionType )

CubicLineIntersectionType::CubicLineIntersectionType()
  : ArgsParserObjectType( "CubicLineIntersection",
                         argsspecCubicLineIntersection, 3 )
{
}

CubicLineIntersectionType::~CubicLineIntersectionType()
{
}

const CubicLineIntersectionType* CubicLineIntersectionType::instance()
{
  static const CubicLineIntersectionType t;
  return &t;
}

ObjectImp* CubicLineIntersectionType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  int which = static_cast<const IntImp*>( parents[2] )->data();
  bool valid = true;
  const Coordinate c = calcCubicLineIntersect(
    static_cast<const CubicImp*>( parents[0] )->data(),
    static_cast<const AbstractLineImp*>( parents[1] )->data(),
    which, valid );
  if ( valid ) return new PointImp( c );
  else return new InvalidImp;
}

const ObjectImpType* ConicLineIntersectionType::resultId() const
{
  return PointImp::stype();
}

const ObjectImpType* ConicLineOtherIntersectionType::resultId() const
{
  return PointImp::stype();
}

const ObjectImpType* CircleCircleOtherIntersectionType::resultId() const
{
  return PointImp::stype();
}

const ObjectImpType* LineLineIntersectionType::resultId() const
{
  return PointImp::stype();
}

const ObjectImpType* CubicLineIntersectionType::resultId() const
{
  return PointImp::stype();
}

const ObjectImpType* CubicLineOtherIntersectionType::resultId() const
{
  return PointImp::stype();
}

const ObjectImpType* CubicLineTwoIntersectionType::resultId() const
{
  return PointImp::stype();
}

static const ArgsParser::spec argsspecCircleCircleIntersection[] =
{
  { CircleImp::stype(), I18N_NOOP( "Intersect with this circle" ),
    "SHOULD NOT BE SEEN", true },
  { CircleImp::stype(), I18N_NOOP( "Intersect with this circle" ),
    "SHOULD NOT BE SEEN", true },
  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CircleCircleIntersectionType )

CircleCircleIntersectionType::CircleCircleIntersectionType()
  : ArgsParserObjectType( "CircleCircleIntersection",
                         argsspecCircleCircleIntersection, 3 )
{
}

CircleCircleIntersectionType::~CircleCircleIntersectionType()
{
}

const CircleCircleIntersectionType* CircleCircleIntersectionType::instance()
{
  static const CircleCircleIntersectionType t;
  return &t;
}

ObjectImp* CircleCircleIntersectionType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() == 3 &&
       ( parents[0]->inherits( LineImp::stype() ) || parents[1]->inherits( LineImp::stype() ) ) &&
       parents[2]->inherits( IntImp::stype() ) )
  {
    /* This the special case when one or both circles degenerate into a line
     */
    int il = 0;
    int ori = 1;
    if ( parents[1]->inherits( LineImp::stype() ) ) { il = 1; ori = -1; }
    const LineData line = static_cast<const AbstractLineImp*>( parents[il] )->data();
    int side = static_cast<const IntImp*>( parents[2] )->data();
    assert( side == 1 || side == -1 );
    if ( parents[1 - il]->inherits( CircleImp::stype() ) )
    {
      const CircleImp* c = static_cast<const CircleImp*>( parents[1 - il] );
      const Coordinate o = c->center();
      const double rsq = c->squareRadius();
      ori *= c->orientation();
      Coordinate ret = calcCircleLineIntersect( o, rsq, line, ori*side );
      if ( ret.valid() ) return new PointImp( ret );
      else return new InvalidImp;
    } else {
      // same code as for ConicLineIntersection with degenerate conic
      assert (il == 1);
      const LineData degline = static_cast<const AbstractLineImp*>( parents[0] )->data();
      const double vecprod = degline.dir().y * line.dir().x - degline.dir().x * line.dir().y;
      if (side*vecprod > 0)
      {
        Coordinate p = calcIntersectionPoint( degline, line );
        return new PointImp( p );
      }
      return new InvalidImp();
    }
  }

  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  int side = static_cast<const IntImp*>( parents[2] )->data();
  assert( side == 1 || side == -1 );
  const CircleImp* c1 = static_cast<const CircleImp*>( parents[0] );
  const CircleImp* c2 = static_cast<const CircleImp*>( parents[1] );
  const Coordinate o1 = c1->center();
  const Coordinate o2 = c2->center();
  const int ori = ( c1->orientation()*c2->orientation() < 0 )?(-1):(1);
  const double r1sq = c1->squareRadius();
  const Coordinate a = calcCircleRadicalStartPoint(
    o1, o2, r1sq, c2->squareRadius()
    );
  const LineData line = LineData (a, Coordinate ( a.x -o2.y + o1.y, a.y + o2.x - o1.x ));
  Coordinate ret = calcCircleLineIntersect( o1, r1sq, line, ori*side );
  if ( ret.valid() ) return new PointImp( ret );
  else return new InvalidImp;
}

const ObjectImpType* CircleCircleIntersectionType::resultId() const
{
  return PointImp::stype();
}

static const ArgsParser::spec argsspecArcLineIntersection[] =
{
  { ArcImp::stype(), I18N_NOOP( "Intersect with this arc" ),
    "SHOULD NOT BE SEEN", true },
  { AbstractLineImp::stype(), intersectlinestat, "SHOULD NOT BE SEEN", true },
  { IntImp::stype(), "param", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ArcLineIntersectionType )

ArcLineIntersectionType::ArcLineIntersectionType()
  : ArgsParserObjectType( "ArcLineIntersection",
                         argsspecArcLineIntersection, 3 )
{
}

ArcLineIntersectionType::~ArcLineIntersectionType()
{
}

const ArcLineIntersectionType* ArcLineIntersectionType::instance()
{
  static const ArcLineIntersectionType t;
  return &t;
}

ObjectImp* ArcLineIntersectionType::calc( const Args& parents, const KigDocument& ) const
{
  /*
   * special case of an arc that degenerates into a line.  This is possible e.g. for
   * arcs by three points when the points get aligned.
   */
  if ( parents.size() == 3 && parents[0]->inherits( AbstractLineImp::stype() ) &&
                              parents[1]->inherits( AbstractLineImp::stype() ) &&
                              parents[2]->inherits( IntImp::stype() ) )
  {
    int side = static_cast<const IntImp*>( parents[2] )->data();
    assert( side == 1 || side == -1 );
    const LineData degline = static_cast<const AbstractLineImp*>( parents[0] )->data();
    const LineData line = static_cast<const AbstractLineImp*>( parents[1] )->data();
    const double vecprod = degline.dir().y * line.dir().x - degline.dir().x * line.dir().y;
    /*
     * mp: In this case only one of the two points must be valid (the other is "pushed"
     * to infinity).  The choice of which one is done such that we avoid abrupt points exchange
     * when dinamically movint points
     */
    if (side*vecprod < 0)
    {
      Coordinate p = calcIntersectionPoint( degline, line );
      return new PointImp( p );
    }
    return new InvalidImp();
  }

  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  int side = static_cast<const IntImp*>( parents[2] )->data();
  assert( side == 1 || side == -1 );
  const LineData line = static_cast<const AbstractLineImp*>( parents[1] )->data();

  const ArcImp* c = static_cast<const ArcImp*>( parents[0] );
  const double r = c->radius();
  Coordinate ret = calcArcLineIntersect( c->center(), r*r, c->startAngle(),
                                         c->angle(), line, c->orientation()*side );
  if ( ret.valid() ) return new PointImp( ret );
  else return new InvalidImp;
}

const ObjectImpType* ArcLineIntersectionType::resultId() const
{
  return PointImp::stype();
}
