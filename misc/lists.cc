// lists.cc
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

#include "lists.h"

#include "object_constructor.h"
#include "i18n.h"
#include "guiaction.h"
#include "../kig/kig_part.h"

GUIActionList* GUIActionList::instance()
{
  static GUIActionList l;
  return &l;
};

GUIActionList::~GUIActionList()
{
  for ( avectype::iterator i = mactions.begin(); i != mactions.end(); ++i )
    delete *i;
}

GUIActionList::GUIActionList()
{
}

void GUIActionList::regDoc( KigDocument* d )
{
  mdocs.push_back( d );
}

void GUIActionList::unregDoc( KigDocument* d )
{
  mdocs.remove( d );
}

void GUIActionList::add( GUIAction* a )
{
  mactions.push_back( a );
  for ( uint i = 0; i < mdocs.size(); ++i )
  {
    KigDocument::GUIUpdateToken t = mdocs[i]->startGUIActionUpdate();
    mdocs[i]->actionAdded( a, t );
    mdocs[i]->endGUIActionUpdate( t );
  };
}

void GUIActionList::remove( myvector<GUIAction*> a )
{
  for ( uint i = 0; i < a.size(); ++i )
  {
    mactions.remove( a[i] );
  };
  for ( uint i = 0; i < mdocs.size(); ++i )
  {
    KigDocument::GUIUpdateToken t = mdocs[i]->startGUIActionUpdate();
    for ( uint j = 0; j < a.size(); ++j )
      mdocs[i]->actionRemoved( a[j], t );
    mdocs[i]->endGUIActionUpdate( t );
  };
  delete_all( a.begin(), a.end() );
}

void GUIActionList::remove( GUIAction* a )
{
  mactions.remove( a );
  for ( uint i = 0; i < mdocs.size(); ++i )
  {
    KigDocument::GUIUpdateToken t = mdocs[i]->startGUIActionUpdate();
    mdocs[i]->actionRemoved( a, t );
    mdocs[i]->endGUIActionUpdate( t );
  };
  delete a;
}

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

void ObjectConstructorList::remove( ObjectConstructor* a )
{
  mctors.remove( a );
  delete a;
}

void ObjectConstructorList::add( ObjectConstructor* a )
{
  mctors.push_back( a );
}

Macro::Macro( GUIAction* a, ObjectConstructor* c )
  : action( a ), ctor( c )
{
}

MacroList::MacroList()
{
}

MacroList::~MacroList()
{
  myvector<GUIAction*> actions;
  myvector<ObjectConstructor*> ctors;
  for ( vectype::iterator i = mdata.begin(); i != mdata.end(); ++i )
  {
    Macro* m = *i;
    GUIAction* a = m->action;
    actions.push_back( a );
    ObjectConstructor* c = m->ctor;
    ctors.push_back( c );
    delete m;
  };
  mdata.clear();
  GUIActionList::instance()->remove( actions );
  for ( uint i = 0; i < ctors.size(); ++i )
    ObjectConstructorList::instance()->remove( ctors[i] );
}

MacroList* MacroList::instance()
{
  static MacroList t;
  return &t;
}

void MacroList::add( Macro* m )
{
  mdata.push_back( m );
  ObjectConstructorList::instance()->add( m->ctor );
  GUIActionList::instance()->add( m->action );
}

void MacroList::remove( Macro* m )
{
  GUIAction* a = m->action;
  ObjectConstructor* c = m->ctor;
  mdata.remove( m );
  delete m;
  GUIActionList::instance()->remove( a );
  ObjectConstructorList::instance()->remove( c );
}

