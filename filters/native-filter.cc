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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "native-filter.h"

#include "../kig/kig_part.h"
#include "../kig/kig_document.h"
#include "../objects/bogus_imp.h"
#include "../objects/object_type.h"
#include "../objects/object_imp.h"
#include "../objects/object_calcer.h"
#include "../objects/object_drawer.h"
#include "../objects/object_holder.h"
#include "../objects/object_type_factory.h"
#include "../objects/object_imp_factory.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate_system.h"

#include "config.h"

#include <qdom.h>
#include <qfile.h>
#include <qregexp.h>

#include <karchive.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <ktar.h>

#include <stdio.h>

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

KigDocument* KigFilterNative::load( const QString& file )
{
  QFile ffile( file );
  if ( ! ffile.open( IO_ReadOnly ) )
  {
    fileNotFound( file );
    return 0;
  };

  QFile kigdoc( file );
#ifndef KIG_NO_COMPRESSED_FILES
  bool iscompressed = false;
  if ( !file.endsWith( ".kig", false ) )
  {
    // the file is compressed, so we have to decompress it and fetch the
    // kig file inside it...
    iscompressed = true;

    QString tempdir = KGlobal::dirs()->saveLocation( "tmp" );
    if ( tempdir.isEmpty() )
      KIG_FILTER_PARSE_ERROR;

    QString tempname = file.section( '/', -1 );
    if ( file.endsWith( ".kigz", false ) )
    {
      tempname.remove( QRegExp( "\\.[Kk][Ii][Gg][Zz]$" ) );
    }
    else
      KIG_FILTER_PARSE_ERROR;
    // reading compressed file
    KTar* ark = new KTar( file, "application/x-gzip" );
    ark->open( IO_ReadOnly );
    const KArchiveDirectory* dir = ark->directory();
//    assert( dir );
    QStringList entries = dir->entries();
    QStringList kigfiles = entries.grep( QRegExp( "\\.kig$" ) );
    if ( kigfiles.count() != 1 )
      // I throw a generic parse error here, but I should warn the user that
      // this kig archive file doesn't contain one kig file (it contains no
      // kig files or more than one).
      KIG_FILTER_PARSE_ERROR;
    const KArchiveEntry* kigz = dir->entry( kigfiles[0] );
    if ( !kigz->isFile() )
      KIG_FILTER_PARSE_ERROR;
    dynamic_cast<const KArchiveFile*>( kigz )->copyTo( tempdir );
    kdDebug() << "extracted file: " << tempdir + kigz->name() << endl
              << "exists: " << QFile::exists( tempdir + kigz->name() ) << endl;

    kigdoc.setName( tempdir + kigz->name() );
  }
#endif

  if ( !kigdoc.open( IO_ReadOnly ) )
    KIG_FILTER_PARSE_ERROR;

  QDomDocument doc( "KigDocument" );
  if ( !doc.setContent( &kigdoc ) )
    KIG_FILTER_PARSE_ERROR;
  kigdoc.close();

#ifndef KIG_NO_COMPRESSED_FILES
  // removing temp file
  if ( iscompressed )
    kigdoc.remove();
#endif

  QDomElement main = doc.documentElement();

  QString version = main.attribute( "CompatibilityVersion" );
  if ( version.isEmpty() ) version = main.attribute( "Version" );
  if ( version.isEmpty() ) version = main.attribute( "version" );
  if ( version.isEmpty() )
    KIG_FILTER_PARSE_ERROR;

  // matches 0.1, 0.2.0, 153.128.99 etc.
  QRegExp versionre( "(\\d+)\\.(\\d+)(\\.(\\d+))?" );
  if ( ! versionre.exactMatch( version ) )
    KIG_FILTER_PARSE_ERROR;
  bool ok = true;
  int major = versionre.cap( 1 ).toInt( &ok );
  bool ok2 = true;
  int minor = versionre.cap( 2 ).toInt( &ok2 );
  if ( ! ok || ! ok2 )
    KIG_FILTER_PARSE_ERROR;

  //   int minorminor = versionre.cap( 4 ).toInt( &ok );

  // we only support 0.[0-7] and 1.0.*
  if ( major > 0 || minor > 9 )
  {
    notSupported( file, i18n( "This file was created by Kig version \"%1\", "
                              "which this version cannot open." ).arg( version ) );
    return false;
  }
  else if ( major == 0 && minor <= 3 )
  {
    notSupported( file, i18n( "This file was created by Kig version \"%1\".\n"
                              "Support for older Kig formats (pre-0.4) has been "
                              "removed from Kig.\n"
                              "You can try to open this file with an older Kig "
                              "version (0.4 to 0.6),\n"
                              "and then save it again, which will save it in the "
                              "new format." ).arg( version ) );
    return false;
  }
  else if ( major == 0 && minor <= 6 )
    return load04( file, main );
  else
    return load07( file, main );
}

KigDocument* KigFilterNative::load04( const QString& file, const QDomElement& docelem )
{
  bool ok = true;

  KigDocument* ret = new KigDocument();

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
                       "that this Kig version does not support.\n"
                       "A standard coordinate system will be used "
                       "instead." ) );
      }
      else ret->setCoordinateSystem( s );
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
          QString error;
          ObjectImp* imp = ObjectImpFactory::instance()->deserialize( tmp, e, error );
          if ( ( !imp ) && !error.isEmpty() )
          {
            parseError( file, error );
            return false;
          }
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
                                      "which this Kig version does not support."
                                      "Perhaps you have compiled Kig without support "
                                      "for this object type,"
                                      "or perhaps you are using an older Kig version." ).arg( tmp ) );
            return false;
          };

          std::vector<ObjectCalcer*> parents;
          for ( std::vector<int>::iterator j = i->parents.begin();
                j != i->parents.end(); ++j )
            parents.push_back( retcalcers[*j - 1] );

          o = new ObjectTypeCalcer( type, parents );
        }
        else continue;

        o->calc( *ret );
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
      ret->addObjects( retholders );
    }
    else continue; // be forward-compatible..
  };

  return ret;
}

KigFilterNative* KigFilterNative::instance()
{
  static KigFilterNative f;
  return &f;
}

KigDocument* KigFilterNative::load07( const QString& file, const QDomElement& docelem )
{
  KigDocument* ret = new KigDocument();

  bool ok = true;
  std::vector<ObjectCalcer::shared_ptr> calcers;
  std::vector<ObjectHolder*> holders;

  QString t = docelem.attribute( "grid" );
  bool tmphide = ( t == "false" ) || ( t == "no" ) || ( t == "0" );
  ret->setGrid( !tmphide );
  t = docelem.attribute( "axes" );
  tmphide = ( t == "false" ) || ( t == "no" ) || ( t == "0" );
  ret->setAxes( !tmphide );

  for ( QDomElement subsectionelement = docelem.firstChild().toElement(); ! subsectionelement.isNull();
        subsectionelement = subsectionelement.nextSibling().toElement() )
  {
    if ( subsectionelement.tagName() == "CoordinateSystem" )
    {
      QString tmptype = subsectionelement.text();
      // compatibility code - to support Invisible coord system...
      if ( tmptype == "Invisible" )
      {
        tmptype = "Euclidean";
        ret->setGrid( false );
        ret->setAxes( false );
      }
      const QCString type = tmptype.latin1();
      CoordinateSystem* s = CoordinateSystemFactory::build( type );
      if ( ! s )
      {
        warning( i18n( "This Kig file has a coordinate system "
                       "that this Kig version does not support.\n"
                       "A standard coordinate system will be used "
                       "instead." ) );
      }
      else ret->setCoordinateSystem( s );
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
          QString error;
          ObjectImp* imp = ObjectImpFactory::instance()->deserialize( tmp, e, error );
          if ( ( !imp ) && !error.isEmpty() )
          {
            parseError( file, error );
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
                                      "which this Kig version does not support."
                                      "Perhaps you have compiled Kig without support "
                                      "for this object type,"
                                      "or perhaps you are using an older Kig version." ).arg( tmp ) );
            return false;
          }

	  // mp: (I take the responsibility for this!) explanation: the usual ObjectTypeCalcer
	  // constructor also "sortArgs" the parents.  I believe that this *must not* be done
	  // when loading from a saved kig file for the following reasons:
	  // 1. the arguments should already be in their intended order, since the file was
	  // saved from a working hierarchy; furthermore we actually want to restore the original
	  // hierarchy, not really to also fix possible problems with the original hierarchy;
	  // 2. calling sortArgs could have undesirable side effects in particular situations,
	  // since kig actually allow an ObjectType to produce different type of ObjectImp's
	  // it may happen that the parents of an object do not satisfy the requirements
	  // enforced by sortArgs (while moving around the free objects) but still be
	  // perfectly valid
          o = new ObjectTypeCalcer( type, parents, false );
        }
        else KIG_FILTER_PARSE_ERROR;

        o->calc( *ret );
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

        tmp = e.attribute( "style" );
        Qt::PenStyle style = ObjectDrawer::styleFromString( tmp );

        tmp = e.attribute( "point-style" );
        int pointstyle = ObjectDrawer::pointStyleFromString( tmp );

        ObjectConstCalcer* namecalcer = 0;
        tmp = e.attribute( "namecalcer" );
        if ( tmp != "none" && !tmp.isEmpty() )
        {
          int ncid = tmp.toInt( &ok );
          if ( !ok ) KIG_FILTER_PARSE_ERROR;
          if ( ncid <= 0 || ncid > calcers.size() )
            KIG_FILTER_PARSE_ERROR;
          if ( ! dynamic_cast<ObjectConstCalcer*>( calcers[ncid-1].get() ) )
            KIG_FILTER_PARSE_ERROR;
          namecalcer = static_cast<ObjectConstCalcer*>( calcers[ncid-1].get() );
        }

        ObjectDrawer* drawer = new ObjectDrawer( color, width, shown, style, pointstyle );
        holders.push_back( new ObjectHolder( calcer, drawer, namecalcer ) );
      }
    }
  }

  ret->addObjects( holders );
  return ret;
}

bool KigFilterNative::save07( const KigDocument& kdoc, QTextStream& stream )
{
  QDomDocument doc( "KigDocument" );

  QDomElement docelem = doc.createElement( "KigDocument" );
  docelem.setAttribute( "Version", KIGVERSION );
  docelem.setAttribute( "CompatibilityVersion", "0.7.0" );
  docelem.setAttribute( "grid", kdoc.grid() );
  docelem.setAttribute( "axes", kdoc.axes() );

  QDomElement cselem = doc.createElement( "CoordinateSystem" );
  cselem.appendChild( doc.createTextNode( kdoc.coordinateSystem().type() ) );
  docelem.appendChild( cselem );

  std::vector<ObjectHolder*> holders = kdoc.objects();
  std::vector<ObjectCalcer*> calcers = getAllParents( getAllCalcers( holders ) );
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
    drawelem.setAttribute( "style", d->styleToString() );
    drawelem.setAttribute( "point-style", d->pointStyleToString() );

    ObjectCalcer* namecalcer = ( *i )->nameCalcer();
    if ( namecalcer )
    {
      std::map<const ObjectCalcer*,int>::const_iterator ncp = idmap.find( namecalcer );
      assert( ncp != idmap.end() );
      int ncid = ncp->second;
      drawelem.setAttribute( "namecalcer", ncid );
    }
    else
    {
      drawelem.setAttribute( "namecalcer", "none" );
    }

    windowelem.appendChild( drawelem );
  };
  docelem.appendChild( windowelem );

  doc.appendChild( docelem );
  stream << doc.toString();
  return true;
}

bool KigFilterNative::save( const KigDocument& data, const QString& file )
{
  return save07( data, file );
}

bool KigFilterNative::save07( const KigDocument& data, const QString& outfile )
{
  // we have an empty outfile, so we have to print all to stdout
  if ( outfile.isEmpty() )
  {
    QTextStream stdoutstream( stdout, IO_WriteOnly );
    return save07( data, stdoutstream );
  }
#ifndef KIG_NO_COMPRESSED_FILES
  if ( !outfile.endsWith( ".kig", false ) )
  {
    // the user wants to save a compressed file, so we have to save our kig
    // file to a temp file and then compress it...

    QString tempdir = KGlobal::dirs()->saveLocation( "tmp" );
    if ( tempdir.isEmpty() )
      return false;

    QString tempname = outfile.section( '/', -1 );
    if ( outfile.endsWith( ".kigz", false ) )
      tempname.remove( QRegExp( "\\.[Kk][Ii][Gg][Zz]$" ) );
    else
      return false;

    QString tmpfile = tempdir + tempname + ".kig";
    QFile ftmpfile( tmpfile );
    if ( !ftmpfile.open( IO_WriteOnly ) )
      return false;
    QTextStream stream( &ftmpfile );
    if ( !save07( data, stream ) )
      return false;
    ftmpfile.close();

    kdDebug() << "tmp saved file: " << tmpfile << endl;

    // creating the archive and adding our file
    KTar* ark = new KTar( outfile,  "application/x-gzip" );
    ark->open( IO_WriteOnly );
    ark->addLocalFile( tmpfile, tempname + ".kig" );
    ark->close();

    // finally, removing temp file
    QFile::remove( tmpfile );

    return true;
  }
  else
  {
#endif
    QFile file( outfile );
    if ( ! file.open( IO_WriteOnly ) )
    {
      fileNotFound( outfile );
      return false;
    }
    QTextStream stream( &file );
    return save07( data, stream );
#ifndef KIG_NO_COMPRESSED_FILES
  }

  // we should never reach this point...
  return false;
#endif
}

/*
bool KigFilterNative::save( const KigDocument& data, QTextStream& stream )
{
  return save07( data, stream );
}
*/
