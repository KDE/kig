// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef OBJECT_TYPE_FACTORY_H
#define OBJECT_TYPE_FACTORY_H

#include "common.h"

#include <map>
#include <string>

class ObjectTypeFactory
{
  typedef std::map<std::string, const ObjectType*> maptype;
  maptype mmap;
  ObjectTypeFactory();
  ~ObjectTypeFactory();
public:
  static ObjectTypeFactory* instance();
  void add( const ObjectType* type );
  const ObjectType* find( const char* name ) const;
};

#endif
