// base_type.cc
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

#include "base_type.h"

#include "point_imp.h"
#include "line_imp.h"
#include "bogus_imp.h"
#include "object.h"

#include "../misc/common.h"

ObjectABType::ObjectABType( const char* fulltypename, const ArgParser::spec* spec, int n )
  : ArgparserObjectType( fulltypename, spec, n )
{
}

ObjectABType::~ObjectABType()
{
}

ObjectImp* ObjectABType::calc( const Args& parents, const KigDocument& ) const
{
  Args p = margsparser.parse( parents );
  if ( !p[0] || !p[1] ) return new InvalidImp;
  Coordinate a = static_cast<const PointImp*>( parents[0] )->coordinate();
  Coordinate b = static_cast<const PointImp*>( parents[1] )->coordinate();
  return calc( a, b );
}

bool ObjectABType::canMove() const
{
  return true;
}

void ObjectABType::move( RealObject* o,
                         const Coordinate& to,
                         const KigDocument& d ) const
{
  Objects parents = o->parents();
  assert( parents.size() == 2 );
  assert( parents[0]->hasimp( ObjectImp::ID_PointImp ) );
  assert( parents[1]->hasimp( ObjectImp::ID_PointImp ) );
  const Coordinate a = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  const Coordinate b = static_cast<const PointImp*>( parents[1]->imp() )->coordinate();
  const Coordinate dist = b - a;
  parents[0]->move( to, d );
  parents[1]->move( to + dist, d );
}

ObjectLPType::ObjectLPType( const char* fullname, const ArgParser::spec* spec, int n )
  : ArgparserObjectType( fullname, spec, n )
{
}

ObjectLPType::~ObjectLPType()
{
}

ObjectImp* ObjectLPType::calc( const Args& targs, const KigDocument& ) const
{
  Args args = margsparser.parse( targs );
  if ( ! args[0] || ! args[1] ) return new InvalidImp;
  LineData l = static_cast<const AbstractLineImp*>( args[0] )->data();
  Coordinate c = static_cast<const PointImp*>( args[1] )->coordinate();
  return calc( l, c );
}

const Coordinate ObjectABType::moveReferencePoint( const RealObject* o ) const
{
  Objects parents = o->parents();
  assert( parents.size() == 2 );
  assert( parents[0]->hasimp( ObjectImp::ID_PointImp ) );
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}
