// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "cubic_type.h"

#include "cubic_imp.h"
#include "point_imp.h"
#include "bogus_imp.h"

static const char cubictpstatement[] = I18N_NOOP( "Construct a cubic curve through this point" );
static const char cubicselectstatement[] = I18N_NOOP( "Select a point for the new cubic to go through..." );

static const struct ArgsParser::spec argsspecCubicB9P[] =
{
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true }
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
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true }
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
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true }
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

static const ArgsParser::spec argsspecVerticalCubicB4P[] =
{
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true },
  { PointImp::stype(), cubictpstatement, cubicselectstatement, true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( VerticalCubicB4PType )

VerticalCubicB4PType::VerticalCubicB4PType()
  : ArgsParserObjectType( "VerticalCubicB4P", argsspecVerticalCubicB4P, 4 )
{
}

VerticalCubicB4PType::~VerticalCubicB4PType()
{
}

const VerticalCubicB4PType* VerticalCubicB4PType::instance()
{
  static const VerticalCubicB4PType t;
  return &t;
}

ObjectImp* VerticalCubicB4PType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents, 2 ) ) return new InvalidImp;

  std::vector<Coordinate> points;
  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
    points.push_back( static_cast<const PointImp*>( *i )->coordinate() );

  CubicCartesianData d = calcCubicThroughPoints( points );
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

const ObjectImpType* VerticalCubicB4PType::resultId() const
{
  return CubicImp::stype();
}
