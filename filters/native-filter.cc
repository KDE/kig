// filter.cc
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

#include "native-filter.h"

#include "../kig/kig_part.h"
#include "../objects/object_type.h"
#include "../objects/object_type_factory.h"
#include "../objects/object.h"
#include "../objects/object_imp_factory.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate_system.h"
#include "../misc/oldkigformat.h"

#include <qfile.h>
#include <qregexp.h>
#include <qdom.h>
#include <kglobal.h>

#include <vector>
#include <algorithm>
#include <map>

KigFilterNative::KigFilterNative()
{
}

KigFilterNative::~KigFilterNative()
{
}

bool KigFilterNative::supportMime( const QString& mime )
{
  return mime == "application/x-kig";
}

bool KigFilterNative::load( const QString& file, KigDocument& to )
{
  QFile ffile( file );
  if ( ! ffile.open( IO_ReadOnly ) )
  {
    fileNotFound( file );
    return false;
  };
  QDomDocument doc( "KigDocument" );
  if ( !doc.setContent( &ffile ) )
    KIG_FILTER_PARSE_ERROR;
  ffile.close();
  QDomElement main = doc.documentElement();

  QString version = main.attribute( "Version" );
  if ( ! version ) version = main.attribute( "version" );
  if ( ! version )
    KIG_FILTER_PARSE_ERROR;

  // matches 0.1, 0.2.0, 153.128.99 etc.
  QRegExp versionre( "(\\d+)\\.(\\d+)(\\.(\\d+))?" );
  if ( ! versionre.exactMatch( version ) )
    KIG_FILTER_PARSE_ERROR;
  bool ok = true;
  int major = versionre.cap( 1 ).toInt( &ok );
  bool ok2 = true;
  int minor = versionre.cap( 2 ).toInt( &ok );
  if ( ! ok || ! ok2 )
    KIG_FILTER_PARSE_ERROR;

  //   int minorminor = versionre.cap( 4 ).toInt( &ok );

  // we only support 0.* ( for now ? :)
  if ( major != 0 || minor > 5 )
  {
    notSupported( file, i18n( "This file was created by Kig version \"%1\", "
                              "which this version cannot open." ) );
    return false;
  }
  if ( minor > 3 )
    return loadNew( file, main, to );
  else
    return loadOld( file, main, to );
}

bool KigFilterNative::loadOld( const QString& file, const QDomElement& main, KigDocument& to )
{
  using namespace std;

  QDomElement hier = main.firstChild().toElement();
  if ( hier.isNull() || hier.tagName() != "ObjectHierarchy" )
  {
    parseError( file, i18n( "This kig file contains errors." ) );
    return false;
  }

  Objects os;
  Objects final;
  if ( ! parseOldObjectHierarchyElements( hier.firstChild().toElement(), os, final, to ) )
  {
    parseError( file, i18n( "This kig file contains errors." ) );
    return false;
  };
  // stop gcc from complaining about our not using this..
  final.clear();

  to.setObjects( os );
  return true;
}

bool KigFilterNative::save( const KigDocument& kdoc, const QString& to )
{
  using namespace std;

  QFile file( to );
  if ( ! file.open( IO_WriteOnly ) )
  {
    fileNotFound( to );
    return false;
  }
  QTextStream stream( &file );
  QDomDocument doc( "KigDocument" );

  // TODO ?
//  doc.appendChild( QDomImplementation().createDocumentType( ... ) );

  QDomElement docelem = doc.createElement( "KigDocument" );
  docelem.setAttribute( "Version", "0.5.1" );

  // save the coordinate system type..
  QDomElement cselem = doc.createElement( "CoordinateSystem" );
  cselem.appendChild( doc.createTextNode( kdoc.coordinateSystem().type() ) );
  docelem.appendChild( cselem );

  Objects objs = kdoc.objects();
  uint osize = objs.size();
  objs = getAllParents( objs );
  assert( objs.size() == osize );
  // pretend to use this var..
  (void)osize;

  // save the objects..
  QDomElement objectselem = doc.createElement( "Objects" );
  objs = calcPath( objs );

  std::map<const Object*, int> idmap;
  int id = 1;

  for ( Objects::const_iterator i = objs.begin(); i != objs.end(); ++i )
  {
    if ( (*i)->inherits( Object::ID_DataObject ) )
    {
      QDomElement e = doc.createElement( "Data" );
      idmap[*i] = id;
      e.setAttribute( "id", id++ );
      QString ser =
        ObjectImpFactory::instance()->serialize( *(*i)->imp(), e, doc );
      e.setAttribute( "type", ser );
      objectselem.appendChild( e );
    }
    else if ( (*i)->inherits( Object::ID_PropertyObject ) )
    {
      const PropertyObject* o = static_cast<const PropertyObject*>( *i );
      QDomElement e = doc.createElement( "Property" );
      idmap[*i] = id;
      e.setAttribute( "id", id++ );

      std::map<const Object*,int>::const_iterator idp = idmap.find( o->parent() );
      assert( idp != idmap.end() );
      int pid = idp->second;
      QDomElement pel = doc.createElement( "Parent" );
      pel.setAttribute( "id", pid );
      e.appendChild( pel );

      QCString propname = o->parent()->propertiesInternalNames()[o->propId()];
      pel = doc.createElement( "Property" );
      pel.appendChild( doc.createTextNode( propname ) );
      e.appendChild( pel );
      objectselem.appendChild( e );
    }
    else if ( (*i)->inherits( Object::ID_RealObject ) )
    {
      const RealObject* o = static_cast<const RealObject*>( *i );
      QDomElement e = doc.createElement( "Object" );
      idmap[*i] = id;
      e.setAttribute( "id", id++ );

      e.setAttribute( "type", o->type()->fullName() );

      e.setAttribute( "color", o->color().name() );
      e.setAttribute( "shown", QString::fromLatin1( o->shown() ? "true" : "false" ) );
      e.setAttribute( "width", QString::number( o->width() ) );

      const Objects& parents = o->parents();
      for ( Objects::const_iterator i = parents.begin(); i != parents.end(); ++i )
      {
        std::map<const Object*,int>::const_iterator idp = idmap.find( *i );
        assert( idp != idmap.end() );
        int pid = idp->second;
        QDomElement pel = doc.createElement( "Parent" );
        pel.setAttribute( "id", pid );
        e.appendChild( pel );
      };
      objectselem.appendChild( e );
    }
    else assert( false );
  };
  docelem.appendChild( objectselem );

  doc.appendChild( docelem );
  stream << doc.toCString();
  file.close();
  return true;
}

bool KigFilterNative::loadNew( const QString& file, const QDomElement& docelem, KigDocument& kdoc )
{
  bool ok = true;

  for ( QDomNode n = docelem.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    QDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    if ( e.tagName() == "CoordinateSystem" )
    {
      const QCString type = e.text().latin1();
      CoordinateSystem* s = CoordinateSystemFactory::build( type );
      if ( ! s )
      {
        warning( i18n( "This Kig file has a coordinate system "
                       "that this Kig version does not support. "
                       "A standard coordinate system will be used "
                       "instead." ) );
      }
      else kdoc.setCoordinateSystem( s );
    }
    else if ( e.tagName() == "Objects" )
    {
      Objects ret = kdoc.objects();
      // first pass: do a topological sort of the objects, to support
      // randomly ordered files...
      std::vector<HierElem> elems;
      QDomElement objectselem = e;
      for ( QDomNode o = objectselem.firstChild(); ! o.isNull(); o = o.nextSibling() )
      {
        e = o.toElement();
        if ( e.isNull() ) continue;
        uint id;
        if ( e.tagName() == "Data" || e.tagName() == "Property" )
        {
          // fetch the id
          QString tmp = e.attribute("id");
          id = tmp.toInt(&ok);
          if ( !ok ) KIG_FILTER_PARSE_ERROR;

          extendVect( elems, id );
          elems[id-1].el = e;
        }
        else if ( e.tagName() == "Object" )
        {
          QString tmp = e.attribute( "id" );
          id = tmp.toInt( &ok );
          if ( ! ok ) KIG_FILTER_PARSE_ERROR;

          extendVect( elems, id );
          elems[id-1].el = e;
        }
        else continue;
        for ( QDomNode p = e.firstChild(); !p.isNull(); p = p.nextSibling() )
        {
          QDomElement f = p.toElement();
          if ( f.isNull() ) continue;
          if ( f.tagName() == "Parent" )
          {
            QString tmp = f.attribute( "id" );
            uint pid = tmp.toInt( &ok );
            if ( ! ok ) KIG_FILTER_PARSE_ERROR;

            extendVect( elems, id );
            elems[id-1].parents.push_back( pid );
          }
        }
      };

      for ( uint i = 0; i < elems.size(); ++i )
        if ( elems[i].el.isNull() )
          KIG_FILTER_PARSE_ERROR;
      elems = sortElems( elems );

      uint oldsize = ret.size();
      ret.resize( oldsize + elems.size(), 0 );

      for ( std::vector<HierElem>::iterator i = elems.begin();
            i != elems.end(); ++i )
      {
        QDomElement e = i->el;
        if ( e.tagName() == "Data" )
        {
          QString tmp = e.attribute( "type" );
          if ( tmp.isNull() )
            KIG_FILTER_PARSE_ERROR;
          ObjectImp* imp = ObjectImpFactory::instance()->deserialize( tmp, e );
          if ( ! imp )
          {
            notSupported( file, i18n( "This Kig file uses an object of type \"%1\", "
                                      "which this Kig version does not support.  "
                                      "Perhaps you have compiled Kig without support "
                                      "for this object type, or perhaps you are using "
                                      "an older Kig version..." ) );
            return false;
          };
          ret[oldsize + i->id - 1] = new DataObject( imp );
        }
        else if ( e.tagName() == "Property" )
        {
          QCString propname;
          for ( QDomElement ec = e.firstChild().toElement(); !ec.isNull();
                ec = ec.nextSibling().toElement() )
          {
            if ( ec.tagName() == "Property" )
              propname = ec.text().latin1();
            else continue;
          };

          if ( i->parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
          Object* parent = ret[oldsize + i->parents[0] -1];
          QCStringList propnames = parent->propertiesInternalNames();
          int propid = propnames.findIndex( propname );
          if ( propid == -1 )
            KIG_FILTER_PARSE_ERROR;

          ret[oldsize + i->id - 1] = new PropertyObject( parent, propid );
        }
        else if ( e.tagName() == "Object" )
        {
          QString tmp = e.attribute( "type" );
          if ( tmp.isNull() )
            KIG_FILTER_PARSE_ERROR;

          const ObjectType* type =
            ObjectTypeFactory::instance()->find( tmp.latin1() );
          if ( !type )
          {
            notSupported( file, i18n( "This Kig file uses an object of type \"%1\", "
                                      "which this Kig version does not support.  "
                                      "Perhaps you have compiled Kig without support "
                                      "for this object type, or perhaps you are using "
                                      "an older Kig version..." ) );
            return false;
          };

          tmp = e.attribute( "color" );
          QColor color( tmp );
          if ( !color.isValid() )
            KIG_FILTER_PARSE_ERROR;

          tmp = e.attribute( "shown" );
          bool shown = !( tmp == "false" || tmp == "no" );

          tmp = e.attribute( "width" );
          int width = tmp.toInt( &ok );
          if ( ! ok ) width = -1;

          Objects parents;
          for ( std::vector<int>::iterator j = i->parents.begin();
                j != i->parents.end(); ++j )
            parents.push_back( ret[oldsize + *j - 1] );

          RealObject* newobj = new RealObject( type, parents );
          newobj->setColor( color );
          newobj->setShown( shown );
          newobj->setWidth( width );
          ret[oldsize + i->id - 1] = newobj;
        }
        else continue;

        // property objects require their parents to be calced..
        ret[oldsize + i->id - 1]->calc( kdoc );
      }
      kdoc.setObjects( ret );
    }
    else continue; // be forward-compatible..
  };

  return true;
}

KigFilterNative* KigFilterNative::instance()
{
  static KigFilterNative f;
  return &f;
}
