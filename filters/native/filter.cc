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

#include "filter.h"

#include "../../kig/kig_part.h"
#include "../../objects/object_factory.h"
#include "../../objects/object.h"
#include "../../objects/bogus_imp.h"
#include "../../objects/point_type.h"
#include "../../objects/line_type.h"
#include "../../objects/conic_types.h"
#include "../../objects/intersection_types.h"
#include "../../objects/transform_types.h"
#include "../../misc/calcpaths.h"

#include <qfile.h>
#include <qregexp.h>
#include <qdom.h>
#include <kglobal.h>

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

KigFilter::Result KigFilterNative::load( const QString& from, KigDocument& to )
{
  QFile file( from );
  if ( ! file.open( IO_ReadOnly ) ) return FileNotFound;
  QDomDocument doc( "KigDocument" );
  if ( !doc.setContent( &file ) ) return ParseError;
  file.close();
  QDomElement main = doc.documentElement();

  QString version = main.attribute( "Version" );
  if ( ! version ) version = main.attribute( "version" );
  if ( ! version ) return ParseError;

  // matches 0.1, 0.2.0, 153.128.99 etc.
  QRegExp versionre( "(\\d+)\\.(\\d+)(\\.(\\d+))?" );
  if ( ! versionre.exactMatch( version ) ) return ParseError;
  bool ok = true;
  int major = versionre.cap( 1 ).toInt( &ok );
  if ( ! ok ) return ParseError;
  int minor = versionre.cap( 2 ).toInt( &ok );
  if ( ! ok ) return ParseError;
//   int minorminor = versionre.cap( 4 ).toInt( &ok );

  // we only support 0.* ( for now ? :)
  if ( major != 0 ) return NotSupported;
  if ( minor > 4 ) return NotSupported;
  return minor > 3 ?
    loadNew( main, to ) : loadOld( main, to );
}

static void extendObjects( Objects& objs, uint size )
{
  if ( size > objs.size() )
  {
    int osize = objs.size();
    objs.resize( size, 0 );
    for ( uint i = osize; i < size; ++i )
      if ( !objs[i] ) objs[i] = new RealObject( 0, Objects() );
  };
};

KigFilter::Result KigFilterNative::loadOld( const QDomElement& main, KigDocument& to )
{
  bool ok = true;
  // TODO: fix memory leaks on parse errors..

  Objects os;
  std::vector<QCString> types;
  std::vector<QDomElement> elements;
  // first the "independent" points..
  QDomNode n;
  QDomElement e;
  for ( n = main.firstChild(); !n.isNull(); n = n.nextSibling() )
  {
    e = n.toElement();
    if ( e.isNull() ) return ParseError;
    if ( e.tagName() == "ObjectHierarchy" ) break;
    if ( e.tagName() == "Point" )
    {
      double x = e.attribute("x").toDouble(&ok);
      if ( ! ok ) return ParseError;
      double y = e.attribute("y").toDouble(&ok);
      if ( ! ok ) return ParseError;
      Coordinate c( x, y );

      os.push_back( ObjectFactory::instance()->fixedPoint( c ) );
    }
    else
      return ParseError;
  };

  QDomElement hier = e;
  for (QDomNode n = hier.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    e = n.toElement();
    if ( e.isNull() ) return ParseError;
    if ( e.tagName() != "HierarchyElement" ) return ParseError;

    QString tmp;
    tmp = e.attribute("typeName");
    if(tmp.isNull()) return ParseError;
    QCString type = tmp.utf8();

    // fetch the id
    tmp = e.attribute("id");
    if(tmp.isNull()) return ParseError;
    uint id = tmp.toInt(&ok);
    if ( !ok ) return ParseError;

    types.resize( std::max( types.size(), id ) );
    types[id-1] = type;
    elements.resize( std::max( types.size(), id ) );
    elements[id-1] = e;

    tmp = e.attribute( "given" );
    bool given = tmp == "true" || tmp == "yes";

    Objects parents;
    QColor color = Qt::blue;
    bool shown = true;
    for ( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
      QDomElement el = n.toElement();
      if ( el.isNull() ) return ParseError;
      if ( el.tagName() == "parent" )
      {
        QString tmp = el.attribute( "id" );
        if ( tmp.isNull() ) return ParseError;
        int pid = tmp.toInt( &ok );
        if ( ! ok ) return ParseError;
        extendObjects( os, pid );
        parents.push_back( os[pid-1] );
      }
      else if ( el.tagName() == "param" )
      {
        QString name = el.attribute( "name" );
        if ( name.isNull() ) return ParseError;
        if ( name == "color" )
        {
          color = QColor( el.text() );
          if ( !color.isValid() )
            return ParseError;
        }
        else if ( name == "shown" )
        {
          shown = el.text() == "true" || el.text() == "yes";
        };
      };
    };
    extendObjects( os, id );
    if ( !given )
    {
      RealObject* o = static_cast<RealObject*>( os[id-1] );
      o->setParents( parents );
      o->setColor( color );
      o->setShown( shown );
    };
  };

  Objects t = calcPath( os );
  for ( Objects::iterator o = t.begin(); o != t.end(); ++o )
    if ( ! oldElemToNewObject( types[o-t.begin()], elements[o-t.begin()] , *static_cast<RealObject*>( *o ) ) )
      return ParseError;

  to.setObjects( os );
  return OK;
}

bool KigFilterNative::oldElemToNewObject( const QCString type,
                                          const QDomElement& e,
                                          RealObject& o )
{
  if ( type == "NormalPoint" )
  {
    bool constrained = false;
    double param = 0.;
    double x = 0.;
    double y = 0.;
    for ( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
      bool ok = true;
      QDomElement e = n.toElement();
      if ( e.isNull() ) return false;
      if ( e.tagName() != "param" ) continue;
      QString name = e.attribute( "name" );
      if ( name == "implementation-type" )
        constrained = e.text() == "Constrained";
      else if ( name == "param" )
      {
        param = e.text().toDouble( &ok );
        if ( ! ok ) return false;
      }
      else if ( name == "x" )
      {
        x = e.text().toDouble( &ok );
        if ( ! ok ) return false;
      }
      else if ( name == "y" )
      {
        y = e.text().toDouble( &ok );
        if ( ! ok ) return false;
      }
      else if ( name == "point-size" )
      {
        o.setWidth( e.text().toDouble( &ok ) );
        if ( ! ok ) return false;
      };
    };
    if ( !constrained )
    {
      o.setType( FixedPointType::instance() );
      o.addParent( new DataObject( new DoubleImp( x ) ) );
      o.addParent( new DataObject( new DoubleImp( y ) ) );
    }
    else
    {
      o.setType( ConstrainedPointType::instance() );
      o.addParent( new DataObject( new DoubleImp( param ) ) );
    };
    return true;
  }
  else if ( type == "Locus" )
  {
    RealObject* locus = ObjectFactory::instance()->locus( o.parents() );
    if ( ! locus ) return false;
    o.setType( locus->type() );
    o.setParents( locus->parents() );
    return true;
  }
  else if ( type == "CircleTransform" || type == "ConicTransform" || type == "CubicTransform" ||
            type == "LineTransform" || type == "RayTransform" || type == "SegmentTransform" ||
            type == "PointTransform" )
  {
  }
  else
  {
    const ObjectType* t = 0;
    if ( type == "CircleLineIntersectionPoint" )
      t = ConicLineIntersectionType::instance();
    else if ( type == "ConicLineIntersectionPoint" )
      t = ConicLineIntersectionType::instance();
    else if ( type == "CubicLineIntersectionPoint" )
      t = LineCubicIntersectionType::instance();
    else if ( type == "IntersectionPoint" )
      t = LineLineIntersectionType::instance();
    else if ( type == "LineTTP" )
      t = LineABType::instance();
    else if ( type == "LineConicAsymptotes" )
      t = ConicAsymptoteType::instance();
    else if ( type == "LineConicRadical" || type == "LineRadical" )
      t = ConicRadicalType::instance();
    else if ( type == "LineDirectrix" )
      t = ConicDirectrixType::instance();
    else if ( type == "LinePolar" )
      t = ConicPolarLineType::instance();
    else if ( type == "MirrorPoint" )
      t = LineReflectionType::instance();
    else if ( type == "PointPolar" )
      t = ConicPolarPointType::instance();
    else if ( type == "RotatedPoint" )
      t = RotationType::instance();
    else if ( type == "TranslatedPoint" )
      t = TranslatedType::instance();
    o.setType( t );
    return true;
  };
}

KigFilter::Result KigFilterNative::save( const KigDocument&, const QString& )
{
  return NotSupported;
}

KigFilter::Result KigFilterNative::loadNew( const QDomElement&, KigDocument& )
{
  return NotSupported;
}
