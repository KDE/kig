// object_type_factory.h
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

#ifndef OBJECT_TYPE_FACTORY_H
#define OBJECT_TYPE_FACTORY_H

#include "common.h"

#include <map>
#include <string>

class ObjectTypeFactory
{
  typedef std::map<std::string, const ObjectType*> maptype;
  maptype mmap;
  bool malreadysetup;
  ObjectTypeFactory();
  ~ObjectTypeFactory();
  void setupBuiltinTypes();
public:
  static ObjectTypeFactory* instance();
  void add( const ObjectType* type );
  const ObjectType* find( const char* name ) const;
};

#endif
