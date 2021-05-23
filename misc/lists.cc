// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "lists.h"

#include "object_constructor.h"
#include "guiaction.h"
#include "object_hierarchy.h"
#include "../kig/kig_part.h"
#include "kig_version.h"

#include <KMessageBox>
#include <QFile>
#include <QTextStream>
#include <qdom.h>
#include <QRegExp>
#include <algorithm>
#include <iterator>
using namespace std;

template<typename T>
void vect_remove( std::vector<T>& v, const T& t )
{
  typename std::vector<T>::iterator new_end = std::remove( v.begin(), v.end(), t );
  v.erase( new_end, v.end() );
}

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

void GUIActionList::regDoc( KigPart* d )
{
  mdocs.insert( d );
}

void GUIActionList::unregDoc( KigPart* d )
{
  mdocs.erase( d );
}

void GUIActionList::add( const std::vector<GUIAction*>& a )
{
  copy( a.begin(), a.end(), inserter( mactions, mactions.begin() ) );
  for ( dvectype::iterator i = mdocs.begin(); i != mdocs.end(); ++i )
  {
    KigPart::GUIUpdateToken t = (*i)->startGUIActionUpdate();
    for ( uint j = 0; j < a.size(); ++j )
      (*i)->actionAdded( a[j], t );
    (*i)->endGUIActionUpdate( t );
  };
}

void GUIActionList::add( GUIAction* a )
{
  mactions.insert( a );
  for ( dvectype::iterator i = mdocs.begin(); i != mdocs.end(); ++i )
  {
    KigPart::GUIUpdateToken t = (*i)->startGUIActionUpdate();
    (*i)->actionAdded( a, t );
    (*i)->endGUIActionUpdate( t );
  };
}

void GUIActionList::remove( const std::vector<GUIAction*>& a )
{
  for ( uint i = 0; i < a.size(); ++i )
  {
    mactions.erase( a[i] );
  };
  for ( dvectype::iterator i = mdocs.begin(); i != mdocs.end(); ++i )
  {
    KigPart::GUIUpdateToken t = (*i)->startGUIActionUpdate();
    for ( uint j = 0; j < a.size(); ++j )
      (*i)->actionRemoved( a[j], t );
    (*i)->endGUIActionUpdate( t );
  };
  delete_all( a.begin(), a.end() );
}

void GUIActionList::remove( GUIAction* a )
{
  mactions.erase( a );
  for ( dvectype::iterator i = mdocs.begin(); i != mdocs.end(); ++i )
  {
    KigPart::GUIUpdateToken t = (*i)->startGUIActionUpdate();
    (*i)->actionRemoved( a, t );
    (*i)->endGUIActionUpdate( t );
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
  const std::vector<ObjectCalcer*>& os, const KigDocument& d,
  const KigWidget& w, bool co ) const
{
  vectype ret;
  for ( vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i )
  {
    int r = (*i)->wantArgs( os, d, w );
    if ( r == ArgsParser::Complete || ( !co && r == ArgsParser::Valid ) )
      ret.push_back( *i );
  };
  return ret;
}

void ObjectConstructorList::remove( ObjectConstructor* a )
{
  vect_remove( mctors, a );
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

bool operator==( const Macro& l, const Macro& r )
{
  return ( l.action->descriptiveName() == r.action->descriptiveName() ) &&
         ( l.action->description() == r.action->description() ) &&
         ( l.action->iconFileName() == r.action->iconFileName() );
}

MacroList::MacroList()
{
}

MacroList::~MacroList()
{
  std::vector<GUIAction*> actions;
  std::vector<ObjectConstructor*> ctors;
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

void MacroList::add( const std::vector<Macro*>& ms )
{
  copy( ms.begin(), ms.end(), back_inserter( mdata ) );
  std::vector<GUIAction*> acts;
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
  mdata.erase( std::remove( mdata.begin(), mdata.end(), m ),
               mdata.end() );
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
  std::vector<Macro*> ms;
  ms.push_back( m );
  return save( ms, f );
}

bool MacroList::save( const std::vector<Macro*>& ms, const QString& f )
{
  QDomDocument doc( QStringLiteral("KigMacroFile") );

  QDomElement docelem = doc.createElement( QStringLiteral("KigMacroFile") );
  docelem.setAttribute( QStringLiteral("Version"), KIG_VERSION_STRING );
  docelem.setAttribute( QStringLiteral("Number"), static_cast<uint>( ms.size() ) );

  for ( uint i = 0; i < ms.size(); ++i )
  {
    MacroConstructor* ctor = ms[i]->ctor;

    QDomElement macroelem = doc.createElement( QStringLiteral("Macro") );

    // name
    QDomElement nameelem = doc.createElement( QStringLiteral("Name") );
    nameelem.appendChild( doc.createTextNode( ctor->descriptiveName() ) );
    macroelem.appendChild( nameelem );

    // desc
    QDomElement descelem = doc.createElement( QStringLiteral("Description") );
    descelem.appendChild( doc.createTextNode( ctor->description() ) );
    macroelem.appendChild( descelem );

    // icon
    QByteArray icon = ctor->iconFileName( true );
    if ( !icon.isNull() )
    {
      QDomElement descelem = doc.createElement( QStringLiteral("IconFileName") );
      descelem.appendChild( doc.createTextNode( icon ) );
      macroelem.appendChild( descelem );
    }

    // data
    QDomElement hierelem = doc.createElement( QStringLiteral("Construction") );
    ctor->hierarchy().serialize( hierelem, doc );
    macroelem.appendChild( hierelem );

    docelem.appendChild( macroelem );
  };

  doc.appendChild( docelem );

  QFile file( f );
  if ( ! file.open( QIODevice::WriteOnly ) )
    return false;
  QTextStream stream( &file );
  stream << doc.toByteArray();
  return true;
}

bool MacroList::load( const QString& f, std::vector<Macro*>& ret, const KigPart& kdoc )
{
  QFile file( f );
  if ( ! file.open( QIODevice::ReadOnly ) )
  {
    KMessageBox::sorry( 0, i18n( "Could not open macro file '%1'", f ) );
    return false;
  }
  QDomDocument doc( QStringLiteral("KigMacroFile") );
  if ( !doc.setContent( &file ) )
  {
    KMessageBox::sorry( 0, i18n( "Could not open macro file '%1'", f ) );
    return false;
  }
  file.close();
  QDomElement main = doc.documentElement();

  if ( main.tagName() == QLatin1String("KigMacroFile") )
    return loadNew( main, ret, kdoc );
  else
  {
    KMessageBox::detailedSorry(
      0, i18n( "Kig cannot open the macro file \"%1\".", f ),
      i18n( "This file was created by a very old Kig version (pre-0.4). "
            "Support for this format has been removed from recent Kig versions. "
            "You can try to import this macro using a previous Kig version "
            "(0.4 to 0.6) and then export it again in the new format." ),
      i18n( "Not Supported" ) );
    return false;
  }
}

bool MacroList::loadNew( const QDomElement& docelem, std::vector<Macro*>& ret, const KigPart& )
{
  bool sok = true;
  // unused..
//  int number = docelem.attribute( "Number" ).toInt( &sok );
  if ( ! sok ) return false;

  QString version = docelem.attribute( QStringLiteral("Version") );
//  QRegExp re( "(\\d+)\\.(\\d+)\\.(\\d+)" );
//  re.match( version );
  // unused..
//  int major = re.cap( 1 ).toInt( &sok );
//  int minor = re.cap( 2 ).toInt( &sok );
//  int mminor = re.cap( 3 ).toInt( &sok );
//  if ( ! sok ) return false;

  int unnamedindex = 1;
  QString tmp;

  for ( QDomElement macroelem = docelem.firstChild().toElement();
        ! macroelem.isNull(); macroelem = macroelem.nextSibling().toElement() )
  {
    QString name, description;
    ObjectHierarchy* hierarchy = 0;
    QByteArray actionname;
    QByteArray iconfile( "system-run" );
    if ( macroelem.tagName() != QLatin1String("Macro") ) continue; // forward compat ?
    for ( QDomElement dataelem = macroelem.firstChild().toElement();
          ! dataelem.isNull(); dataelem = dataelem.nextSibling().toElement() )
    {
      if ( dataelem.tagName() == QLatin1String("Name") )
        name = dataelem.text();
      else if ( dataelem.tagName() == QLatin1String("Description") )
        description = dataelem.text();
      else if ( dataelem.tagName() == QLatin1String("Construction") )
        hierarchy = ObjectHierarchy::buildSafeObjectHierarchy( dataelem, tmp );
      else if ( dataelem.tagName() == QLatin1String("ActionName") )
        actionname = dataelem.text().toLatin1();
      else if ( dataelem.tagName() == QLatin1String("IconFileName") )
        iconfile = dataelem.text().toLatin1();
      else continue;
    };
    assert( hierarchy );
    // if the macro has no name, we give it a bogus name...
    bool name_i18ned = false;
    if ( name.isEmpty() )
    {
      name = i18n( "Unnamed Macro #%1", unnamedindex++ );
      name_i18ned = true;
    }
    MacroConstructor* ctor =
      new MacroConstructor( *hierarchy, name_i18ned ? name : i18n( name.toUtf8() ),
                            description.isEmpty() ? QString() : i18n( description.toUtf8() ),
                            iconfile );
    delete hierarchy;
    GUIAction* act = new ConstructibleAction( ctor, actionname );
    Macro* macro = new Macro( act, ctor );
    ret.push_back( macro );
  };
  return true;
}

const ObjectConstructorList::vectype& ObjectConstructorList::constructors() const
{
  return mctors;
}
