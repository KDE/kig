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

#include <vector>

static const char* cubictpstatement = I18N_NOOP( "Construct a cubic through this point" );

static const struct ArgParser::spec argsspec9p[] =
{
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement }
};

CubicB9PType::CubicB9PType()
  : BuiltinType( "CubicB9P", argsspec9p, 1 )
{
}

CubicB9PType::~CubicB9PType()
{
}

const CubicB9PType* CubicB9PType::instance()
{
  static const CubicB9PType t;
  return &t;
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
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement }
};

CubicNodeB6PType::CubicNodeB6PType()
  : BuiltinType( "CubicNodeB6P", argsspec6p, 1 )
{
}

CubicNodeB6PType::~CubicNodeB6PType()
{
}

const CubicNodeB6PType* CubicNodeB6PType::instance()
{
  static const CubicNodeB6PType t;
  return &t;
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

static const ArgParser::spec argsspec4p[] =
{
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement }
};

CubicCuspB4PType::CubicCuspB4PType()
  : BuiltinType( "CubicCuspB4P", argsspec4p, 1 )
{
}

CubicCuspB4PType::~CubicCuspB4PType()
{
}

const CubicCuspB4PType* CubicCuspB4PType::instance()
{
  static const CubicCuspB4PType t;
  return &t;
}

ObjectImp* CubicCuspB4PType::calc( const Args& parents ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  if ( parents.size() > 4 ) return new InvalidImp;
  std::vector<Coordinate> points;

  for ( uint i = 0; i < parents.size(); ++i )
  {
    assert( parents[i]->inherits( ObjectImp::ID_PointImp ) );
    points.push_back(
      static_cast<const PointImp*>( parents[i] )->coordinate() );
  };
  return new CubicImp( calcCubicCuspThroughPoints( points ) );
}
