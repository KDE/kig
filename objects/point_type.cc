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

const FixedPointType FixedPointType::sself;
const ConstrainedPointType ConstrainedPointType::sself;

const ArgParser::spec FixedPointType::argsspec[1] =
{
  { ObjectImp::ID_DoubleImp, 2 }
};

const ArgParser::spec ConstrainedPointType::argsspec[2] =
{
  { ObjectImp::ID_DoubleImp, 1 },
  { ObjectImp::ID_CurveImp, 1 }
};

FixedPointType::FixedPointType()
  : ObjectType( "point", "FixedPoint", argsspec, 1 )
{
}

FixedPointType::~FixedPointType()
{
}

ObjectImp* FixedPointType::calc( const Args& parents, const KigWidget& ) const
{
  assert( parents.size() == 2 );
  assert( parents[0]->inherits( ObjectImp::ID_DoubleImp ) );
  assert( parents[1]->inherits( ObjectImp::ID_DoubleImp ) );
  double a = static_cast<const DoubleImp*>( parents[0] )->data();
  double b = static_cast<const DoubleImp*>( parents[1] )->data();
  PointImp* d = new PointImp( Coordinate( a, b ) );
  return d;
}

const FixedPointType* FixedPointType::instance()
{
  return &sself;
}

const ConstrainedPointType* ConstrainedPointType::instance()
{
  return &sself;
}

ObjectImp* ConstrainedPointType::calc( const Args& parents, const KigWidget& ) const
{
  assert( parents.size() == 2 );
  assert( parents[0]->inherits( ObjectImp::ID_DoubleImp ) );
  assert( parents[1]->inherits( ObjectImp::ID_CurveImp ) );
  double param = static_cast<const DoubleImp*>( parents[0] )->data();
  return new PointImp( static_cast<const CurveImp*>( parents[1] )->getPoint( param ) );
}

ConstrainedPointType::ConstrainedPointType()
  : ObjectType( "point", "ConstrainedPoint", argsspec, 2 )
{
}

ConstrainedPointType::~ConstrainedPointType()
{
}

void FixedPointType::move( Object* ourobj, const Coordinate&,
                           const Coordinate& dist ) const
{
  // fetch the old coord..;
  const Args fa = ourobj->fixedArgs();
  assert( fa.size() == 2 );
  assert( fa.front()->inherits( ObjectImp::ID_DoubleImp ) );
  assert( fa.back()->inherits( ObjectImp::ID_DoubleImp ) );
  const DoubleImp* ox = static_cast<const DoubleImp*>( fa.front() );
  const DoubleImp* oy = static_cast<const DoubleImp*>( fa.back() );

  // the new fixedargs..
  Args na;
  na.push_back( new DoubleImp( ox->data() + dist.x ) );
  na.push_back( new DoubleImp( oy->data() + dist.y ) );

  // commit..
  ourobj->reset( this, na, ourobj->parents() );
}

void ConstrainedPointType::move( Object* ourobj, const Coordinate&,
                                 const Coordinate& dist ) const
{
  // fetch the new coord the user wants..
  assert( ourobj->has( ObjectImp::ID_PointImp ) );
  const PointImp* oi = static_cast<const PointImp*>( ourobj->imp() );
  const Coordinate oc = oi->coordinate();
  const Coordinate nc = oc + dist;

  // fetch the CurveImp..
  const Objects parents = ourobj->parents();
  assert( parents.size() == 1 );
  assert( parents.front()->has( ObjectImp::ID_CurveImp ) );
  const CurveImp* ci = static_cast<const CurveImp*>( parents.front()->imp() );

  // fetch the new param..
  const double np = ci->getParam( nc );

  // new fixedArgs..
  Args na;
  na.push_back( new DoubleImp( np ) );

  // commit..
  ourobj->reset( this, na, ourobj->parents() );
}

bool ConstrainedPointType::canMove() const
{
  return true;
}

bool FixedPointType::canMove() const
{
  return true;
}

MidPointType::MidPointType()
  : ObjectABType( "point", "MidPoint" )
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
