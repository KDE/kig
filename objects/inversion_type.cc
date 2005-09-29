// Copyright (C)  2005  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#include "inversion_type.h"
#include "point_imp.h"
#include "line_imp.h"
#include "circle_imp.h"
#include "other_imp.h"
#include "bogus_imp.h"

#include "../misc/common.h"

#include <klocale.h>

static const char str1[] = I18N_NOOP( "Invert with respect to this circle" );
static const char str2[] = I18N_NOOP( "Select the circle we want to invert against..." );

static const ArgsParser::spec argsspecInvertPoint[] =
{
  { PointImp::stype(), I18N_NOOP( "Compute the inversion of this point" ),
    I18N_NOOP( "Select the point to invert..." ), false },
  { CircleImp::stype(), str1, str2, false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( InvertPointType )

InvertPointType::InvertPointType()
  : ArgsParserObjectType( "InvertPoint", argsspecInvertPoint, 2 )
{
}

InvertPointType::~InvertPointType()
{
}

const InvertPointType* InvertPointType::instance()
{
  static const InvertPointType s;
  return &s;
}

const ObjectImpType* InvertPointType::resultId() const
{
  return PointImp::stype();
}

ObjectImp* InvertPointType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  const CircleImp* c = static_cast<const CircleImp*>( args[1] );
  Coordinate center = c->center();
  Coordinate relp = static_cast<const PointImp*>( args[0] )->coordinate() - center;
  double radiussq = c->squareRadius();
  double normsq = relp.x*relp.x + relp.y*relp.y;
  if ( normsq == 0 ) return new InvalidImp;
  return new PointImp( center + (radiussq/normsq)*relp );
}

/*
 * inversion of a line
 */

static const ArgsParser::spec argsspecInvertLine[] =
{
  { LineImp::stype(), I18N_NOOP( "Compute the inversion of this line" ),
    I18N_NOOP( "Select the line to invert..." ), false },
  { CircleImp::stype(), str1, str2, false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( InvertLineType )

InvertLineType::InvertLineType()
  : ArgsParserObjectType( "InvertLine", argsspecInvertLine, 2 )
{
}

InvertLineType::~InvertLineType()
{
}

const InvertLineType* InvertLineType::instance()
{
  static const InvertLineType s;
  return &s;
}

const ObjectImpType* InvertLineType::resultId() const
{
  return CircleImp::stype();
}

ObjectImp* InvertLineType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  const CircleImp* c = static_cast<const CircleImp*>( args[1] );
  Coordinate center = c->center();
  double radiussq = c->squareRadius();
  const LineData line = static_cast<const AbstractLineImp*>( args[0] )->data();
  Coordinate relb = line.b - center;
  Coordinate ab = line.b - line.a;
  double t = (relb.x*ab.x + relb.y*ab.y)/(ab.x*ab.x + ab.y*ab.y);
  Coordinate relh = relb - t*ab;
  double normhsq = relh.x*relh.x + relh.y*relh.y;
  if ( normhsq < 1e-12*radiussq ) return new LineImp( line.a, line.b );
  Coordinate newcenter = center + 0.5*radiussq/normhsq*relh;
  double newradius = 0.5*radiussq/sqrt(normhsq);

  return new CircleImp( newcenter, newradius );
}

/*
 * inversion of a segment
 */

static const ArgsParser::spec argsspecInvertSegment[] =
{
  { SegmentImp::stype(), I18N_NOOP( "Compute the inversion of this segment" ),
    I18N_NOOP( "Select the segment to invert..." ), false },
  { CircleImp::stype(), str1, str2, false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( InvertSegmentType )

InvertSegmentType::InvertSegmentType()
  : ArgsParserObjectType( "InvertSegment", argsspecInvertSegment, 2 )
{
}

InvertSegmentType::~InvertSegmentType()
{
}

const InvertSegmentType* InvertSegmentType::instance()
{
  static const InvertSegmentType s;
  return &s;
}

const ObjectImpType* InvertSegmentType::resultId() const
{
  return ArcImp::stype();
}

ObjectImp* InvertSegmentType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  const CircleImp* c = static_cast<const CircleImp*>( args[1] );
  Coordinate center = c->center();
  double radiussq = c->squareRadius();
  const LineData line = static_cast<const AbstractLineImp*>( args[0] )->data();
  Coordinate rela = line.a - center;
  Coordinate relb = line.b - center;
  Coordinate ab = relb - rela;
  double t = (relb.x*ab.x + relb.y*ab.y)/(ab.x*ab.x + ab.y*ab.y);
  Coordinate relh = relb - t*ab;
  double normhsq = relh.x*relh.x + relh.y*relh.y;

  /*
   * compute the inversion of the two endpoints
   */

  Coordinate newcenterrel = 0.5*radiussq/normhsq*relh;
  Coordinate relainv = radiussq/rela.squareLength() * rela;
  Coordinate relbinv = radiussq/relb.squareLength() * relb;

  if ( normhsq < 1e-12*radiussq )
  {
    if ( rela.squareLength() < 1e-12 )
    {
      return new RayImp( relbinv + center, 2*relbinv + center );
    }
    if ( relb.squareLength() < 1e-12 )
    {
      return new RayImp( relainv + center, 2*relainv + center );
    }
    if ( relb.x*rela.x + relb.y*rela.y > 0 )
    {
      return new SegmentImp( relainv + center, relbinv + center );
    }
    return new InvalidImp();
  }
  double newradius = 0.5*radiussq/sqrt(normhsq);

  relainv -= newcenterrel;
  relbinv -= newcenterrel;
  double angle1 = atan2( relainv.y, relainv.x );
  double angle2 = atan2( relbinv.y, relbinv.x );
  double angle = angle2 - angle1;
  if ( ab.x*rela.y - ab.y*rela.x > 0 )
  {
    angle1 = angle2;
    angle = -angle;
  }
  while ( angle1 < 0 ) angle1 += 2*M_PI;
  while ( angle1 >= 2*M_PI ) angle1 -= 2*M_PI;
  while ( angle < 0 ) angle += 2*M_PI;
  while ( angle >= 2*M_PI ) angle -= 2*M_PI;
  return new ArcImp( newcenterrel + center, newradius, angle1, angle );
}

/*
 * inversion of a circle
 */

static const ArgsParser::spec argsspecInvertCircle[] =
{
  { CircleImp::stype(), I18N_NOOP( "Compute the inversion of this circle" ),
    I18N_NOOP( "Select the circle to invert..." ), false },
  { CircleImp::stype(), str1, str2, false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( InvertCircleType )

InvertCircleType::InvertCircleType()
  : ArgsParserObjectType( "InvertCircle", argsspecInvertCircle, 2 )
{
}

InvertCircleType::~InvertCircleType()
{
}

const InvertCircleType* InvertCircleType::instance()
{
  static const InvertCircleType s;
  return &s;
}

const ObjectImpType* InvertCircleType::resultId() const
{
  return CircleImp::stype();
}

ObjectImp* InvertCircleType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  const CircleImp* refcircle = static_cast<const CircleImp*>( args[1] );
  Coordinate refc = refcircle->center();
  double refrsq = refcircle->squareRadius();
  const CircleImp* circle = static_cast<const CircleImp*>( args[0] );
  Coordinate c = circle->center() - refc;
  double clength = c.length();
  Coordinate cnorm = Coordinate (1.,0.);
  if ( clength != 0.0 ) cnorm = c/clength;
  double r = circle->radius();
  Coordinate tc = r*cnorm;
  Coordinate b = c + tc;   //(1 + t)*c;
  double bsq = b.x*b.x + b.y*b.y;
  Coordinate bprime = refrsq*b/bsq;
  if ( std::fabs( clength - r ) < 1e-6*clength )       // circle through origin -> line
  {
    return new LineImp( bprime+refc, bprime+refc+Coordinate( -c.y, c.x ) );
  }

  Coordinate a = c - tc;
  double asq = a.x*a.x + a.y*a.y;
  Coordinate aprime = refrsq*a/asq;

  Coordinate cprime = 0.5*(aprime + bprime);
  double rprime = 0.5*( bprime - aprime ).length();

  return new CircleImp( cprime + refc, rprime );
}

/*
 * inversion of an arc
 */

static const ArgsParser::spec argsspecInvertArc[] =
{
  { ArcImp::stype(), I18N_NOOP( "Compute the inversion of this arc" ),
    I18N_NOOP( "Select the arc to invert..." ), false },
  { CircleImp::stype(), str1, str2, false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( InvertArcType )

InvertArcType::InvertArcType()
  : ArgsParserObjectType( "InvertArc", argsspecInvertArc, 2 )
{
}

InvertArcType::~InvertArcType()
{
}

const InvertArcType* InvertArcType::instance()
{
  static const InvertArcType s;
  return &s;
}

const ObjectImpType* InvertArcType::resultId() const
{
  return ArcImp::stype();
}

ObjectImp* InvertArcType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  const CircleImp* refcircle = static_cast<const CircleImp*>( args[1] );
  Coordinate refc = refcircle->center();
  double refrsq = refcircle->squareRadius();
  const ArcImp* arc = static_cast<const ArcImp*>( args[0] );
  Coordinate c = arc->center() - refc;
  double clength = c.length();
  Coordinate cnorm = Coordinate (1.,0.);
  if ( clength != 0.0 ) cnorm = c/clength;
  double r = arc->radius();
  /*
   * r > clength means center of inversion circle inside of circle supporting arc
   */
  Coordinate tc = r*cnorm;
  Coordinate b = c + tc;
  double bsq = b.x*b.x + b.y*b.y;
  Coordinate bprime = refrsq*b/bsq;
  if ( std::fabs( clength - r ) < 1e-6*clength )  // support circle through origin -> 
                                                  // segment, ray or invalid
                                                  // (reversed segment, union of two rays)
  {
    bool valid1 = false;
    bool valid2 = false;
    Coordinate ep1 = arc->firstEndPoint() - refc;
    Coordinate ep2 = arc->secondEndPoint() - refc;
    Coordinate ep1inv = Coordinate::invalidCoord();
    Coordinate ep2inv = Coordinate::invalidCoord();
    double ep1sq = ep1.squareLength();
    if ( ep1sq > 1e-12 )
    {
      valid1 = true;
      ep1inv = refrsq/ep1sq * ep1;
    }
    Coordinate rayendp = ep1inv;
    int sign = 1;
    double ep2sq = ep2.squareLength();
    if ( ep2sq > 1e-12 )
    {
      valid2 = true;
      ep2inv = refrsq/ep2sq * ep2;
      rayendp = ep2inv;
      sign = -1;
    }
    if ( valid1 || valid2 )
    {
      if ( valid1 && valid2 )
      {
        // this gives either a segment or the complement of a segment (relative
        // to its support line).  We return a segment in any case (fixme)
        double ang = atan2( -c.y, -c.x );
        double sa = arc->startAngle();
        if ( ang < sa ) ang += 2*M_PI;
        if ( ang - sa - arc->angle() < 0 ) return new InvalidImp();
        return new SegmentImp( ep1inv + refc, ep2inv + refc );
      } else
        return new RayImp ( rayendp + refc, 
                 rayendp + refc + sign*Coordinate( -c.y, c.x ) );  // this should give a Ray
    } else
      return new LineImp( bprime+refc, bprime+refc+Coordinate( -c.y, c.x ) );
  }

  Coordinate a = c - tc;
  double asq = a.x*a.x + a.y*a.y;
  Coordinate aprime = refrsq*a/asq;

  Coordinate cprime = 0.5*(aprime + bprime);
  double rprime = 0.5*( bprime - aprime ).length();

  Coordinate ep1 = arc->firstEndPoint() - refc;
  double ang1 = arc->startAngle();
  double newstartangle = 2*atan2(ep1.y,ep1.x) - ang1;
  Coordinate ep2 = arc->secondEndPoint() - refc;
  double ang2 = ang1 + arc->angle();
  double newendangle = 2*atan2(ep2.y,ep2.x) - ang2;
  double newangle = newendangle - newstartangle;

  /*
   * newstartangle and newendangle might have to be exchanged:
   * this is the case if the circle supporting our arc does not
   * contain the center of the inversion circle
   */
  if ( r < clength )
  {
    newstartangle = newendangle - M_PI;
    newangle = - newangle;
    // newendangle is no-longer valid
  }
  while ( newstartangle < 0 ) newstartangle += 2*M_PI;
  while ( newstartangle >= 2*M_PI ) newstartangle -= 2*M_PI;
  while ( newangle < 0 ) newangle += 2*M_PI;
  while ( newangle >= 2*M_PI ) newangle -= 2*M_PI;
  return new ArcImp( cprime + refc, rprime, newstartangle, newangle );
}

