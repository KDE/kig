// object_constructor_list.cc
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

#include "object_constructor_list.h"

#include "object_constructor.h"
#include "i18n.h"

ObjectConstructorList::ObjectConstructorList()
{
}

ObjectConstructorList::~ObjectConstructorList()
{
  for ( vectype::iterator i = mctors.begin(); i != mctors.end(); ++i )
    delete *i;
}

ObjectConstructorList* ObjectConstructorList::instance()
{
  static ObjectConstructorList s;
  return &s;
}

ObjectConstructorList::vectype ObjectConstructorList::ctorsThatWantArgs(
  const Objects& os, const KigDocument& d,
  const KigWidget& w, bool co ) const
{
  vectype ret;
  for ( vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i )
  {
    int r = (*i)->wantArgs( os, d, w );
    if ( r == ArgsChecker::Complete || ( !co && r == ArgsChecker::Valid ) )
      ret.push_back( *i );
  };
  return ret;
}

void ObjectConstructorList::add( ObjectConstructor* a )
{
  mctors.push_back( a );
}
