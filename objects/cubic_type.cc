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

#include "cubic_type.h"

#include "cubic_imp.h"
#include "point_imp.h"
#include "bogus_imp.h"

#include <klocale.h>

static const char cubictpstatement[] = I18N_NOOP( "Construct a cubic curve through this point" );

static const struct ArgsParser::spec argsspecCubicB9P[] =
{
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CubicB9PType )

CubicB9PType::CubicB9PType()
  : ArgsParserObjectType( "CubicB9P", argsspecCubicB9P, 9 )
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
  if ( ! margsparser.checkArgs( os, 2 ) ) return new InvalidImp;

  std::vector<Coordinate> points;
  for ( uint i = 0; i < os.size(); ++i )
    points.push_back( static_cast<const PointImp*>( os[i] )->coordinate() );

  CubicCartesianData d = calcCubicThroughPoints( points );
  if ( d.valid() )
    return new CubicImp( d );
  else
    return new InvalidImp;
}

static const ArgsParser::spec argsspecCubicNodeB6P[] =
{
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CubicNodeB6PType )

CubicNodeB6PType::CubicNodeB6PType()
  : ArgsParserObjectType( "CubicNodeB6P", argsspecCubicNodeB6P, 6 )
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
  if ( ! margsparser.checkArgs( parents, 2 ) ) return new InvalidImp;

  std::vector<Coordinate> points;
  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    points.push_back( static_cast<const PointImp*>( *i )->coordinate() );

  CubicCartesianData d = calcCubicNodeThroughPoints( points );
  if ( d.valid() )
    return new CubicImp( d );
  else
    return new InvalidImp;
}

static const ArgsParser::spec argsspecCubicCuspB4P[] =
{
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true },
  { PointImp::stype(), cubictpstatement,
    I18N_NOOP( "Select a point for the new cubic to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CubicCuspB4PType )

CubicCuspB4PType::CubicCuspB4PType()
  : ArgsParserObjectType( "CubicCuspB4P", argsspecCubicCuspB4P, 4 )
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
  if ( ! margsparser.checkArgs( parents, 2 ) ) return new InvalidImp;

  std::vector<Coordinate> points;
  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    points.push_back( static_cast<const PointImp*>( *i )->coordinate() );

  CubicCartesianData d = calcCubicCuspThroughPoints( points );
  if ( d.valid() ) return new CubicImp( d );
  else return new InvalidImp;
}

const ObjectImpType* CubicB9PType::resultId() const
{
  return CubicImp::stype();
}

const ObjectImpType* CubicNodeB6PType::resultId() const
{
  return CubicImp::stype();
}

const ObjectImpType* CubicCuspB4PType::resultId() const
{
  return CubicImp::stype();
}
