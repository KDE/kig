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
#include "../objects/object_imp.h"
#include "../objects/object_drawer.h"
#include "../objects/object_type_factory.h"
#include "../objects/object_imp_factory.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate_system.h"

#include "config.h"

#include <qfile.h>
#include <qregexp.h>
#include <qdom.h>
#include <kglobal.h>

#include <vector>
#include <algorithm>
#include <map>

struct HierElem
{
  int id;
  std::vector<int> parents;
  QDomElement el;
};

static void extendVect( std::vector<HierElem>& vect, uint size )
{
  if ( size > vect.size() )
  {
    int osize = vect.size();
    vect.resize( size );
    for ( uint i = osize; i < size; ++i )
      vect[i].id = i+1;
  };
}

static void visitElem( std::vector<HierElem>& ret,
                       const std::vector<HierElem>& elems,
                       std::vector<bool>& seen,
                       int i )
{
  if ( !seen[i] )
  {
    for ( uint j = 0; j < elems[i].parents.size(); ++j )
      visitElem( ret, elems, seen, elems[i].parents[j] - 1);
    ret.push_back( elems[i] );
    seen[i] = true;
  };
}

static std::vector<HierElem> sortElems( const std::vector<HierElem> elems )
{
  std::vector<HierElem> ret;
  std::vector<bool> seenElems( elems.size(), false );
  for ( uint i = 0; i < elems.size(); ++i )
    visitElem( ret, elems, seenElems, i );
  return ret;
}

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
  if ( major != 0 || minor > 7 )
  {
    notSupported( file, i18n( "This file was created by Kig version \"%1\", "
                              "which this version cannot open." ).arg( version ) );
    return false;
  }
  else if ( minor <= 3 )
  {
    notSupported( file, i18n( "This file was created by Kig version \"%1\". \n"
                              "Support for older Kig formats ( pre-0.4 ) has been "
                              "removed from Kig. \n"
                              "You can try to open this file with an older Kig "
                              "version ( 0.4 to 0.6 ), \n"
                              "and then save it again, which will save it in the "
                              "new format.." ).arg( version ) );
    return false;
  }
  else if ( minor <= 6 )
    return load04( file, main, to );
  else
    return load07( file, main, to );
}

bool KigFilterNative::save04( const KigDocument& kdoc, const QString& to )
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
  docelem.setAttribute( "Version", "0.6.0" );

  // save the coordinate system type..
  QDomElement cselem = doc.createElement( "CoordinateSystem" );
  cselem.appendChild( doc.createTextNode( kdoc.coordinateSystem().type() ) );
  docelem.appendChild( cselem );

  std::vector<ObjectHolder*> holders = kdoc.objects();
  std::vector<ObjectCalcer*> objs = getAllParents( getCalcers( kdoc.objects() ) );
  std::map<ObjectCalcer*, ObjectHolder*> holdermap;
  for ( std::vector<ObjectHolder*>::iterator i = holders.begin();
        i != holders.end(); ++i )
    holdermap[( *i )->calcer()] = *i;

  // save the objects..
  QDomElement objectselem = doc.createElement( "Objects" );
  objs = calcPath( objs );

  std::map<const ObjectCalcer*, int> idmap;
  int id = 1;

  for ( std::vector<ObjectCalcer*>::const_iterator i = objs.begin(); i != objs.end(); ++i )
  {
    bool internal = holdermap.find( *i ) == holdermap.end();
    QDomElement objectelem = doc.createElement( "tempname" );
    idmap[*i] = id;
    objectelem.setAttribute( "id", id++ );
    objectelem.setAttribute( "internal", QString::fromLatin1( internal ? "true" : "false" ) );
    if ( dynamic_cast<ObjectConstCalcer*>( *i ) )
    {
      objectelem.setTagName( "Data" );
      QString ser =
        ObjectImpFactory::instance()->serialize( *(*i)->imp(), objectelem, doc );
      objectelem.setAttribute( "type", ser );
    }
    else if ( dynamic_cast<const ObjectPropertyCalcer*>( *i ) )
    {
      const ObjectPropertyCalcer* o = static_cast<const ObjectPropertyCalcer*>( *i );
      QDomElement e = doc.createElement( "Property" );

      std::map<const ObjectCalcer*,int>::const_iterator idp = idmap.find( o->parent() );
      assert( idp != idmap.end() );
      int pid = idp->second;
      QDomElement pel = doc.createElement( "Parent" );
      pel.setAttribute( "id", pid );
      objectelem.appendChild( pel );

      QCString propname = o->parent()->imp()->propertiesInternalNames()[o->propId()];
      pel = doc.createElement( "Property" );
      pel.appendChild( doc.createTextNode( propname ) );
      objectelem.appendChild( pel );
    }
    else if ( dynamic_cast<const ObjectTypeCalcer*>( *i ) )
    {
      const ObjectTypeCalcer* o = static_cast<const ObjectTypeCalcer*>( *i );
      objectelem.setTagName( "Object" );
      objectelem.setAttribute( "type", o->type()->fullName() );

      const std::vector<ObjectCalcer*> parents = o->parents();
      for ( std::vector<ObjectCalcer*>::const_iterator i = parents.begin(); i != parents.end(); ++i )
      {
        std::map<const ObjectCalcer*,int>::const_iterator idp = idmap.find( *i );
        assert( idp != idmap.end() );
        int pid = idp->second;
        QDomElement pel = doc.createElement( "Parent" );
        pel.setAttribute( "id", pid );
        objectelem.appendChild( pel );
      }
    }
    else assert( false );

    if ( !internal )
    {
      // here we save the objectdrawer data as properties of the objectelem.
      // it would be better to make a new file format where the
      // objectcalcer hierarchy is completely separate from the holders
      // holding references to them, but I'm too lazy for that atm, so I'm doing it
      // this way..
      assert( holdermap.find( *i ) != holdermap.end() );
      const ObjectHolder* h = holdermap[*i];
      const ObjectDrawer* d = h->drawer();
      objectelem.setAttribute( "color", d->color().name() );
      objectelem.setAttribute( "shown", QString::fromLatin1( d->shown() ? "true" : "false" ) );
      objectelem.setAttribute( "width", QString::number( d->width() ) );
    };

    objectselem.appendChild( objectelem );
  };
  docelem.appendChild( objectselem );

  doc.appendChild( docelem );
  stream << doc.toCString();
  file.close();
  return true;
}

bool KigFilterNative::load04( const QString& file, const QDomElement& docelem, KigDocument& kdoc )
{
  bool ok = true;
  assert( kdoc.objects().empty() );

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
                       "that this Kig version does not support. \n"
                       "A standard coordinate system will be used "
                       "instead." ) );
      }
      else kdoc.setCoordinateSystem( s );
    }
    else if ( e.tagName() == "Objects" )
    {
      std::vector<ObjectCalcer*> retcalcers;
      std::vector<ObjectHolder*> retholders;

      // first pass: do a topological sort of the objects, to support
      // randomly ordered files...
      std::vector<HierElem> elems;
      QDomElement objectselem = e;
      for ( QDomNode o = objectselem.firstChild(); ! o.isNull(); o = o.nextSibling() )
      {
        e = o.toElement();
        if ( e.isNull() ) continue;
        uint id;
        if ( e.tagName() == "Data" || e.tagName() == "Property" || e.tagName() == "Object" )
        {
          // fetch the id
          QString tmp = e.attribute("id");
          id = tmp.toInt(&ok);
          if ( !ok ) KIG_FILTER_PARSE_ERROR;

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

      retcalcers.resize( elems.size(), 0 );

      for ( std::vector<HierElem>::iterator i = elems.begin();
            i != elems.end(); ++i )
      {
        QDomElement e = i->el;
        bool internal = e.attribute( "internal" ) == "true" ? true : false;
        ObjectCalcer* o = 0;
        if ( e.tagName() == "Data" )
        {
          QString tmp = e.attribute( "type" );
          if ( tmp.isNull() )
            KIG_FILTER_PARSE_ERROR;
          ObjectImp* imp = ObjectImpFactory::instance()->deserialize( tmp, e );
          if ( ! imp )
          {
            notSupported( file, i18n( "This Kig file uses an object of type \"%1\", "
                                      "which this Kig version does not support. \n"
                                      "Perhaps you have compiled Kig without support "
                                      "for this object type, \n"
                                      "or perhaps you are using an older Kig version..." ) );
            return false;
          };
          o = new ObjectConstCalcer( imp );
        }
        else if ( e.tagName() == "Property" )
        {
          QCString propname;
          for ( QDomElement ec = e.firstChild().toElement(); !ec.isNull();
                ec = ec.nextSibling().toElement() )
          {
            if ( ec.tagName() == "Property" )
              propname = ec.text().latin1();
          };

          if ( i->parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
          ObjectCalcer* parent = retcalcers[i->parents[0] -1];
          QCStringList propnames = parent->imp()->propertiesInternalNames();
          int propid = propnames.findIndex( propname );
          if ( propid == -1 )
            KIG_FILTER_PARSE_ERROR;

          o = new ObjectPropertyCalcer( parent, propid );
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
                                      "which this Kig version does not support. \n "
                                      "Perhaps you have compiled Kig without support "
                                      "for this object type, \n"
                                      "or perhaps you are using an older Kig version..." ) );
            return false;
          };

          std::vector<ObjectCalcer*> parents;
          for ( std::vector<int>::iterator j = i->parents.begin();
                j != i->parents.end(); ++j )
            parents.push_back( retcalcers[*j - 1] );

          o = new ObjectTypeCalcer( type, parents );
        }
        else continue;

        o->calc( kdoc );
        retcalcers[i->id - 1] = o;

        if ( ! internal )
        {
          QString tmp = e.attribute( "color" );
          QColor color( tmp );
          if ( !color.isValid() )
            KIG_FILTER_PARSE_ERROR;

          tmp = e.attribute( "shown" );
          bool shown = !( tmp == "false" || tmp == "no" );

          tmp = e.attribute( "width" );
          int width = tmp.toInt( &ok );
          if ( ! ok ) width = -1;

          ObjectDrawer* d = new ObjectDrawer( color, width, shown );
          retholders.push_back( new ObjectHolder( o, d ) );
        }
      }
      kdoc._addObjects( retholders );
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

bool KigFilterNative::load07( const QString& file, const QDomElement& docelem, KigDocument& kdoc )
{
  bool ok = true;
  assert( kdoc.objects().empty() );
  std::vector<ObjectCalcer::shared_ptr> calcers;
  std::vector<ObjectHolder*> holders;

  for ( QDomElement subsectionelement = docelem.firstChild().toElement(); ! subsectionelement.isNull();
        subsectionelement = subsectionelement.nextSibling().toElement() )
  {
    if ( subsectionelement.tagName() == "CoordinateSystem" )
    {
      const QCString type = subsectionelement.text().latin1();
      CoordinateSystem* s = CoordinateSystemFactory::build( type );
      if ( ! s )
      {
        warning( i18n( "This Kig file has a coordinate system "
                       "that this Kig version does not support. \n"
                       "A standard coordinate system will be used "
                       "instead." ) );
      }
      else kdoc.setCoordinateSystem( s );
    }
    else if ( subsectionelement.tagName() == "Hierarchy" )
    {
      for ( QDomElement e = subsectionelement.firstChild().toElement(); ! e.isNull();
            e = e.nextSibling().toElement() )
      {
        QString tmp = e.attribute( "id" );
        uint id = tmp.toInt( &ok );
        if ( id <= 0 ) KIG_FILTER_PARSE_ERROR;

        std::vector<ObjectCalcer*> parents;
        for ( QDomElement parentel = e.firstChild().toElement(); ! parentel.isNull();
              parentel = parentel.nextSibling().toElement() )
        {
          if ( parentel.tagName() != "Parent" ) continue;
          QString tmp = parentel.attribute( "id" );
          uint parentid = tmp.toInt( &ok );
          if ( ! ok ) KIG_FILTER_PARSE_ERROR;
          if ( parentid == 0 || parentid > calcers.size() ) KIG_FILTER_PARSE_ERROR;
          ObjectCalcer* parent = calcers[parentid - 1].get();
          if ( ! parent ) KIG_FILTER_PARSE_ERROR;
          parents.push_back( parent );
        }

        ObjectCalcer* o = 0;

        if ( e.tagName() == "Data" )
        {
          if ( !parents.empty() ) KIG_FILTER_PARSE_ERROR;
          QString tmp = e.attribute( "type" );
          ObjectImp* imp = ObjectImpFactory::instance()->deserialize( tmp, e );
          if ( ! imp )
          {
            notSupported( file, i18n( "This Kig file uses an object of type \"%1\", "
                                      "which this Kig version does not support. \n"
                                      "Perhaps you have compiled Kig without support "
                                      "for this object type, \n"
                                      "or perhaps you are using an older Kig version..." ) );
            return false;
          }
          o = new ObjectConstCalcer( imp );
        }
        else if ( e.tagName() == "Property" )
        {
          if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
          QCString propname = e.attribute( "which" ).latin1();

          ObjectCalcer* parent = parents[0];
          int propid = parent->imp()->propertiesInternalNames().findIndex( propname );
          if ( propid == -1 ) KIG_FILTER_PARSE_ERROR;

          o = new ObjectPropertyCalcer( parent, propid );
        }
        else if ( e.tagName() == "Object" )
        {
          QString tmp = e.attribute( "type" );
          const ObjectType* type =
            ObjectTypeFactory::instance()->find( tmp.latin1() );
          if ( ! type )
          {
            notSupported( file, i18n( "This Kig file uses an object of type \"%1\", "
                                      "which this Kig version does not support. \n "
                                      "Perhaps you have compiled Kig without support "
                                      "for this object type, \n"
                                      "or perhaps you are using an older Kig version..." ) );
            return false;
          }

          o = new ObjectTypeCalcer( type, parents );
        }
        else KIG_FILTER_PARSE_ERROR;

        o->calc( kdoc );
        calcers.resize( id, 0 );
        calcers[id-1] = o;
      }
    }
    else if ( subsectionelement.tagName() == "View" )
    {
      for ( QDomElement e = subsectionelement.firstChild().toElement(); ! e.isNull();
            e = e.nextSibling().toElement() )
      {
        if ( e.tagName() != "Draw" ) KIG_FILTER_PARSE_ERROR;

        QString tmp = e.attribute( "object" );
        uint id = tmp.toInt( &ok );
        if ( !ok ) KIG_FILTER_PARSE_ERROR;
        if ( id <= 0 || id > calcers.size() )
          KIG_FILTER_PARSE_ERROR;
        ObjectCalcer* calcer = calcers[id-1].get();

        tmp = e.attribute( "color" );
        QColor color( tmp );
        if ( !color.isValid() )
          KIG_FILTER_PARSE_ERROR;

        tmp = e.attribute( "shown" );
        bool shown = !( tmp == "false" || tmp == "no" );

        tmp = e.attribute( "width" );
        int width = tmp.toInt( &ok );
        if ( ! ok ) width = -1;

        ObjectDrawer* drawer = new ObjectDrawer( color, width, shown );
        holders.push_back( new ObjectHolder( calcer, drawer ) );
      }
    }
  }

  kdoc.setObjects( holders );
  return true;
}

bool KigFilterNative::save07( const KigDocument& kdoc, const QString& to )
{
  QFile file( to );
  if ( ! file.open( IO_WriteOnly ) )
  {
    fileNotFound( to );
    return false;
  }
  QTextStream stream( &file );
  QDomDocument doc( "KigDocument" );

  QDomElement docelem = doc.createElement( "KigDocument" );
  docelem.setAttribute( "Version", KIGVERSION );

  QDomElement cselem = doc.createElement( "CoordinateSystem" );
  cselem.appendChild( doc.createTextNode( kdoc.coordinateSystem().type() ) );
  docelem.appendChild( cselem );

  std::vector<ObjectHolder*> holders = kdoc.objects();
  std::vector<ObjectCalcer*> calcers = getAllParents( getCalcers( holders ) );
  calcers = calcPath( calcers );

  QDomElement hierelem = doc.createElement( "Hierarchy" );
  std::map<const ObjectCalcer*, int> idmap;
  for ( std::vector<ObjectCalcer*>::const_iterator i = calcers.begin();
        i != calcers.end(); ++i )
    idmap[*i] = ( i - calcers.begin() ) + 1;
  int id = 1;

  for ( std::vector<ObjectCalcer*>::const_iterator i = calcers.begin(); i != calcers.end(); ++i )
  {
    QDomElement objectelem;
    if ( dynamic_cast<ObjectConstCalcer*>( *i ) )
    {
      objectelem = doc.createElement( "Data" );
      QString ser =
        ObjectImpFactory::instance()->serialize( *(*i)->imp(), objectelem, doc );
      objectelem.setAttribute( "type", ser );
    }
    else if ( dynamic_cast<const ObjectPropertyCalcer*>( *i ) )
    {
      const ObjectPropertyCalcer* o = static_cast<const ObjectPropertyCalcer*>( *i );
      objectelem = doc.createElement( "Property" );

      QCString propname = o->parent()->imp()->propertiesInternalNames()[o->propId()];
      objectelem.setAttribute( "which", propname );
    }
    else if ( dynamic_cast<const ObjectTypeCalcer*>( *i ) )
    {
      const ObjectTypeCalcer* o = static_cast<const ObjectTypeCalcer*>( *i );
      objectelem = doc.createElement( "Object" );
      objectelem.setAttribute( "type", o->type()->fullName() );
    }
    else assert( false );

    const std::vector<ObjectCalcer*> parents = ( *i )->parents();
    for ( std::vector<ObjectCalcer*>::const_iterator i = parents.begin(); i != parents.end(); ++i )
    {
      std::map<const ObjectCalcer*,int>::const_iterator idp = idmap.find( *i );
      assert( idp != idmap.end() );
      int pid = idp->second;
      QDomElement pel = doc.createElement( "Parent" );
      pel.setAttribute( "id", pid );
      objectelem.appendChild( pel );
    }

    objectelem.setAttribute( "id", id++ );
    hierelem.appendChild( objectelem );
  }
  docelem.appendChild( hierelem );

  QDomElement windowelem = doc.createElement( "View" );
  for ( std::vector<ObjectHolder*>::iterator i = holders.begin(); i != holders.end(); ++i )
  {
    std::map<const ObjectCalcer*,int>::const_iterator idp = idmap.find( ( *i )->calcer() );
    assert( idp != idmap.end() );
    int id = idp->second;

    const ObjectDrawer* d = ( *i )->drawer();
    QDomElement drawelem = doc.createElement( "Draw" );
    drawelem.setAttribute( "object", id );
    drawelem.setAttribute( "color", d->color().name() );
    drawelem.setAttribute( "shown", QString::fromLatin1( d->shown() ? "true" : "false" ) );
    drawelem.setAttribute( "width", QString::number( d->width() ) );

    windowelem.appendChild( drawelem );
  };
  docelem.appendChild( windowelem );

  doc.appendChild( docelem );
  stream << doc.toCString();
  file.close();
  return true;
}

bool KigFilterNative::save( const KigDocument& data, const QString& file )
{
  return save07( data, file );
}
