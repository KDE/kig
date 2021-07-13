// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

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

#include <kig_version.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#include <QDebug>
#include <QDomElement>
#include <QFile>
#include <QFont>
#include <QStandardPaths>
#include <QTextStream>

#include <KTar>

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

static std::vector<HierElem> sortElems( const std::vector<HierElem> &elems )
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
  return mime == QLatin1String("application/x-kig");
}

KigDocument* KigFilterNative::load( const QString& file)
{
  QFile ffile( file );
  if ( ! ffile.open( QIODevice::ReadOnly ) )
  {
    fileNotFound( file );
    return 0;
  };

  QFile kigdoc( file );
  bool iscompressed = false;
  if ( !file.endsWith( QLatin1String( ".kig" ), Qt::CaseInsensitive ) )
  {
    // the file is compressed, so we have to decompress it and fetch the
    // kig file inside it...
    iscompressed = true;

    QString tempdir = QStandardPaths::writableLocation( QStandardPaths::TempLocation );
    if ( tempdir.isEmpty() )
      KIG_FILTER_PARSE_ERROR;

    tempdir += QLatin1Char('/');

    QString tempname = file.section( '/', -1 );
    if ( file.endsWith( QLatin1String( ".kigz" ), Qt::CaseInsensitive ) )
    {
      tempname.remove( QRegExp( "\\.[Kk][Ii][Gg][Zz]$" ) );
    }
    else
      KIG_FILTER_PARSE_ERROR;
    // reading compressed file
    KTar ark( file, QStringLiteral("application/gzip") );
    ark.open( QIODevice::ReadOnly );
    const KArchiveDirectory* dir = ark.directory();
//    assert( dir );
    QStringList entries = dir->entries();
    QStringList kigfiles = entries.filter( QRegExp( "\\.kig$" ) );
    if ( kigfiles.count() != 1 )
      // I throw a generic parse error here, but I should warn the user that
      // this kig archive file doesn't contain one kig file (it contains no
      // kig files or more than one).
      KIG_FILTER_PARSE_ERROR;
    const KArchiveEntry* kigz = dir->entry( kigfiles.at( 0 ) );
    if ( !kigz->isFile() )
      KIG_FILTER_PARSE_ERROR;
    dynamic_cast<const KArchiveFile*>( kigz )->copyTo( tempdir );
    qDebug() << "extracted file: " << tempdir + kigz->name()
              << "exists: " << QFile::exists( tempdir + kigz->name() );

    kigdoc.setFileName( tempdir + kigz->name() );
  }

  if ( !kigdoc.open( QIODevice::ReadOnly ) )
    KIG_FILTER_PARSE_ERROR;

  QDomDocument doc( QStringLiteral("KigDocument") );
  if ( !doc.setContent( &kigdoc ) )
    KIG_FILTER_PARSE_ERROR;
  kigdoc.close();

  // removing temp file
  if ( iscompressed )
    kigdoc.remove();
  
  return load( doc );
}

KigDocument* KigFilterNative::load( const QDomDocument& doc )
{
  QDomElement main = doc.documentElement();

  QString version = main.attribute( QStringLiteral("CompatibilityVersion") );
  if ( version.isEmpty() ) version = main.attribute( QStringLiteral("Version") );
  if ( version.isEmpty() ) version = main.attribute( QStringLiteral("version") );
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
    notSupported( i18n( "This file was created by Kig version \"%1\", "
                        "which this version cannot open.", version ) );
    return 0;
  }
  else if ( major == 0 && minor <= 3 )
  {
    notSupported( i18n( "This file was created by Kig version \"%1\".\n"
                        "Support for older Kig formats (pre-0.4) has been "
                        "removed from Kig.\n"
                        "You can try to open this file with an older Kig "
                        "version (0.4 to 0.6),\n"
                        "and then save it again, which will save it in the "
                        "new format.", version ) );
    return 0;
  }
  else if ( major == 0 && minor <= 6 )
    return load04( main );
  else
    return load07( main );
}

KigDocument* KigFilterNative::load04( const QDomElement& docelem )
{
  bool ok = true;

  KigDocument* ret = new KigDocument();

  for ( QDomNode n = docelem.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    QDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    if ( e.tagName() == QLatin1String("CoordinateSystem") )
    {
      const QByteArray type = e.text().toLatin1();
      CoordinateSystem* s = CoordinateSystemFactory::build( type.data() );
      if ( ! s )
      {
        warning( i18n( "This Kig file has a coordinate system "
                       "that this Kig version does not support.\n"
                       "A standard coordinate system will be used "
                       "instead." ) );
      }
      else ret->setCoordinateSystem( s );
    }
    else if ( e.tagName() == QLatin1String("Objects") )
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
        if ( e.tagName() == QLatin1String("Data") || e.tagName() == QLatin1String("Property") || e.tagName() == QLatin1String("Object") )
        {
          // fetch the id
          QString tmp = e.attribute(QStringLiteral("id"));
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
          if ( f.tagName() == QLatin1String("Parent") )
          {
            QString tmp = f.attribute( QStringLiteral("id") );
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
        bool internal = e.attribute( QStringLiteral("internal") ) == QLatin1String("true") ? true : false;
        ObjectCalcer* o = 0;
        if ( e.tagName() == QLatin1String("Data") )
        {
          QString tmp = e.attribute( QStringLiteral("type") );
          if ( tmp.isNull() )
            KIG_FILTER_PARSE_ERROR;
          QString error;
          ObjectImp* imp = ObjectImpFactory::instance()->deserialize( tmp, e, error );
          if ( ( !imp ) && !error.isEmpty() )
          {
            parseError( error );
            return 0;
          }
          o = new ObjectConstCalcer( imp );
        }
        else if ( e.tagName() == QLatin1String("Property") )
        {
          QByteArray propname;
          for ( QDomElement ec = e.firstChild().toElement(); !ec.isNull();
                ec = ec.nextSibling().toElement() )
          {
            if ( ec.tagName() == QLatin1String("Property") )
              propname = ec.text().toLatin1();
          };

          if ( i->parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
          ObjectCalcer* parent = retcalcers[i->parents[0] -1];
          QByteArrayList propnames = parent->imp()->propertiesInternalNames();
          int propid = propnames.indexOf( propname );
          if ( propid == -1 )
            KIG_FILTER_PARSE_ERROR;

          o = new ObjectPropertyCalcer( parent, propname );
        }
        else if ( e.tagName() == QLatin1String("Object") )
        {
          QString tmp = e.attribute( QStringLiteral("type") );
          if ( tmp.isNull() )
            KIG_FILTER_PARSE_ERROR;

          const ObjectType* type =
            ObjectTypeFactory::instance()->find( tmp.toLatin1() );
          if ( !type )
          {
            notSupported( i18n( "This Kig file uses an object of type \"%1\", "
                                "which this Kig version does not support."
                                "Perhaps you have compiled Kig without support "
                                "for this object type,"
                                "or perhaps you are using an older Kig version.", tmp ) );
            return 0;
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
          QString tmp = e.attribute( QStringLiteral("color") );
          QColor color( tmp );
          if ( !color.isValid() )
            KIG_FILTER_PARSE_ERROR;

          tmp = e.attribute( QStringLiteral("shown") );
          bool shown = !( tmp == QLatin1String("false") || tmp == QLatin1String("no") );

          tmp = e.attribute( QStringLiteral("width") );
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

static const char* obsoletemessage = I18N_NOOP(
                           "This Kig file uses an object of type \"%1\", "
                           "which is obsolete, you should save the construction with "
                           "a different name and check that it works as expected." );

KigDocument* KigFilterNative::load07( const QDomElement& docelem )
{
  KigDocument* ret = new KigDocument();

  bool ok = true;
  std::vector<ObjectCalcer::shared_ptr> calcers;
  std::vector<ObjectHolder*> holders;

  QString t = docelem.attribute( QStringLiteral("grid") );
  bool tmphide = ( t == QLatin1String("false") ) || ( t == QLatin1String("no") ) || ( t == QLatin1String("0") );
  ret->setGrid( !tmphide );
  t = docelem.attribute( QStringLiteral("axes") );
  tmphide = ( t == QLatin1String("false") ) || ( t == QLatin1String("no") ) || ( t == QLatin1String("0") );
  ret->setAxes( !tmphide );

  for ( QDomElement subsectionelement = docelem.firstChild().toElement(); ! subsectionelement.isNull();
        subsectionelement = subsectionelement.nextSibling().toElement() )
  {
    if ( subsectionelement.tagName() == QLatin1String("CoordinateSystem") )
    {
      QString tmptype = subsectionelement.text();
      // compatibility code - to support Invisible coord system...
      if ( tmptype == QLatin1String("Invisible") )
      {
        tmptype = QStringLiteral("Euclidean");
        ret->setGrid( false );
        ret->setAxes( false );
      }
      const QByteArray type = tmptype.toLatin1();
      CoordinateSystem* s = CoordinateSystemFactory::build( type.data() );
      if ( ! s )
      {
        warning( i18n( "This Kig file has a coordinate system "
                       "that this Kig version does not support.\n"
                       "A standard coordinate system will be used "
                       "instead." ) );
      }
      else ret->setCoordinateSystem( s );
    }
    else if ( subsectionelement.tagName() == QLatin1String("Hierarchy") )
    {
      for ( QDomElement e = subsectionelement.firstChild().toElement(); ! e.isNull();
            e = e.nextSibling().toElement() )
      {
        QString tmp = e.attribute( QStringLiteral("id") );
        uint id = tmp.toInt( &ok );
        if ( id <= 0 ) KIG_FILTER_PARSE_ERROR;

        std::vector<ObjectCalcer*> parents;
        for ( QDomElement parentel = e.firstChild().toElement(); ! parentel.isNull();
              parentel = parentel.nextSibling().toElement() )
        {
          if ( parentel.tagName() != QLatin1String("Parent") ) continue;
          QString tmp = parentel.attribute( QStringLiteral("id") );
          uint parentid = tmp.toInt( &ok );
          if ( ! ok ) KIG_FILTER_PARSE_ERROR;
          if ( parentid == 0 || parentid > calcers.size() ) KIG_FILTER_PARSE_ERROR;
          ObjectCalcer* parent = calcers[parentid - 1].get();
          if ( ! parent ) KIG_FILTER_PARSE_ERROR;
          parents.push_back( parent );
        }

        ObjectCalcer* o = 0;

        if ( e.tagName() == QLatin1String("Data") )
        {
          if ( !parents.empty() ) KIG_FILTER_PARSE_ERROR;
          QString tmp = e.attribute( QStringLiteral("type") );
          QString error;
          ObjectImp* imp = ObjectImpFactory::instance()->deserialize( tmp, e, error );
          if ( ( !imp ) && !error.isEmpty() )
          {
            parseError( error );
            return 0;
          }
          o = new ObjectConstCalcer( imp );
        }
        else if ( e.tagName() == QLatin1String("Property") )
        {
          if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
          QByteArray propname = e.attribute( QStringLiteral("which") ).toLatin1();

          ObjectCalcer* parent = parents[0];
          int propid = parent->imp()->propertiesInternalNames().indexOf( propname );
          if ( propid == -1 ) KIG_FILTER_PARSE_ERROR;

          o = new ObjectPropertyCalcer( parent, propname );
        }
        else if ( e.tagName() == QLatin1String("Object") )
        {
          QString tmp = e.attribute( QStringLiteral("type") );
          const ObjectType* type =
            ObjectTypeFactory::instance()->find( tmp.toLatin1() );
          if ( ! type )
          {
            if ( tmp == QLatin1String("MeasureTransport") && parents.size() == 3 )
            {
              warning( i18n( obsoletemessage, tmp ) );
              type = ObjectTypeFactory::instance()->find( "TransportOfMeasure" );
              ObjectCalcer* circle = parents[0];
              ObjectCalcer* point = parents[1];
              ObjectCalcer* segment = parents[2];
              parents[0] = segment;
              parents[1] = circle;
              parents[2] = point;
            } else if ( tmp == QLatin1String("LineCubicIntersection") )
            {
              warning( i18n( obsoletemessage, tmp ) );
              type = ObjectTypeFactory::instance()->find( "CubicLineIntersection" );
            } else if ( tmp == QLatin1String("InvertLine") )
            {
              warning( i18n( obsoletemessage, tmp ) );
              type = ObjectTypeFactory::instance()->find( "CircularInversion" );
            } else if ( tmp == QLatin1String("InvertSegment") )
            {
              warning( i18n( obsoletemessage, tmp ) );
              type = ObjectTypeFactory::instance()->find( "CircularInversion" );
            } else if ( tmp == QLatin1String("InvertCircle") )
            {
              warning( i18n( obsoletemessage, tmp ) );
              type = ObjectTypeFactory::instance()->find( "CircularInversion" );
            } else if ( tmp == QLatin1String("InvertArc") )
            {
              warning( i18n( obsoletemessage, tmp ) );
              type = ObjectTypeFactory::instance()->find( "CircularInversion" );
            } else if ( tmp == QLatin1String("ConicArcBTPC") )
            {
              warning( i18n( obsoletemessage, tmp ) );
              type = ObjectTypeFactory::instance()->find( "ConicArcBCTP" );
              //
              // the only difference is in the order of parents
              // entering the center first seems more useful, and allows
              // for a better user interface
              //
              ObjectCalcer* point = parents[3];
              parents[3] = parents[2];
              parents[2] = parents[1];
              parents[1] = parents[0];
              parents[0] = point;
            } else {
              notSupported( i18n( "This Kig file uses an object of type \"%1\", "
                                  "which this Kig version does not support."
                                  "Perhaps you have compiled Kig without support "
                                  "for this object type,"
                                  "or perhaps you are using an older Kig version.", tmp ) );
              return 0;
            }
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
    else if ( subsectionelement.tagName() == QLatin1String("View") )
    {
      for ( QDomElement e = subsectionelement.firstChild().toElement(); ! e.isNull();
            e = e.nextSibling().toElement() )
      {
        if ( e.tagName() != QLatin1String("Draw") ) KIG_FILTER_PARSE_ERROR;

        QString tmp = e.attribute( QStringLiteral("object") );
        uint id = tmp.toInt( &ok );
        if ( !ok ) KIG_FILTER_PARSE_ERROR;
        if ( id <= 0 || id > calcers.size() )
          KIG_FILTER_PARSE_ERROR;
        ObjectCalcer* calcer = calcers[id-1].get();

        tmp = e.attribute( QStringLiteral("color") );
        QColor color( tmp );
        if ( !color.isValid() )
          KIG_FILTER_PARSE_ERROR;

        tmp = e.attribute( QStringLiteral("shown") );
        bool shown = !( tmp == QLatin1String("false") || tmp == QLatin1String("no") );

        tmp = e.attribute( QStringLiteral("width") );
        int width = tmp.toInt( &ok );
        if ( ! ok ) width = -1;

        tmp = e.attribute( QStringLiteral("style") );
        Qt::PenStyle style = ObjectDrawer::styleFromString( tmp );

        tmp = e.attribute( QStringLiteral("point-style") );
        Kig::PointStyle pointstyle = Kig::pointStyleFromString( tmp );

        tmp = e.attribute( QStringLiteral("font") );
        QFont f;
        if ( !tmp.isEmpty() )
          f.fromString( tmp );

        ObjectConstCalcer* namecalcer = 0;
        tmp = e.attribute( QStringLiteral("namecalcer") );
        if ( tmp != QLatin1String("none") && !tmp.isEmpty() )
        {
          int ncid = tmp.toInt( &ok );
          if ( !ok ) KIG_FILTER_PARSE_ERROR;
          if ( ncid <= 0 || ncid > static_cast<int>( calcers.size() ) )
            KIG_FILTER_PARSE_ERROR;
          if ( ! dynamic_cast<ObjectConstCalcer*>( calcers[ncid-1].get() ) )
            KIG_FILTER_PARSE_ERROR;
          namecalcer = static_cast<ObjectConstCalcer*>( calcers[ncid-1].get() );
        }

        ObjectDrawer* drawer = new ObjectDrawer( color, width, shown, style, pointstyle, f );
        holders.push_back( new ObjectHolder( calcer, drawer, namecalcer ) );
      }
    }
  }

  ret->addObjects( holders );
  return ret;
}

bool KigFilterNative::save07( const KigDocument& kdoc, QTextStream& stream )
{
  QDomDocument doc( QStringLiteral("KigDocument") );

  QDomProcessingInstruction xmlpi = doc.createProcessingInstruction(
          QStringLiteral( "xml" ), QStringLiteral( "version=\"1.0\" encoding=\"utf-8\"" ) );
  doc.appendChild( xmlpi );

  QDomElement docelem = doc.createElement( QStringLiteral("KigDocument") );
  docelem.setAttribute( QStringLiteral("Version"), KIG_VERSION_STRING );
  docelem.setAttribute( QStringLiteral("CompatibilityVersion"), QStringLiteral("0.7.0") );
  docelem.setAttribute( QStringLiteral("grid"), kdoc.grid() );
  docelem.setAttribute( QStringLiteral("axes"), kdoc.axes() );

  QDomElement cselem = doc.createElement( QStringLiteral("CoordinateSystem") );
  cselem.appendChild( doc.createTextNode( kdoc.coordinateSystem().type() ) );
  docelem.appendChild( cselem );

  std::vector<ObjectHolder*> holders = kdoc.objects();
  std::vector<ObjectCalcer*> calcers = getAllParents( getAllCalcers( holders ) );
  calcers = calcPath( calcers );

  QDomElement hierelem = doc.createElement( QStringLiteral("Hierarchy") );
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
      objectelem = doc.createElement( QStringLiteral("Data") );
      QString ser =
        ObjectImpFactory::instance()->serialize( *(*i)->imp(), objectelem, doc );
      objectelem.setAttribute( QStringLiteral("type"), ser );
    }
    else if ( dynamic_cast<const ObjectPropertyCalcer*>( *i ) )
    {
      const ObjectPropertyCalcer* o = static_cast<const ObjectPropertyCalcer*>( *i );
      objectelem = doc.createElement( QStringLiteral("Property") );

      QByteArray propname = o->parent()->imp()->getPropName( o->propGid() );
      objectelem.setAttribute( QStringLiteral("which"), QString( propname ) );
    }
    else if ( dynamic_cast<const ObjectTypeCalcer*>( *i ) )
    {
      const ObjectTypeCalcer* o = static_cast<const ObjectTypeCalcer*>( *i );
      objectelem = doc.createElement( QStringLiteral("Object") );
      objectelem.setAttribute( QStringLiteral("type"), o->type()->fullName() );
    }
    else assert( false );

    const std::vector<ObjectCalcer*> parents = ( *i )->parents();
    for ( std::vector<ObjectCalcer*>::const_iterator i = parents.begin(); i != parents.end(); ++i )
    {
      std::map<const ObjectCalcer*,int>::const_iterator idp = idmap.find( *i );
      assert( idp != idmap.end() );
      int pid = idp->second;
      QDomElement pel = doc.createElement( QStringLiteral("Parent") );
      pel.setAttribute( QStringLiteral("id"), pid );
      objectelem.appendChild( pel );
    }

    objectelem.setAttribute( QStringLiteral("id"), id++ );
    hierelem.appendChild( objectelem );
  }
  docelem.appendChild( hierelem );

  QDomElement windowelem = doc.createElement( QStringLiteral("View") );
  for ( std::vector<ObjectHolder*>::iterator i = holders.begin(); i != holders.end(); ++i )
  {
    std::map<const ObjectCalcer*,int>::const_iterator idp = idmap.find( ( *i )->calcer() );
    assert( idp != idmap.end() );
    int id = idp->second;

    const ObjectDrawer* d = ( *i )->drawer();
    QDomElement drawelem = doc.createElement( QStringLiteral("Draw") );
    drawelem.setAttribute( QStringLiteral("object"), id );
    drawelem.setAttribute( QStringLiteral("color"), d->color().name() );
    drawelem.setAttribute( QStringLiteral("shown"), QLatin1String( d->shown() ? "true" : "false" ) );
    drawelem.setAttribute( QStringLiteral("width"), QString::number( d->width() ) );
    drawelem.setAttribute( QStringLiteral("style"), d->styleToString() );
    drawelem.setAttribute( QStringLiteral("point-style"), Kig::pointStyleToString( d->pointStyle() ) );
    drawelem.setAttribute( QStringLiteral("font"), d->font().toString() );

    ObjectCalcer* namecalcer = ( *i )->nameCalcer();
    if ( namecalcer )
    {
      std::map<const ObjectCalcer*,int>::const_iterator ncp = idmap.find( namecalcer );
      assert( ncp != idmap.end() );
      int ncid = ncp->second;
      drawelem.setAttribute( QStringLiteral("namecalcer"), ncid );
    }
    else
    {
      drawelem.setAttribute( QStringLiteral("namecalcer"), QStringLiteral("none") );
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
    QTextStream stdoutstream( stdout, QIODevice::WriteOnly );
    stdoutstream.setCodec( "UTF-8" );
    return save07( data, stdoutstream );
  }
  if ( !outfile.endsWith( QLatin1String( ".kig" ), Qt::CaseInsensitive ) )
  {
    // the user wants to save a compressed file, so we have to save our kig
    // file to a temp file and then compress it...

    QString tempdir = QStandardPaths::writableLocation( QStandardPaths::TempLocation );
    if ( tempdir.isEmpty() )
      return false;

    QString tempname = outfile.section( '/', -1 );
    if ( outfile.endsWith( QLatin1String( ".kigz" ), Qt::CaseInsensitive ) )
      tempname.remove( QRegExp( "\\.[Kk][Ii][Gg][Zz]$" ) );
    else
      return false;

    const QString tmpfile = tempdir + QLatin1Char('/') + tempname + ".kig";
    QFile ftmpfile( tmpfile );
    if ( !ftmpfile.open( QIODevice::WriteOnly ) )
      return false;
    QTextStream stream( &ftmpfile );
    stream.setCodec( "UTF-8" );
    if ( !save07( data, stream ) )
      return false;
    ftmpfile.close();

    qDebug() << "tmp saved file: " << tmpfile;

    // creating the archive and adding our file
    KTar ark( outfile,  QStringLiteral("application/gzip") );
    ark.open( QIODevice::WriteOnly );
    ark.addLocalFile( tmpfile, tempname + ".kig" );
    ark.close();

    // finally, removing temp file
    QFile::remove( tmpfile );

    return true;
  }
  else
  {
    QFile file( outfile );
    if ( ! file.open( QIODevice::WriteOnly ) )
    {
      fileNotFound( outfile );
      return false;
    }
    QTextStream stream( &file );
    stream.setCodec( "UTF-8" );
    return save07( data, stream );
  }

  // we should never reach this point...
  return false;
}
