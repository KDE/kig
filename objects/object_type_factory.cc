// object_type_factory.cc
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

#include "object_type_factory.h"

#include "object_type.h"
#include "line_type.h"
#include "point_type.h"
#include "circle_type.h"
#include "cubic_type.h"
#include "conic_types.h"

ObjectTypeFactory::ObjectTypeFactory()
  : malreadysetup( false )
{
  setupBuiltinTypes();
}

ObjectTypeFactory::~ObjectTypeFactory()
{
}

ObjectTypeFactory* ObjectTypeFactory::instance()
{
  static ObjectTypeFactory fact;
  return &fact;
}

void ObjectTypeFactory::add( const ObjectType* type )
{
  assert( mmap.find( std::string( type->fullName() ) ) == mmap.end() );
  mmap[std::string( type->fullName() )] = type;
}

const ObjectType* ObjectTypeFactory::find( const char* name ) const
{
  maptype::const_iterator i = mmap.find( std::string( name ) );
  if ( i == mmap.end() ) return 0;
  else return i->second;
}

void ObjectTypeFactory::setupBuiltinTypes()
{
  assert( ! malreadysetup );
  malreadysetup = true;
  // line_type.h
  add( new SegmentABType );
  add( new LineABType );
  add( new RayABType );
  add( new LinePerpendLPType );
  add( new LineParallelLPType );

  // point_type.h
  add( new FixedPointType );
  add( new ConstrainedPointType );

  // circle_type.h
  add( new CircleBCPType );
  add( new CircleBTPType );

  // conic_type.h
  add( new ConicB5PType );
  add( new ConicBAAPType );
  add( new EllipseBFFPType );
  add( new HyperbolaBFFPType );
  add( new ConicBDFPType );
  add( new ParabolaBTPType );
  add( new ConicPolarPointType );
  add( new ConicPolarLineType );

  // cubic_type.h
  add( new CubicB9PType );
  add( new CubicNodeB6PType );
}
