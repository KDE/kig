// cubic_type.cc
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

#include "cubic_type.h"

#include "cubic_imp.h"
#include "point_imp.h"
#include "bogus_imp.h"

static const struct ArgParser::spec argsspec9p[] =
{
  { ObjectImp::ID_PointImp, 9 }
};

CubicB9PType::CubicB9PType()
  : ObjectType( "cubic", "CubicB9P", argsspec9p, 1 )
{
}

CubicB9PType::~CubicB9PType()
{
}

ObjectImp* CubicB9PType::calc( const Args& os ) const
{
  std::vector<Coordinate> points;

  uint size = os.size();
  if ( size < 2 ) return new InvalidImp;
  for ( uint i = 0; i < size; ++i )
    if( os[i]->inherits( ObjectImp::ID_PointImp ) )
      points.push_back( static_cast<const PointImp*>( os[i] )->coordinate() );

  return new CubicImp( calcCubicThroughPoints( points ) );
}

static const ArgParser::spec argsspec6p[] =
{
  { ObjectImp::ID_PointImp, 6 }
};

CubicNodeB6PType::CubicNodeB6PType()
  : ObjectType( "cubic", "CubicNodeB6P", argsspec6p, 1 )
{
}

CubicNodeB6PType::~CubicNodeB6PType()
{
}

ObjectImp* CubicNodeB6PType::calc( const Args& parents ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  std::vector<Coordinate> points;
  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      points.push_back( static_cast<const PointImp*>( *i )->coordinate() );
  if ( points.size() < 2 ) return new InvalidImp;
  return new CubicImp( calcCubicNodeThroughPoints( points ) );
}

ObjectType* CubicNodeB6PType::copy() const
{
  return new CubicNodeB6PType;
}

ObjectType* CubicB9PType::copy() const
{
  return new CubicB9PType;
}
