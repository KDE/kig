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
#include "circle_imp.h"
#include "line_imp.h"

static const ArgParser::spec argsspeccl[] =
{
  { ObjectImp::ID_ConicImp, 1 },
  { ObjectImp::ID_LineImp, 1 },
  { ObjectImp::ID_IntImp, 1 }
};

ConicLineIntersectionType::ConicLineIntersectionType()
  : ObjectType( "point", "ConicLineIntersection",
                argsspeccl, 3 )
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

ObjectImp* ConicLineIntersectionType::calc( const Args& parents,
                                            const KigWidget& ) const
{
  if ( parents.size() < 3 ) return new InvalidImp;
  Args p = margsparser.parse( parents );
  if ( !p[0] || ! p[0]->inherits( ObjectImp::ID_ConicImp ) ||
       !p[1] || ! p[1]->inherits( ObjectImp::ID_LineImp ) ||
       !p[2] || ! p[2]->inherits( ObjectImp::ID_IntImp )
    ) return new InvalidImp;

  int side = static_cast<const IntImp*>( p[2] )->data();
  assert( side == 1 || side == -1 );
  const LineData line = static_cast<const LineImp*>( p[1] )->data();

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

