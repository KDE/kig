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

static const struct ArgParser::spec argsspecCubicB9P[] =
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
  : ArgparserObjectType( "CubicB9P", argsspecCubicB9P, 9 )
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

ObjectImp* CubicB9PType::calc( const Args& os, const KigDocument& ) const
{
  std::vector<Coordinate> points;
  if ( os.size() < 2 ) return new InvalidImp;
  for ( uint i = 0; i < os.size(); ++i )
    if( os[i]->inherits( ObjectImp::ID_PointImp ) )
      points.push_back( static_cast<const PointImp*>( os[i] )->coordinate() );
  if ( points.size() != os.size() ) return new InvalidImp;
  CubicCartesianData d = calcCubicThroughPoints( points );
  if ( d.valid() ) return new CubicImp( d );
  else return new InvalidImp;
}

static const ArgParser::spec argsspecCubicNodeB6P[] =
{
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement }
};

CubicNodeB6PType::CubicNodeB6PType()
  : ArgparserObjectType( "CubicNodeB6P", argsspecCubicNodeB6P, 6 )
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

ObjectImp* CubicNodeB6PType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  std::vector<Coordinate> points;
  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    if ( (*i)->inherits( ObjectImp::ID_PointImp ) )
      points.push_back( static_cast<const PointImp*>( *i )->coordinate() );
  if ( points.size() != parents.size() ) return new InvalidImp;
  CubicCartesianData d = calcCubicNodeThroughPoints( points );
  if ( d.valid() ) return new CubicImp( d );
  else return new InvalidImp;
}

static const ArgParser::spec argsspecCubicCuspB4P[] =
{
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement },
  { ObjectImp::ID_PointImp, cubictpstatement }
};

CubicCuspB4PType::CubicCuspB4PType()
  : ArgparserObjectType( "CubicCuspB4P", argsspecCubicCuspB4P, 4 )
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

ObjectImp* CubicCuspB4PType::calc( const Args& parents, const KigDocument& ) const
{
  if ( parents.size() < 2 ) return new InvalidImp;
  if ( parents.size() > 4 ) return new InvalidImp;
  std::vector<Coordinate> points;

  for ( uint i = 0; i < parents.size(); ++i )
  {
    if( parents[i]->inherits( ObjectImp::ID_PointImp ) )
      points.push_back(
        static_cast<const PointImp*>( parents[i] )->coordinate() );
  };
  if ( points.size() != parents.size() )
    return new InvalidImp;
  CubicCartesianData d = calcCubicCuspThroughPoints( points );
  if ( d.valid() ) return new CubicImp( d );
  else return new InvalidImp;
}

int CubicB9PType::resultId() const
{
  return ObjectImp::ID_CubicImp;
}

int CubicNodeB6PType::resultId() const
{
  return ObjectImp::ID_CubicImp;
}

int CubicCuspB4PType::resultId() const
{
  return ObjectImp::ID_CubicImp;
}
