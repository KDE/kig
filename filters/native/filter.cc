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
#include "../../objects/custom_types.h"
#include "../../objects/object_type_factory.h"
#include "../../objects/object.h"
#include "../../objects/bogus_imp.h"
#include "../../objects/point_type.h"
#include "../../objects/line_type.h"
#include "../../objects/conic_types.h"
#include "../../objects/intersection_types.h"
#include "../../objects/transform_types.h"
#include "../../objects/object_imp_factory.h"
#include "../../misc/calcpaths.h"
#include "../../misc/coordinate_system.h"

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
};

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
};

static std::vector<HierElem> sortElems( const std::vector<HierElem> elems )
{
  std::vector<HierElem> ret;
  std::vector<bool> seenElems( elems.size(), false );
  for ( uint i = 0; i < elems.size(); ++i )
    visitElem( ret, elems, seenElems, i );
  return ret;
};

KigFilter::Result KigFilterNative::loadOld( const QDomElement& main, KigDocument& to )
{
  bool ok = true;

  std::vector<HierElem> elems;
  Objects os;

  QDomElement hier = main.firstChild().toElement();
  if ( hier.isNull() ) return ParseError;
  if ( hier.tagName() != "ObjectHierarchy" ) return NotSupported;
  for (QDomNode n = hier.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    QString tmp;
    QDomElement e = n.toElement();
    if ( e.isNull() ) return ParseError;
    if ( e.tagName() != "HierarchyElement" ) return ParseError;

    // fetch the id
    tmp = e.attribute("id");
    if(tmp.isNull()) return ParseError;
    uint id = tmp.toInt(&ok);
    if ( !ok ) return ParseError;

    extendVect( elems, id );
    elems[id-1].el = e;

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
        extendVect( elems, id );
        elems[id-1].parents.push_back( pid );
      }
    };
  };

  os.resize( elems.size(), 0 );

  // now we do a topological sort of the elems..
  std::vector<HierElem> sortedElems = sortElems( elems );

  // and now we go over them again, this time filling up os..
  for ( uint i = 0; i < sortedElems.size(); ++i )
  {
    HierElem& elem = sortedElems[i];
    QDomElement e = elem.el;
    QString tmp;

    tmp = e.attribute("typeName");
    if(tmp.isNull()) return ParseError;
    QCString type = tmp.utf8();

    QColor color = Qt::blue;
    bool shown = true;
    for ( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
      QDomElement el = n.toElement();
      if ( el.isNull() ) return ParseError;
      if ( el.tagName() == "param" )
      {
        QString name = el.attribute( "name" );
        if ( name.isNull() ) return ParseError;
        if ( name == "color" )
        {
          color = QColor( el.text() );
          if ( !color.isValid() ) return ParseError;
        }
        else if ( name == "shown" )
        {
          shown = el.text() == "true" || el.text() == "yes";
        };
      };
    };
    Objects parents;
    for ( uint i = 0; i < elem.parents.size(); ++i )
    {
      assert( os[elem.parents[i] - 1] );
      parents.push_back( os[elem.parents[i] - 1] );
    };

    RealObject* o = new RealObject( 0, parents );
    o->setShown( shown );
    o->setColor( color );

    if ( ! oldElemToNewObject( type, e, *o ) )
    {
      Objects all = getAllParents( os );
      for ( Objects::iterator i = all.begin(); i != all.end(); ++i )
        delete *i;
      return ParseError;
    };
    o->calc( to );
    os[elem.id-1] = o;
  };

  to.setObjects( os );
  return OK;
}

bool KigFilterNative::oldElemToNewObject( const QCString type,
                                          const QDomElement& e,
                                          RealObject& o )
{
  if ( type == "NormalPoint" )
  {
    o.setWidth( 5 ); // proper width default
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
      assert( o.parents().size() == 1 );
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
    uint size = o.parents().size();
    if ( size < 2 ) return false;
    const ObjectType* t = 0;
    const Object* transform = o.parents()[1];
    if ( transform->hasimp( ObjectImp::ID_VectorImp ) )
      t = TranslatedType::instance();
    else if ( transform->hasimp( ObjectImp::ID_PointImp ) )
      t = PointReflectionType::instance();
    else if ( transform->hasimp( ObjectImp::ID_RayImp ) )
      t = ProjectiveRotationType::instance();
    else if ( transform->hasimp( ObjectImp::ID_AngleImp ) )
      t = RotationType::instance();
    else if ( transform->hasimp( ObjectImp::ID_SegmentImp ) )
    {
      if ( size != 3 ) return false;
      transform = o.parents()[2];
      if ( transform->hasimp( ObjectImp::ID_PointImp ) )
        t = ScalingOverCenterType::instance();
      else if ( transform->hasimp( ObjectImp::ID_LineImp ) )
        t = ScalingOverLineType::instance();
    }
    else if ( transform->hasimp( ObjectImp::ID_LineImp ) )
      // line at the end, cause a ray and a segment are also
      // "lines"..
      t = LineReflectionType::instance();
    o.setType( t );
    return true;
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
    else
    {
      t = ObjectTypeFactory::instance()->find( type );
      if ( ! t ) return false;
    };
    o.setType( t );
    return true;
  };
}

KigFilter::Result KigFilterNative::save( const KigDocument& kdoc, const QString& to )
{
  using namespace std;

  QFile file( to );
  if ( ! file.open( IO_WriteOnly ) )
    return FileNotFound;
  QTextStream stream( &file );
  QDomDocument doc( "KigDocument" );

  // TODO ?
//  doc.appendChild( QDomImplementation().createDocumentType( ... ) );

  QDomElement docelem = doc.createElement( "KigDocument" );
  docelem.setAttribute( "Version", "0.4.0" );

  // save the coordinate system type..
  QDomElement cselem = doc.createElement( "CoordinateSystem" );
  cselem.appendChild( doc.createTextNode( kdoc.coordinateSystem().type() ) );
  docelem.appendChild( cselem );

  Objects objs = kdoc.objects();
  objs = getAllParents( objs );

  // save the custom types..
  std::vector<const CustomType*> customtypes;
  for ( Objects::const_iterator i = objs.begin(); i != objs.end(); ++i )
  {
    if ( (*i)->inherits( Object::ID_RealObject ) )
    {
      const ObjectType* type = static_cast<const RealObject*>( *i )->type();
      if ( type->inherits( ObjectType::ID_CustomType ) )
        customtypes.push_back( static_cast<const CustomType*>( type ) );
    }
  }
  if ( !customtypes.empty() )
  {
    QDomElement ctelem = doc.createElement( "Types" );
    for ( uint i = 0; i < customtypes.size(); ++i )
    {
      QDomElement typeelem = doc.createElement( "Type" );
      QString tmp = ObjectTypeFactory::instance()->serialize( *customtypes[i], typeelem, doc );
      typeelem.setAttribute( "type", tmp );
      typeelem.setAttribute( "id", QString::fromLatin1( "custom-%1" ).arg( i + 1 ) );
      ctelem.appendChild( typeelem );
    };
    docelem.appendChild( ctelem );
  };

  // save the objects..
  QDomElement objectselem = doc.createElement( "Objects" );
  objs = calcPath( objs );

  std::map<Object*, int> idmap;
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
    else if ( (*i)->inherits( Object::ID_RealObject ) )
    {
      const RealObject* o = static_cast<const RealObject*>( *i );
      QDomElement e = doc.createElement( "Object" );
      idmap[*i] = id;
      e.setAttribute( "id", id++ );

      if ( o->type()->inherits( ObjectType::ID_CustomType ) )
      {
        // custom types that we have saved along are identified with "custom-id"
        uint cid = find( customtypes.begin(), customtypes.end(), o->type() ) - customtypes.begin();
        assert( cid < customtypes.size() );
        e.setAttribute( "type", QString::fromLatin1( "custom-%1" ).arg( cid + 1 ) );
      }
      else e.setAttribute( "type", o->type()->fullName() );

      e.setAttribute( "color", o->color().name() );
      e.setAttribute( "shown", QString::fromLatin1( o->shown() ? "true" : "false" ) );
      e.setAttribute( "width", QString::number( o->width() ) );

      const Objects& parents = o->parents();
      for ( Objects::const_iterator i = parents.begin(); i != parents.end(); ++i )
      {
        std::map<Object*,int>::const_iterator idp = idmap.find( *i );
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
//  return NotSupported;
  docelem.appendChild( objectselem );

  doc.appendChild( docelem );
  stream << doc.toCString();
  file.close();
  return OK;
}

KigFilter::Result KigFilterNative::loadNew( const QDomElement& docelem, KigDocument& kdoc )
{
  bool ok = true;

  std::vector<CustomType*> customtypes;

  for ( QDomNode n = docelem.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    QDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    if ( e.tagName() == "CoordinateSystem" )
    {
      const QCString type = e.text().latin1();
      CoordinateSystem* s = CoordinateSystemFactory::build( type );
      if ( ! s ) return NotSupported;
      else kdoc.setCoordinateSystem( s );
    }
    else if ( e.tagName() == "Types" )
    {
      QDomElement typeselem = e;
      for ( QDomNode o = typeselem.firstChild(); ! o.isNull(); o = o.nextSibling() )
      {
        QDomElement typeelem = o.toElement();
        if ( typeelem.isNull() ) return ParseError;
        if ( typeelem.tagName() != "Type" ) return ParseError;
        QString tmp = typeelem.attribute( "type" );
        CustomType* type = ObjectTypeFactory::instance()->deserialize( tmp, typeelem );
        tmp = typeelem.attribute( "id" );
        if ( tmp.left( 7 ) != "custom-" ) return ParseError;
        int id = tmp.mid( 7 ).toInt( &ok );
        if ( ! ok ) return ParseError;

        customtypes.resize( kMax( (uint) id, customtypes.size() ), 0 );
        customtypes[id-1] = type;
      };
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
        if ( e.tagName() == "Data" )
        {
          // fetch the id
          QString tmp = e.attribute("id");
          if(tmp.isNull()) return ParseError;
          uint id = tmp.toInt(&ok);
          if ( !ok ) return ParseError;

          extendVect( elems, id );
          elems[id-1].el = e;
        }
        else if ( e.tagName() == "Object" )
        {
          QString tmp = e.attribute( "id" );
          if ( tmp.isNull() ) return ParseError;
          uint id = tmp.toInt( &ok );
          if ( ! ok ) return ParseError;

          for ( QDomNode p = e.firstChild(); !p.isNull(); p = p.nextSibling() )
          {
            QDomElement f = p.toElement();
            if ( f.isNull() ) continue;
            if ( f.tagName() == "Parent" )
            {
              tmp = f.attribute( "id" );
              if ( tmp.isNull() ) return ParseError;
              uint pid = tmp.toInt( &ok );
              if ( ! ok ) return ParseError;

              extendVect( elems, id );
              elems[id-1].parents.push_back( pid );
            }
          }

          extendVect( elems, id );
          elems[id-1].el = e;
        }
        else continue;
      };
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
          if ( tmp.isNull() ) return ParseError;
          ObjectImp* imp = ObjectImpFactory::instance()->deserialize( tmp, e );
          ret[oldsize + i->id - 1] = new DataObject( imp );
        }
        else if ( e.tagName() == "Object" )
        {
          QString tmp = e.attribute( "type" );
          if ( tmp.isNull() ) return ParseError;

          const ObjectType* type = 0;
          if ( tmp.left( 7 ) == "custom-" )
          {
            int id = tmp.mid( 7 ).toInt( &ok );
            if ( ! ok ) return ParseError;
            type = customtypes[id-1];
          }
          else
            type = ObjectTypeFactory::instance()->find( tmp.latin1() );
          if ( !type ) return ParseError;

          tmp = e.attribute( "color" );
          if ( tmp.isNull() ) return ParseError;
          QColor color( tmp );

          tmp = e.attribute( "shown" );
          if ( tmp.isNull() ) return ParseError;
          bool shown = ! ( tmp == "false" || tmp == "no" );

          tmp = e.attribute( "width" );
          int width;
          if ( tmp.isNull() ) width = 5;
          else width = tmp.toInt( &ok );
          if ( ! ok ) width = 5;

          Objects parents;
          for ( std::vector<int>::iterator j = i->parents.begin(); j != i->parents.end(); ++j )
            parents.push_back( ret[oldsize + *j - 1] );

          RealObject* newobj = new RealObject( type, parents );
          newobj->setColor( color );
          newobj->setShown( shown );
          newobj->setWidth( width );
          ret[oldsize + i->id - 1] = newobj;
        }
        else continue;
      }
      kdoc.setObjects( ret );
      for ( uint i = 0; i < customtypes.size(); ++i )
        CustomTypes::instance()->add( customtypes[i] );
    }
    else continue; // be forward-compatible..
  };

  return OK;
}
