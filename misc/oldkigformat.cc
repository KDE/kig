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
#include "../objects/conic_types.h"
#include "../objects/intersection_types.h"
#include "../objects/line_type.h"
#include "../objects/object.h"
#include "../objects/object_factory.h"
#include "../objects/object_type_factory.h"
#include "../objects/other_type.h"
#include "../objects/point_type.h"
#include "../objects/transform_types.h"
#include "coordinate.h"
#include "objects.h"

#include <qcstring.h>
#include <qstring.h>
#include <qregexp.h>
#include <qdom.h>

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

bool oldElemToNewObject( const QCString type,
                         const QDomElement& e,
                                          RealObject& o,
                                          Objects& dataos,
                                          KigDocument& kdoc )
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
        o.setWidth( e.text().toInt( &ok ) );
        if ( ! ok ) return false;
      };
    };
    if ( !constrained )
    {
      DataObject* xo = new DataObject( new DoubleImp( x ) );
      DataObject* yo = new DataObject( new DoubleImp( y ) );
      o.addParent( xo );
      o.addParent( yo );
      dataos.push_back( xo );
      dataos.push_back( yo );
      o.setType( FixedPointType::instance() );
    }
    else
    {
      assert( o.parents().size() == 1 );
      o.setType( ConstrainedPointType::instance() );
      DataObject* po = new DataObject( new DoubleImp( param ) );
      o.addParent( po );
      dataos.push_back( po );
    };
    return true;
  }
  else if ( type == "Locus" )
  {
    Objects locusos = ObjectFactory::instance()->locus( o.parents() );
    dataos.push_back( locusos[0] );
    RealObject* locus = static_cast<RealObject*>( locusos[1] );
    o.setType( locus->type() );
    o.setParents( locus->parents(), kdoc );
    delete locus;
    return true;
  }
  else if ( type == "CoordinatePropertyPoint" )
  {
    uint size = o.parents().size();
    if ( size != 1 ) return false;
    int wp = -1;
    for ( QDomElement el = e.firstChild().toElement(); ! el.isNull();
          el = el.nextSibling().toElement() )
    {
      if ( el.tagName() == "param" )
      {
        if ( el.attribute( "name" ) == "which-property" )
        {
          QString whichproperty = el.text();
          QCString which = translateOldKigPropertyName( whichproperty );
          wp = o.parents()[0]->propertiesInternalNames().findIndex( which );
        }
      };
    };
    if ( wp == -1 ) return false;
    dataos.push_back( new PropertyObject( o.parents()[0], wp ) );
    dataos.back()->calc( kdoc );
    o.setParents( Objects( dataos.back() ), kdoc );
    o.setType( CopyObjectType::instance() );
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
  else if ( type == "TextLabel" )
  {
    bool ok = true;
    Objects parents = o.parents();
    Objects propos;
    Coordinate c;
    QString text;
    for ( QDomElement el = e.firstChild().toElement(); ! el.isNull();
          el = el.nextSibling().toElement() )
    {
      if ( el.tagName() == "param" )
      {
        QString name = el.attribute( "name" );
        if ( name == "coordinate-x" )
        {
          c.x = el.text().toDouble( &ok );
          if ( ! ok ) return false;
        }
        else if ( name == "coordinate-y" )
        {
          c.y = el.text().toDouble( &ok );
          if ( ! ok ) return false;
        }
        else if ( name.left( 20 ) == "property-for-object-" )
        {
          int nparent = name.mid( 20 ).toInt( &ok );
          if( ! ok ) return false;
          QCString prop = translateOldKigPropertyName( el.text() );
          int propid = parents[nparent]->propertiesInternalNames().findIndex( prop );
          if ( propid == -1 ) return false;
          propos.push_back( new PropertyObject( parents[nparent], propid ) );
        }
        else if ( name == "text" )
          text = el.text();
      };
    };
    propos.calc( kdoc );
    copy( propos.begin(), propos.end(), back_inserter( dataos ) );
    Objects labelos = ObjectFactory::instance()->label( text, c, false, propos );
    labelos.calc( kdoc );
    copy( labelos.begin(), labelos.end() - 1, back_inserter( dataos ) );
    RealObject* label = static_cast<RealObject*>( labelos.back() );
    o.setType( label->type() );
    o.setParents( label->parents(), kdoc );
    delete label;
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
    return true;
  }
}

