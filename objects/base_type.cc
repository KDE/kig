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
#include "bogus_imp.h"
#include "object.h"

const struct ArgParser::spec ObjectABType::argsspec[] =
{
  { ObjectImp::ID_PointImp, 2 }
};

ObjectABType::ObjectABType( const char* basetypename, const char* fulltypename )
  : ObjectType( basetypename, fulltypename, argsspec, 1 )
{
}

ObjectABType::~ObjectABType()
{
}

ObjectImp* ObjectABType::calc( const Args& parents, const KigWidget& ) const
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
  assert( parents[0]->has( ObjectImp::ID_PointImp ) );
  assert( parents[1]->has( ObjectImp::ID_PointImp ) );

  parents[0]->move( from, dist );
  parents[1]->move( from, dist );
}

const struct ArgParser::spec ObjectABCType::argsspec[] =
{
  { ObjectImp::ID_PointImp, 3 }
};

ObjectABCType::ObjectABCType( const char* basetypename, const char* fulltypename )
  : ObjectType( basetypename, fulltypename, argsspec, 1 )
{
}

ObjectABCType::~ObjectABCType()
{
}
