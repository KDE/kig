// object_constructor_list.h
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

#ifndef KIG_MISC_OBJECT_CONSTRUCTOR_LIST_H
#define KIG_MISC_OBJECT_CONSTRUCTOR_LIST_H

#include <vector>

class ObjectConstructor;
class Objects;
class KigDocument;
class KigWidget;

class ObjectConstructorList
{
public:
  typedef std::vector<ObjectConstructor*> vectype;
private:
  vectype mctors;
  ObjectConstructorList();
  ~ObjectConstructorList();
  void setupBuiltinTypes();
public:
  static ObjectConstructorList* instance();
  void add( ObjectConstructor* a );
  vectype ctorsThatWantArgs( const Objects&, const KigDocument&,
                             const KigWidget&, bool completeOnly = false
    ) const;
};

#endif
