// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "object_type_factory.h"

#include <config-kig.h>

#include "object_type.h"
#include "circle_type.h"
#include "conic_types.h"
#include "cubic_type.h"
#include "intersection_types.h"
#include "line_type.h"
#include "text_type.h"
#include "other_type.h"
#include "transform_types.h"
#include "point_type.h"
#include "tests_type.h"

#ifdef KIG_ENABLE_PYTHON_SCRIPTING
#include "../scripting/python_type.h"
#endif

#include <qdom.h>
#include <string>

ObjectTypeFactory::ObjectTypeFactory()
{
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


