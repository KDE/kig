// oldkigformat.cc
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

#include "oldkigformat.h"

#include "../objects/bogus_imp.h"
#include "../objects/circle_imp.h"
#include "../objects/conic_imp.h"
#include "../objects/conic_types.h"
#include "../objects/cubic_imp.h"
#include "../objects/intersection_types.h"
#include "../objects/line_imp.h"
#include "../objects/line_type.h"
#include "../objects/locus_imp.h"
#include "../objects/object.h"
#include "../objects/object_factory.h"
#include "../objects/object_type_factory.h"
#include "../objects/other_type.h"
#include "../objects/other_imp.h"
#include "../objects/point_imp.h"
#include "../objects/point_type.h"
#include "../objects/text_imp.h"
#include "../objects/transform_types.h"
#include "coordinate.h"
#include "objects.h"

#include <qcstring.h>
#include <qstring.h>
#include <qregexp.h>
#include <qdom.h>

static bool oldElemToNewObject( const QCString type,
                                const QDomElement& e,
                                RealObject& o,
                                const KigDocument& kdoc )
{
  bool ok = true;
  std::map<QCString, QString> params;
  for ( QDomElement el = e.firstChild().toElement(); ! el.isNull();
        el = el.nextSibling().toElement() )
    if ( el.tagName() == "param" )
    {
      if ( el.attribute( "name" ).latin1() == "point-size" )
      {
        o.setWidth( el.text().toInt( &ok ) );
        if ( ! ok ) return false;
      }
      else
        params[el.attribute( "name" ).latin1()] = el.text();
    };

  if ( type == "NormalPoint" )
  {
    o.setWidth( 5 ); // proper width default
    bool constrained = params["implementation-type"] == "Constrained";
    if ( constrained )
    {
      double param = params["param"].toDouble( &ok );
      if ( ! ok ) return false;
      assert( o.parents().size() == 1 );
      o.setType( ConstrainedPointType::instance() );
      DataObject* po = new DataObject( new DoubleImp( param ) );
      o.addParent( po );
    }
    else
    {
      double x = params["x"].toDouble( &ok );
      if ( ! ok ) return false;
      double y = params["y"].toDouble( &ok );
      if ( ! ok ) return false;
      DataObject* xo = new DataObject( new DoubleImp( x ) );
      DataObject* yo = new DataObject( new DoubleImp( y ) );
      o.addParent( xo );
      o.addParent( yo );
      o.setType( FixedPointType::instance() );
    }
  }
  else if ( type == "Locus" )
  {
    Object* tlocus = ObjectFactory::instance()->locus( o.parents() );
    RealObject* locus = static_cast<RealObject*>( tlocus );
    o.setType( locus->type() );
    o.setParents( locus->parents() );
    // make sure locus gets deleted properly..
    delete locus;
  }
  else if ( type == "CoordinatePropertyPoint" )
  {
    uint size = o.parents().size();
    if ( size != 1 ) return false;
    QString whichproperty = params["which-property"];
    QCString which = translateOldKigPropertyName( whichproperty );
    Object* po = ObjectFactory::instance()->propertyObject(
      o.parents()[0], which );
    po->calc( kdoc );
    o.setParents( Objects( po ) );
    o.setType( CopyObjectType::instance() );
  }
  else if ( type == "CircleTransform" ||
            type == "ConicTransform" || type == "CubicTransform" ||
            type == "LineTransform" ||
            type == "RayTransform" || type == "SegmentTransform" ||
            type == "PointTransform" )
  {
    uint size = o.parents().size();
    if ( size < 2 ) return false;
    const ObjectType* t = 0;
    const Object* transform = o.parents()[1];
    if ( transform->hasimp( VectorImp::stype() ) )
      t = TranslatedType::instance();
    else if ( transform->hasimp( PointImp::stype() ) )
      t = PointReflectionType::instance();
    else if ( transform->hasimp( RayImp::stype() ) )
      t = ProjectiveRotationType::instance();
    else if ( transform->hasimp( AngleImp::stype() ) )
      t = RotationType::instance();
    else if ( transform->hasimp( SegmentImp::stype() ) )
    {
      if ( size != 3 ) return false;
      transform = o.parents()[2];
      if ( transform->hasimp( PointImp::stype() ) )
        t = ScalingOverCenterType::instance();
      else if ( transform->hasimp( AbstractLineImp::stype() ) )
        t = ScalingOverLineType::instance();
    }
    else if ( transform->hasimp( LineImp::stype() ) )
      t = LineReflectionType::instance();
    o.setType( t );
  }
  else if ( type == "TextLabel" )
  {
    bool ok = true;
    Objects parents = o.parents();
    Objects propos;
    double x = params["coordinate-x"].toDouble( &ok );
    if ( ! ok ) return false;
    double y = params["coordinate-y"].toDouble( &ok );
    if ( ! ok ) return false;
    Coordinate c( x, y );
    QString text = params["text"];
    for ( uint i = 0; i < parents.size(); ++i )
    {
      QCString d;
      d.setNum( i );
      d.prepend( "property-for-object-" );
      QCString prop = translateOldKigPropertyName( params[d] );
      int propid = parents[i]->propertiesInternalNames().findIndex( prop );
      if ( propid == -1 ) return false;
      propos.push_back( new PropertyObject( parents[i], propid ) );
    };
    propos.calc( kdoc );
    Object* tlabel = ObjectFactory::instance()->label( text, c, false, propos, kdoc );
    assert( tlabel->inherits( Object::ID_RealObject ) );
    RealObject* label = static_cast<RealObject*>( tlabel );
    o.setType( label->type() );
    o.setParents( label->parents() );

    delete label;
  }
  else if ( type == "CircleLineIntersectionPoint" )
  {
    QString sside = params["circlelineintersect-side"];
    int side = sside == "first" ? -1 : 1;
    DataObject* ndo = new DataObject( new IntImp( side ) );
    o.addParent( ndo );
    o.setType( ConicLineIntersectionType::instance() );
  }
  else if ( type == "ConicLineIntersectionPoint" )
  {
    int side = params["coniclineintersect-side"].toInt( & ok );
    if ( ! ok ) return false;
    DataObject* ndo = new DataObject( new IntImp( side ) );
    o.addParent( ndo );
    o.setType( ConicLineIntersectionType::instance() );
  }
  else if ( type == "CubicLineIntersectionPoint" )
  {
    int root = params["cubiclineintersect-root"].toInt( &ok );
    if ( ! ok ) return false;
    DataObject* ndo = new DataObject( new IntImp( root ) );
    o.addParent( ndo );
    o.setType( LineCubicIntersectionType::instance() );
  }
  else if ( type == "LineConicAsymptotes" )
  {
    int branch = params["lineconicasymptotes-branch"].toInt( &ok );
    if ( ! ok ) return false;
    DataObject* ndo = new DataObject( new IntImp( branch ) );
    o.addParent( ndo );
    o.setType( ConicAsymptoteType::instance() );
  }
  else if ( type == "LineConicRadical" || type == "LineRadical" )
  {
    int which = 1;
    int zeroindex = 1;
    std::map<QCString, QString>::iterator i;
    i = params.find( "lineconicradical-branch" );
    if ( i != params.end() )
    {
      which = i->second.toInt( &ok );
      if ( ! ok ) return false;
    };
    i = params.find( "lineconicradical-zero" );
    if ( i != params.end() )
    {
      zeroindex = i->second.toInt( &ok );
      if ( ! ok ) return false;
    };
    DataObject* ndo = new DataObject( new IntImp( which ) );
    o.addParent( ndo );
    ndo = new DataObject( new IntImp( zeroindex ) );
    o.addParent( ndo );
    o.setType( ConicRadicalType::instance() );
  }
  else
  {
    const ObjectType* t = 0;
    if ( type == "IntersectionPoint" )
      t = LineLineIntersectionType::instance();
    else if ( type == "LineTTP" )
      t = LineABType::instance();
    else if ( type == "LineDirectrix" )
      t = ConicDirectrixType::instance();
    else if ( type == "LinePolar" )
      t = ConicPolarLineType::instance();
    else if ( type == "MirrorPoint" )
      t = LineReflectionType::instance();
    else if ( type == "PointPolar" )
      t = ConicPolarPointType::instance();
    else if ( type == "Ray" )
      t = RayABType::instance();
    else if ( type == "RotatedPoint" )
      t = RotationType::instance();
    else if ( type == "Segment" )
      t = SegmentABType::instance();
    else if ( type == "TranslatedPoint" )
      t = TranslatedType::instance();
    else
    {
      t = ObjectTypeFactory::instance()->find( type );
      if ( ! t ) return false;
    };
    o.setType( t );
  }
  return true;
}

QCString translateOldKigPropertyName( const QString& whichproperty )
{
  // we need to translate the property names from old kig to new
  // kig..
  // a lot of them are simply the lower case of the other ones..
  QCString ret = whichproperty.lower().latin1();
  // this deals with lots of names too..
  ret.replace( QRegExp( QString::fromLatin1( " " ) ), "-" );
  if ( ret == "angle-in-radians" )
    ret = "angle-radian";
  else if ( ret == "angle-in-degrees" )
    ret = "angle-degrees";
  return ret;
};

void extendVect( std::vector<HierElem>& vect, uint size )
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

std::vector<HierElem> sortElems( const std::vector<HierElem> elems )
{
  std::vector<HierElem> ret;
  std::vector<bool> seenElems( elems.size(), false );
  for ( uint i = 0; i < elems.size(); ++i )
    visitElem( ret, elems, seenElems, i );
  return ret;
};

bool parseOldObjectHierarchyElements( const QDomElement& firstelement,
                                      const Objects& given, ReferenceObject& retref,
                                      Objects& finalos, const KigDocument& doc )
{
  bool ok = true;

  std::vector<HierElem> elems;

  for (QDomElement e = firstelement; !e.isNull(); e = e.nextSibling().toElement() )
  {
    QString tmp;
    if ( e.tagName() != "HierarchyElement" ) return false;

    // fetch the id
    tmp = e.attribute("id");
    uint id = tmp.toInt(&ok);
    if ( !ok ) return false;

    extendVect( elems, id );
    elems[id-1].el = e;

    for ( QDomElement el = e.firstChild().toElement(); !el.isNull();
          el = el.nextSibling().toElement() )
    {
      if ( el.tagName() == "parent" )
      {
        QString tmp = el.attribute( "id" );
        int pid = tmp.toInt( &ok );
        if ( ! ok ) return false;
        extendVect( elems, id );
        elems[id-1].parents.push_back( pid );
      }
    };
  };

  Objects ret = given;
  ret.resize( elems.size(), 0 );
  ReferenceObject ourref;

  // now we do a topological sort of the elems..
  std::vector<HierElem> sortedElems = sortElems( elems );

  assert( sortedElems.size() == elems.size() );

  // and now we go over them again, this time filling up ret..
  for ( uint i = 0; i < sortedElems.size(); ++i )
  {
    HierElem& elem = sortedElems[i];
    QDomElement e = elem.el;
    if ( e.isNull() ) continue;
    QString tmp;

    tmp = e.attribute( "final" );
    if ( tmp.isNull() ) return false;
    bool final = tmp == "true" || tmp == "yes";

    tmp = e.attribute("typeName");
    if(tmp.isNull()) return false;
    QCString type = tmp.utf8();

    QColor color = Qt::blue;
    bool shown = true;
    for ( QDomElement el = e.firstChild().toElement(); !el.isNull();
          el = el.nextSibling().toElement() )
    {
      if ( el.tagName() == "param" )
      {
        QString name = el.attribute( "name" );
        if ( name == "color" )
        {
          color = QColor( el.text() );
          if ( !color.isValid() ) return false;
        }
        else if ( name == "shown" )
          shown = el.text() == "true" || el.text() == "yes";
      };
    };
    Objects parents;
    for ( uint i = 0; i < elem.parents.size(); ++i )
    {
      assert( ret[elem.parents[i] - 1] );
      parents.push_back( ret[elem.parents[i] - 1] );
    };

    RealObject* o = new RealObject( 0, parents );
    o->setShown( shown );
    o->setColor( color );

    if ( ! oldElemToNewObject( type, e, *o, doc ) )
    {
      // we don't need to delete the objects we already have, that's
      // done by the ReferenceObject "ourref" above..
      finalos.clear();
      return false;
    };
    o->calc( doc );
    ret[elem.id-1] = o;
    ourref.addParent( o );

    if ( final ) finalos.push_back( o );
  };

  assert( ret.size() == elems.size() );

  retref.setParents( ret );

  return true;
};

Object* randomObjectForType( const QCString& type )
{
  // our job here isn't too hard.  We can simply provide DataObjects
  // for almost all object types, and the oldElemToNewObject()
  // function won't know the difference.  The only exception is
  // constrained points.  If a old-style locus has a constrained point
  // as a parent, the import stuff will translate it to a new style
  // locus by looking at the constrained point's parents.  Therefore,
  // we have to make sure the constrained point *has* parents.

  // some random values
  Coordinate a( 0, 0 );
  Coordinate b( 10, 10 );

  // note that we provide constrained points too in the case of fixed
  // points.  This is not necessary, I'm just being lazy ;)
  if ( type == "NormalPoint" )
  {
    DataObject* line =
      new DataObject( new LineImp( a, b ) );
    Object* constrainedpoint = ObjectFactory::instance()->constrainedPoint( line, .5 );
    return constrainedpoint;
  }
  else
  {
    if ( type.left( 7 ) == "Segment" )
      return new DataObject( new SegmentImp( a, b ) );
    else if ( type.contains( "Point" ) )
      return new DataObject( new PointImp( a ) );
    else if ( type.left( 5 ) == "Cubic" )
      return new DataObject( new CubicImp( CubicCartesianData( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ) ) );
    else if ( type.left( 6 ) == "Circle" )
      return new DataObject( new CircleImp( a, 5 ) );
    else if ( type.left( 4 ) == "Line" )
      return new DataObject( new LineImp( a, b ) );
    else if ( type.left( 3 ) == "Ray" )
      return new DataObject( new RayImp( a, b ) );
    else if ( type == "Locus" )
    {
      // a trivial locus..
      DataObject* pdo = new DataObject( new PointImp( a ) );
      Objects pdos( pdo );
      RealObject* cro = new RealObject( CopyObjectType::instance(), pdos );
      ObjectHierarchy hier( pdos, cro );

      delete pdo;
      delete cro;

      return new DataObject( new LocusImp( new LineImp( a, b ), hier ) );
    }
    else if ( type == "TextLabel" )
      return new DataObject( new TextImp( QString::fromLatin1( "a" ), a ) );
    else if ( type == "Angle" )
      return new DataObject( new AngleImp( a, 1., 2. ) );
    else if ( type == "Vector" )
      return new DataObject( new VectorImp( a, b ) );
    else
      return new DataObject( new ConicImpCart( ConicCartesianData( 1, 2, 3, 4, 5, 6 ) ) );
  };
};
