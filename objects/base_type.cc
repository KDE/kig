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
  : BuiltinType( fulltypename, spec, n )
{
}

ObjectABType::~ObjectABType()
{
}

ObjectImp* ObjectABType::calc( const Args& parents ) const
{
  if( parents.size() != 2 ) return new InvalidImp;
  Coordinate a = static_cast<const PointImp*>( parents[0] )->coordinate();
  Coordinate b = static_cast<const PointImp*>( parents[1] )->coordinate();
  return calc( a, b );
}

bool ObjectABType::canMove() const
{
  return true;
}

void ObjectABType::move( Object* o,
                         const Coordinate& from,
                         const Coordinate& dist ) const
{
  Objects parents = o->parents();
  assert( parents.size() == 2 );
  assert( parents[0]->hasimp( ObjectImp::ID_PointImp ) );
  assert( parents[1]->hasimp( ObjectImp::ID_PointImp ) );

  parents[0]->move( from, dist );
  parents[1]->move( from, dist );
}

ObjectLPType::ObjectLPType( const char* fullname, const ArgParser::spec* spec, int n )
  : BuiltinType( fullname, spec, n )
{
}

ObjectLPType::~ObjectLPType()
{
}

ObjectImp* ObjectLPType::calc( const Args& targs ) const
{
  if( targs.size() != 2 ) return new InvalidImp;
  Args args = margsparser.parse( targs );
  LineData l = static_cast<const AbstractLineImp*>( args[0] )->data();
  Coordinate c = static_cast<const PointImp*>( args[1] )->coordinate();
  return calc( l, c );
}
