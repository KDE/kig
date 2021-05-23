// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "circle_type.h"

#include <math.h>

#include "circle_imp.h"
#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"
#include "special_imptypes.h"

#include "../misc/common.h"

static const char constructcirclethroughpointstat[] = I18N_NOOP( "Construct a circle through this point" );

static const char constructcirclewithcenterstat[] = I18N_NOOP( "Construct a circle with this center" );

static const ArgsParser::spec argsspecCircleBCP[] =
{
  { PointImp::stype(), constructcirclewithcenterstat,
    I18N_NOOP( "Select the center of the new circle..." ), false },
  { PointImp::stype(), constructcirclethroughpointstat,
    I18N_NOOP( "Select a point for the new circle to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CircleBCPType )

CircleBCPType::CircleBCPType()
  : ObjectABType( "CircleBCP", argsspecCircleBCP, 2 )
{
}

CircleBCPType::~CircleBCPType()
{
}

const CircleBCPType* CircleBCPType::instance()
{
  static const CircleBCPType s;
  return &s;
}

ObjectImp* CircleBCPType::calcx( const Coordinate& a, const Coordinate& b ) const
{
  return new CircleImp( a, ( b - a ).length() );
}

static const ArgsParser::spec argsspecCircleBTP[] =
{
  { PointImp::stype(), constructcirclethroughpointstat,
    I18N_NOOP( "Select a point for the new circle to go through..." ), true },
  { PointImp::stype(), constructcirclethroughpointstat,
    I18N_NOOP( "Select a point for the new circle to go through..." ), true },
  { PointImp::stype(), constructcirclethroughpointstat,
    I18N_NOOP( "Select a point for the new circle to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CircleBTPType )

CircleBTPType::CircleBTPType()
  : ArgsParserObjectType( "CircleBTP", argsspecCircleBTP, 3 )
{
}

CircleBTPType::~CircleBTPType()
{
}

const CircleBTPType* CircleBTPType::instance()
{
  static const CircleBTPType t;
  return &t;
}

ObjectImp* CircleBTPType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args, 2 ) ) return new InvalidImp;

  const Coordinate a = static_cast<const PointImp*>( args[0] )->coordinate();
  const Coordinate b = static_cast<const PointImp*>( args[1] )->coordinate();
  Coordinate c;
  if ( args.size() == 3 )
    c = static_cast<const PointImp*>( args[2] )->coordinate();
  else
  {
    // we pick the third point so that the three points form a
    // triangle with equal sides...

    // midpoint:
    Coordinate m = ( b + a ) / 2;
    if ( b.y != a.y )
    {
      // direction of the perpend:
      double d = -(b.x-a.x)/(b.y-a.y);

      // length:
      // sqrt( 3 ) == tan( 60° ) == sqrt( 2^2 - 1^2 )
      double l = 1.73205080756 * (a-b).length() / 2;

      double d2 = d*d;
      double l2 = l*l;
      double dx = sqrt( l2 / ( d2 + 1 ) );
      double dy = sqrt( l2 * d2 / ( d2 + 1 ) );
      if( d < 0 ) dy = -dy;

      c.x = m.x + dx;
      c.y = m.y + dy;
    }
    else
    {
      c.x = m.x;
      c.y = m.y + ( a.x - b.x );
    };
  };

  const Coordinate center = calcCenter( a, b, c );
  if ( center.valid() )
  {
    /* this is also done in calcCenter... should optimize in some way */
    double xdo = b.x-a.x;
    double ydo = b.y-a.y;

    double xao = c.x-a.x;
    double yao = c.y-a.y;

    double determinant = (xdo * yao - xao * ydo);
    if (determinant > 0) return new CircleImp( center, (center - a ).length() );
      else return new CircleImp( center, -(center - a ).length() );
  }

  /*
   * case of collinear points, we need to identify the intermediate one
   */

  double xmin = fmin( a.x, fmin( b.x, c.x) );
  double xmax = fmax( a.x, fmax( b.x, c.x) );
  double ymin = fmin( a.y, fmin( b.y, c.y) );
  double ymax = fmax( a.y, fmax( b.y, c.y) );
  double d, axy, bxy, cxy;

  /* decide whether to work with x coordinate or y coordinate */

  if ( xmax - xmin > ymax - ymin )
  {
    axy = a.x;
    bxy = b.x;
    cxy = c.x;
    d = xmax - xmin;
  } else
  {
    axy = a.y;
    bxy = b.y;
    cxy = c.y;
    d = ymax - ymin;
  }

  if ( fabs( axy - cxy ) >= d ) // b between a and c
    return new LineImp( a, c );
  if ( fabs( cxy - bxy ) >= d ) // a between c and b
    return new LineImp( c, b );

  // otherwise: c between b and a
  return new LineImp( b, a);

  /*
   * mp: note that the orientation of the new line is from a to c
   * if b is intermediate, otherwise it is reversed whenever
   * two of the three points cross each-other.
   * This should give consistent results when intersecting circles that
   * degenerate into lines
   */
}

const ObjectImpType* CircleBCPType::resultId() const
{
  return CircleImp::stype();
}

const ObjectImpType* CircleBTPType::resultId() const
{
  return CircleImp::stype();
}

static const ArgsParser::spec argsspecCircleBPR[] =
{
  { PointImp::stype(), constructcirclewithcenterstat,
      I18N_NOOP( "Select the center of the new circle..." ), false },
  { &lengthimptypeinstance, I18N_NOOP( "With this radius" ), 
      I18N_NOOP( "Select the length of the radius..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CircleBPRType )

CircleBPRType::CircleBPRType()
  : ArgsParserObjectType( "CircleBPR", argsspecCircleBPR, 2 )
{
}

CircleBPRType::~CircleBPRType()
{
}

const CircleBPRType* CircleBPRType::instance()
{
  static const CircleBPRType t;
  return &t;
}

ObjectImp* CircleBPRType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;
  const Coordinate c = static_cast<const PointImp*>( args[0] )->coordinate();
  bool valid;
  double r = getDoubleFromImp( args[1], valid);
  if ( ! valid ) return new InvalidImp;
  r = fabs( r );
  // double r = static_cast<const DoubleImp*>( args[1] )->data();
  return new CircleImp( c, r );
}

const ObjectImpType* CircleBPRType::resultId() const
{
  return CircleImp::stype();
}
