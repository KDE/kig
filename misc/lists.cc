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
#include "oldkigformat.h"
#include "object_hierarchy.h"
#include "../objects/object.h"
#include "../kig/kig_part.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qdom.h>
#include <qregexp.h>
#include <algorithm>
using namespace std;

GUIActionList* GUIActionList::instance()
{
  static GUIActionList l;
  return &l;
}

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

void GUIActionList::add( const myvector<GUIAction*>& a )
{
  copy( a.begin(), a.end(), back_inserter( mactions ) );
  for ( uint i = 0; i < mdocs.size(); ++i )
  {
    KigDocument::GUIUpdateToken t = mdocs[i]->startGUIActionUpdate();
    for ( uint j = 0; j < a.size(); ++j )
      mdocs[i]->actionAdded( a[j], t );
    mdocs[i]->endGUIActionUpdate( t );
  };
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

void GUIActionList::remove( const myvector<GUIAction*>& a )
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

Macro::Macro( GUIAction* a, MacroConstructor* c )
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

void MacroList::add( const myvector<Macro*>& ms )
{
  copy( ms.begin(), ms.end(), back_inserter( mdata ) );
  myvector<GUIAction*> acts;
  for ( uint i = 0; i < ms.size(); ++i )
  {
    ObjectConstructorList::instance()->add( ms[i]->ctor );
    acts.push_back( ms[i]->action );
  };
  GUIActionList::instance()->add( acts );
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

const MacroList::vectype& MacroList::macros() const
{
  return mdata;
}

Macro::~Macro()
{
}

bool MacroList::save( Macro* m, const QString& f )
{
  return save( myvector<Macro*>( m ), f );
}

bool MacroList::save( const myvector<Macro*>& ms, const QString& f )
{
  QDomDocument doc( "KigMacroFile" );

  QDomElement docelem = doc.createElement( "KigMacroFile" );
  docelem.setAttribute( "Version", "0.5.1" );
  docelem.setAttribute( "Number", ms.size() );

  for ( uint i = 0; i < ms.size(); ++i )
  {
    MacroConstructor* ctor = ms[i]->ctor;

    QDomElement macroelem = doc.createElement( "Macro" );

    // name
    QDomElement nameelem = doc.createElement( "Name" );
    nameelem.appendChild( doc.createTextNode( ctor->descriptiveName() ) );
    macroelem.appendChild( nameelem );

    // desc
    QDomElement descelem = doc.createElement( "Description" );
    descelem.appendChild( doc.createTextNode( ctor->description() ) );
    macroelem.appendChild( descelem );

    // data
    QDomElement hierelem = doc.createElement( "Construction" );
    ctor->hierarchy().serialize( hierelem, doc );
    macroelem.appendChild( hierelem );

    docelem.appendChild( macroelem );
  };

  doc.appendChild( docelem );

  QFile file( f );
  if ( ! file.open( IO_WriteOnly ) )
    return false;
  QTextStream stream( &file );
  stream << doc.toCString();
  return true;
}

bool MacroList::load( const QString& f, myvector<Macro*>& ret, const KigDocument& kdoc )
{
  QFile file( f );
  if ( ! file.open( IO_ReadOnly ) )
    return false;
  QDomDocument doc( "KigMacroFile" );
  if ( !doc.setContent( &file ) )
    return false;
  file.close();
  QDomElement main = doc.documentElement();

  if ( main.tagName() == "KigMacroFile" )
    return loadNew( main, ret, kdoc );
  else
    return loadOld( main, ret, kdoc );
}

bool MacroList::loadNew( const QDomElement& docelem, myvector<Macro*>& ret, const KigDocument& )
{
  bool sok = true;
  // unused..
//  int number = docelem.attribute( "Number" ).toInt( &sok );
  if ( ! sok ) return false;

  QString version = docelem.attribute( "Version" );
//  QRegExp re( "(\\d+)\\.(\\d+)\\.(\\d+)" );
//  re.match( version );
  // unused..
//  int major = re.cap( 1 ).toInt( &sok );
//  int minor = re.cap( 2 ).toInt( &sok );
//  int mminor = re.cap( 3 ).toInt( &sok );
//  if ( ! sok ) return false;

  for ( QDomElement macroelem = docelem.firstChild().toElement();
        ! macroelem.isNull(); macroelem = macroelem.nextSibling().toElement() )
  {
    QString name, description;
    ObjectHierarchy* hierarchy = 0;
    QCString actionname, iconfile;
    if ( macroelem.tagName() != "Macro" ) continue; // forward compat ?
    for ( QDomElement dataelem = macroelem.firstChild().toElement();
          ! dataelem.isNull(); dataelem = dataelem.nextSibling().toElement() )
    {
      if ( dataelem.tagName() == "Name" )
        name = dataelem.text();
      else if ( dataelem.tagName() == "Description" )
        description = dataelem.text();
      else if ( dataelem.tagName() == "Construction" )
        hierarchy = new ObjectHierarchy( dataelem );
      else if ( dataelem.tagName() == "ActionName" )
        actionname = dataelem.text().latin1();
      else if ( dataelem.tagName() == "IconFileName" )
        iconfile = dataelem.text().latin1();
      else continue;
    };
    assert( hierarchy );
    MacroConstructor* ctor =
      new MacroConstructor( *hierarchy, name, description, iconfile );
    delete hierarchy;
    GUIAction* act = new ConstructibleAction( ctor, actionname );
    Macro* macro = new Macro( act, ctor );
    ret.push_back( macro );
  };
  return true;
}

bool MacroList::loadOld( const QDomElement& docelem, myvector<Macro*>& ret, const KigDocument& doc )
{
  assert( docelem.tagName() == "Types" );
  // loop over all macro's in the file
  for ( QDomElement mtypeel = docelem.firstChild().toElement();
        ! mtypeel.isNull(); mtypeel = mtypeel.nextSibling().toElement() )
  {
    assert( mtypeel.tagName() == "MType" );
    QString name = mtypeel.attribute( "name" );
    QDomElement hierel = mtypeel.firstChild().toElement();
    assert( hierel.tagName() == "ObjectHierarchy" );

    // what we do here is: we fill up all the "given" objects with
    // pseudo-objects generated by the randomObjectForType() function,
    // then construct their child object using the rest of the old
    // hierarchy, and then construct a new hierarchy using the
    // generated objects.  This allows us to reuse the code from the
    // import filter, which doesn't build a hierarchy, but generates
    // the objects directly.  Rebuilding the hierarchy also allows us
    // to make better use of the new style ObjectHierarchy features
    // like the use of impRequirement to figure out what type
    // requirements a macro has..

    Objects given;

    QDomElement e = hierel.firstChild().toElement();
    for ( ; ! e.isNull(); e = e.nextSibling().toElement() )
    {
      assert( e.tagName() == "HierarchyElement" );
      if ( e.attribute( "given" ) != "true" )
        break;
      QString tmp = e.attribute("typeName");
      if(tmp.isNull()) return false;
      QCString type = tmp.latin1();

      Object* r = randomObjectForType( type );
      r->parents().calc( doc );
      r->calc( doc );
      given.push_back( r );
    };

    assert( e.attribute( "given" ) != "true" );
    ReferenceObject ref;
    Objects final;

    if ( !parseOldObjectHierarchyElements( e, given, ref,
                                           final, doc ) ) return false;

    assert( !final.empty() );

    ObjectHierarchy hierarchy( given, final.front() );

    MacroConstructor* ctor =
      new MacroConstructor( hierarchy, name, QString::null );
    GUIAction* act = new ConstructibleAction( ctor, 0 );
    Macro* macro = new Macro( act, ctor );
    ret.push_back( macro );
  };
  return true;
}

const ObjectConstructorList::vectype& ObjectConstructorList::constructors() const
{
  return mctors;
}
