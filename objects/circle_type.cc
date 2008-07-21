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

#include "circle_type.h"

#include <math.h>

#include "circle_imp.h"
#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"
#include "special_imptypes.h"

#include "../misc/common.h"

#include <klocale.h>

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

ObjectImp* CircleBCPType::calc( const Coordinate& a, const Coordinate& b ) const
{
  return new CircleImp( a, ( b - a ).length() );
}

const CircleBTPType* CircleBTPType::instance()
{
  static const CircleBTPType t;
  return &t;
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
    return new CircleImp( center, (center - a ).length() );

  /*
   * case of collinear points, we need to identify the intermediate one
   */

  double xmin = fmin( a.x, fmin( b.x, c.x) );
  double xmax = fmax( a.x, fmax( b.x, c.x) );
  double ymin = fmin( a.y, fmin( b.y, c.y) );
  double ymax = fmax( a.y, fmax( b.y, c.y) );
  double axy, bxy, cxy;

  /* decide whether to work with x coordinate or y coordinate */

  if ( xmax - xmin > ymax - ymin )
  {
    axy = a.x;
    bxy = b.x;
    cxy = c.x;
  } else
  {
    axy = a.y;
    bxy = b.y;
    cxy = c.y;
  }

  /*
   * compute baricentric coordinate of c with respect to a and b
   * (if a and c are not coincident)
   */
  if ( fabs( cxy - axy ) < 1e-6*fabs( bxy - axy ) ) return new InvalidImp;
  double t = (bxy - axy)/(cxy - axy);

  if ( fabs( t ) < 1e-6  || fabs( 1.0 - t ) < 1e-6 ) return new InvalidImp;

  /*
   * t < 0:     a between c and b
   * 0 < t < 1: b between a and c
   * t > 1:     c between a and b
   */
  if ( t < 0.0 ) return new LineImp( c, b );
  if ( t > 1.0 ) return new LineImp( a, b );
  return new LineImp( a, c );
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
