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
  for ( maptype::iterator i = mmap.begin(); i != mmap.end(); ++i )
    delete i->second;
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
  add( SegmentABType::instance() );
  add( LineABType::instance() );
  add( RayABType::instance() );
  add( LinePerpendLPType::instance() );
  add( LineParallelLPType::instance() );

  // point_type.h
  add( FixedPointType::instance() );
  add( ConstrainedPointType::instance() );

  // circle_type.h
  add( CircleBCPType::instance() );
  add( CircleBTPType::instance() );

  // conic_type.h
  add( ConicB5PType::instance() );
  add( ConicBAAPType::instance() );
  add( EllipseBFFPType::instance() );
  add( HyperbolaBFFPType::instance() );
  add( ConicBDFPType::instance() );
  add( ParabolaBTPType::instance() );
  add( ConicPolarPointType::instance() );
  add( ConicPolarLineType::instance() );

  // cubic_type.h
  add( CubicB9PType::instance() );
  add( CubicNodeB6PType::instance() );
}
