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
#include "locus_imp.h"
#include "object.h"

#include <functional>
#include <algorithm>
#include <cmath>

static const char* constructanglethroughpoint =
  I18N_NOOP( "Construct an angle through this point" );

static const ArgParser::spec argsspecAngle[] =
{
  { ObjectImp::ID_PointImp, constructanglethroughpoint },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct an angle at this point" ) },
  { ObjectImp::ID_PointImp, constructanglethroughpoint }
};

AngleType::AngleType()
  : ArgparserObjectType( "Angle", argsspecAngle, 3 )
{
}

AngleType::~AngleType()
{
}

const AngleType* AngleType::instance()
{
  static const AngleType t;
  return &t;
}

ObjectImp* AngleType::calc( const Args& parents, const KigDocument& ) const
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

  double startangle = atan2( lvect.y, lvect.x );
  double endangle = atan2( rvect.y, rvect.x );
  double anglelength = endangle - startangle;
  if ( anglelength < 0 ) anglelength += 2* M_PI;
  if ( startangle < 0 ) startangle += 2*M_PI;

  return new AngleImp( points[1], startangle, anglelength );
}

static const ArgParser::spec argsspecVector[] =
{
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a vector from this point" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a vector to this point" ) }
};

VectorType::VectorType()
  : ObjectABType( "Vector", argsspecVector, 2 )
{
}

VectorType::~VectorType()
{
}

const VectorType* VectorType::instance()
{
  static const VectorType t;
  return &t;
}

ObjectImp* VectorType::calc( const Coordinate& a, const Coordinate& b ) const
{
  return new VectorImp( a, b );
}

static const struct ArgParser::spec argsspec1c[] =
{
  { ObjectImp::ID_CurveImp, "curve" }
};

LocusType::LocusType( const ObjectHierarchy& hier )
  : CustomType( "Locus", argsspec1c, 1 ), mhier( hier )
{
}

LocusType::~LocusType()
{
}

ObjectImp* LocusType::calc( const Args& args, const KigDocument& ) const
{
  using namespace std;

  if ( args.size() < 1 ) return new InvalidImp;
  assert( args[0]->inherits( ObjectImp::ID_CurveImp ) );
  const CurveImp* curveimp = static_cast<const CurveImp*>( args.front() );

  Args fixedargs( args.begin() + 1, args.end() );

  return new LocusImp( curveimp->copy(), mhier.withFixedArgs( fixedargs ) );
}

bool LocusType::inherits( int type ) const
{
  return type == ID_LocusType ? true : Parent::inherits( type );
}

const ObjectHierarchy& LocusType::hierarchy() const
{
  return mhier;
}

int AngleType::resultId() const
{
  return ObjectImp::ID_AngleImp;
}

int VectorType::resultId() const
{
  return ObjectImp::ID_VectorImp;
}

int LocusType::resultId() const
{
  return ObjectImp::ID_LocusImp;
}

