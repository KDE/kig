// other_type.cc
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

#include "other_type.h"

#include "bogus_imp.h"
#include "other_imp.h"
#include "point_imp.h"

#include <math.h>

AngleType::AngleType()
  : ObjectABCType( "angle", "Angle" )
{
}

AngleType::~AngleType()
{
}

ObjectImp* AngleType::calc( const Args& parents, const KigWidget& ) const
{
  if ( parents.size() < 3 ) return new InvalidImp;
  std::vector<Coordinate> points;
  for ( uint i = 0; i < parents.size(); ++i )
    if ( parents[i]->inherits( ObjectImp::ID_PointImp ) )
      points.push_back(
        static_cast<const PointImp*>( parents[i] )->coordinate() );
  if ( points.size() < 3 ) return new InvalidImp;

  Coordinate lvect = points[0] - points[1];
  Coordinate rvect = points[2] - points[1];
  lvect = lvect.normalize();
  rvect = rvect.normalize();

  double startangle = std::atan2( lvect.y, lvect.x );
  double endangle = std::atan2( rvect.y, rvect.x );
  double anglelength = endangle - startangle;
  if ( anglelength < 0 ) anglelength += 2* M_PI;
  if ( startangle < 0 ) startangle += 2*M_PI;

  return new AngleImp( points[1], startangle, anglelength );
}

VectorType::VectorType()
  : ObjectABType( "vector", "Vector" )
{
}

VectorType::~VectorType()
{
}

ObjectImp* VectorType::calc( const Coordinate& a, const Coordinate& b ) const
{
  return new VectorImp( a, b );
}

ObjectType* AngleType::copy() const
{
  return new AngleType;
}

ObjectType* VectorType::copy() const
{
  return new AngleType;
}
