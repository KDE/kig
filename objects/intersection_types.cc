// intersection_types.cc
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "intersection_types.h"

#include "bogus_imp.h"
#include "point_imp.h"
#include "conic_imp.h"
#include "cubic_imp.h"
#include "circle_imp.h"
#include "line_imp.h"

static const ArgParser::spec argsspecConicLineIntersection[] =
{
  { ObjectImp::ID_ConicImp, I18N_NOOP( "Intersect with this conic" ) },
  { ObjectImp::ID_LineImp, I18N_NOOP( "Intersect with this line" ) },
  { ObjectImp::ID_IntImp, "param" }
};

ConicLineIntersectionType::ConicLineIntersectionType()
  : ArgparserObjectType( "ConicLineIntersection",
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

ObjectImp* ConicLineIntersectionType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() != 3 ) return new InvalidImp;
  Args p = margsparser.parse( parents );
  if ( ! p[0] || !p[1] || !p[2] )
    return new InvalidImp;
  assert ( p[0]->inherits( ObjectImp::ID_ConicImp ) &&
           p[1]->inherits( ObjectImp::ID_LineImp ) &&
           p[2]->inherits( ObjectImp::ID_IntImp ) );

  int side = static_cast<const IntImp*>( p[2] )->data();
  assert( side == 1 || side == -1 );
  const LineData line = static_cast<const AbstractLineImp*>( p[1] )->data();

  bool valid = true;
  Coordinate ret;
  if ( p[0]->inherits( ObjectImp::ID_CircleImp ) )
  {
    // easy case..
    const CircleImp* c = static_cast<const CircleImp*>( p[0] );
    ret = calcCircleLineIntersect(
      c->center(), c->squareRadius(),
      line, side, valid );
  }
  else
  {
    // harder case..
    ret = calcConicLineIntersect(
      static_cast<const ConicImp*>( p[0] )->cartesianData(),
      line, side, valid );
  }
  if ( valid ) return new PointImp( ret );
  else return new InvalidImp;
}

static const char* constructlinestat = I18N_NOOP( "Intersect with this line" );

static const ArgParser::spec argsspecLineLineIntersection[] =
{
  { ObjectImp::ID_LineImp, constructlinestat },
  { ObjectImp::ID_LineImp, constructlinestat }
};

LineLineIntersectionType::LineLineIntersectionType()
  : ArgparserObjectType( "LineLineIntersection",
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

ObjectImp* LineLineIntersectionType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() != 2 ||
       !parents[0]->inherits( ObjectImp::ID_LineImp ) ||
       !parents[1]->inherits( ObjectImp::ID_LineImp ) )
    return new InvalidImp;
  return new PointImp(
    calcIntersectionPoint(
      static_cast<const AbstractLineImp*>( parents[0] )->data(),
      static_cast<const AbstractLineImp*>( parents[1] )->data() ) );
}

static const ArgParser::spec argsspecLineCubicIntersection[] =
{
  { ObjectImp::ID_CubicImp, I18N_NOOP( "Intersect with this cubic" ) },
  { ObjectImp::ID_LineImp, I18N_NOOP( "Intersect with this line" ) },
  { ObjectImp::ID_IntImp, "param" }
};

LineCubicIntersectionType::LineCubicIntersectionType()
  : ArgparserObjectType( "LineCubicIntersection",
                         argsspecLineCubicIntersection, 3 )
{
}

LineCubicIntersectionType::~LineCubicIntersectionType()
{
}

const LineCubicIntersectionType* LineCubicIntersectionType::instance()
{
  static const LineCubicIntersectionType t;
  return &t;
}

ObjectImp* LineCubicIntersectionType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() != 3 ) return new InvalidImp;
  Args p = margsparser.parse( parents );
  if ( ! p[0] || ! p[1] || ! p[2] ) return new InvalidImp;
  int which = static_cast<const IntImp*>( p[2] )->data();
  bool valid = true;
  const Coordinate c = calcCubicLineIntersect(
    static_cast<const CubicImp*>( p[0] )->data(),
    static_cast<const AbstractLineImp*>( p[1] )->data(),
    which, valid );
  if ( valid ) return new PointImp( c );
  else return new InvalidImp;
}

int ConicLineIntersectionType::resultId() const
{
  return ObjectImp::ID_PointImp;
}

int LineLineIntersectionType::resultId() const
{
  return ObjectImp::ID_PointImp;
}

int LineCubicIntersectionType::resultId() const
{
  return ObjectImp::ID_PointImp;
}
