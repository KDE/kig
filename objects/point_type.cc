// point_type.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#include "point_type.h"

#include "object.h"
#include "point_imp.h"
#include "curve_imp.h"
#include "bogus_imp.h"

static const ArgParser::spec argsspecdd[] =
{
  { ObjectImp::ID_DoubleImp, "x" },
  { ObjectImp::ID_DoubleImp, "y" }
};

FixedPointType::FixedPointType()
  : ArgparserObjectType( "FixedPoint", argsspecdd, 1 )
{
}

FixedPointType::~FixedPointType()
{
}

ObjectImp* FixedPointType::calc( const Args& parents, const KigDocument& ) const
{
  assert( parents.size() == 2 );
  assert( parents[0]->inherits( ObjectImp::ID_DoubleImp ) );
  assert( parents[1]->inherits( ObjectImp::ID_DoubleImp ) );
  double a = static_cast<const DoubleImp*>( parents[0] )->data();
  double b = static_cast<const DoubleImp*>( parents[1] )->data();
  PointImp* d = new PointImp( Coordinate( a, b ) );
  return d;
}

ObjectImp* ConstrainedPointType::calc( const Args& tparents, const KigDocument& doc ) const
{
  assert( tparents.size() == 2 );
  Args parents = margsparser.parse( tparents );
  assert( parents[0]->inherits( ObjectImp::ID_DoubleImp ) );
  assert( parents[1]->inherits( ObjectImp::ID_CurveImp ) );
  double param = static_cast<const DoubleImp*>( parents[0] )->data();
  return new PointImp( static_cast<const CurveImp*>( parents[1] )->getPoint( param, doc ) );
}

const ArgParser::spec argsspecdc[] =
{
  { ObjectImp::ID_DoubleImp, "parameter" },
  { ObjectImp::ID_CurveImp, "Constrain the point to this curve" }
};

ConstrainedPointType::ConstrainedPointType()
  : ArgparserObjectType( "ConstrainedPoint", argsspecdc, 2 )
{
}

ConstrainedPointType::~ConstrainedPointType()
{
}

void FixedPointType::move( RealObject* ourobj, const Coordinate&,
                           const Coordinate& dist ) const
{
  // fetch the old coord..;
  Objects pa = ourobj->parents();
  assert( pa.size() == 2 );
  assert( pa.front()->inherits( Object::ID_DataObject ) );
  assert( pa.back()->inherits( Object::ID_DataObject ) );

  DataObject* ox = static_cast<DataObject*>( pa.front() );
  DataObject* oy = static_cast<DataObject*>( pa.back() );

  assert( ox->hasimp( ObjectImp::ID_DoubleImp ) );
  assert( oy->hasimp( ObjectImp::ID_DoubleImp ) );

  const DoubleImp* dx = static_cast<const DoubleImp*>( pa.front()->imp() );
  const DoubleImp* dy = static_cast<const DoubleImp*>( pa.back()->imp() );

  ox->setImp( new DoubleImp( dx->data() + dist.x ) );
  oy->setImp( new DoubleImp( dy->data() + dist.y ) );
}

void ConstrainedPointType::move( RealObject* ourobj, const Coordinate&,
                                 const Coordinate& dist ) const
{
  // fetch the new coord the user wants..
  assert( ourobj->hasimp( ObjectImp::ID_PointImp ) );
  const PointImp* oi = static_cast<const PointImp*>( ourobj->imp() );
  const Coordinate oc = oi->coordinate();
  const Coordinate nc = oc + dist;

  // fetch the CurveImp..
  Objects parents = ourobj->parents();
  assert( parents.size() == 2 );
  assert( parents.back()->hasimp( ObjectImp::ID_CurveImp ) );
  const CurveImp* ci = static_cast<const CurveImp*>( parents.back()->imp() );

  // fetch the new param..
  const double np = ci->getParam( nc );

  assert( parents[0]->inherits( Object::ID_DataObject ) );
  assert( parents[0]->hasimp( ObjectImp::ID_DoubleImp ) );

  static_cast<DataObject*>( parents[0] )->setImp( new DoubleImp( np ) );
}

bool ConstrainedPointType::canMove() const
{
  return true;
}

bool FixedPointType::canMove() const
{
  return true;
}

static const ArgParser::spec argsspecMidPoint[] =
{
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct the midpoint of this point" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct the midpoint of this point" ) }
};

MidPointType::MidPointType()
  : ObjectABType( "MidPoint", argsspecMidPoint, 2 )
{
}

MidPointType::~MidPointType()
{
}

const MidPointType* MidPointType::instance()
{
  static const MidPointType t;
  return &t;
}

ObjectImp* MidPointType::calc( const Coordinate& a, const Coordinate& b ) const
{
  return new PointImp( ( a + b ) / 2 );
}

bool ConstrainedPointType::inherits( int type ) const
{
  return type == ID_ConstrainedPointType;
}

const ConstrainedPointType* ConstrainedPointType::instance()
{
  static const ConstrainedPointType t;
  return &t;
}

const FixedPointType* FixedPointType::instance()
{
  static const FixedPointType t;
  return &t;
}

int FixedPointType::resultId() const
{
  return ObjectImp::ID_PointImp;
}

int ConstrainedPointType::resultId() const
{
  return ObjectImp::ID_PointImp;
}

int MidPointType::resultId() const
{
  return ObjectImp::ID_PointImp;
}
