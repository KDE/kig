// object_imp_factory.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#include "object_imp_factory.h"

#include "object_imp.h"
#include "bogus_imp.h"

#include "point_imp.h"
#include "line_imp.h"
#include "locus_imp.h"
#include "conic_imp.h"
#include "cubic_imp.h"
#include "other_imp.h"
#include "circle_imp.h"

#include "../misc/coordinate.h"

#include <qdom.h>

const ObjectImpFactory* ObjectImpFactory::instance()
{
  static const ObjectImpFactory t;
  return &t;
}

ObjectImpFactory::ObjectImpFactory()
{
}

ObjectImpFactory::~ObjectImpFactory()
{
}

static void addXYElements( const Coordinate& c, QDomElement& parent, QDomDocument& doc )
{
  QDomElement xe = doc.createElement( "x" );
  xe.appendChild(
    doc.createTextNode(
      QString::number( c.x ) ) );
  parent.appendChild( xe );
  QDomElement ye = doc.createElement( "y" );
  ye.appendChild(
    doc.createTextNode(
      QString::number( c.y ) ) );
  parent.appendChild( ye );
};

static void addDoubleElement( const char* name, double d, QDomElement& parent, QDomDocument& doc )
{
  QDomElement e = doc.createElement( name );
  e.appendChild( doc.createTextNode( QString::number( d ) ) );
  parent.appendChild( e );
};

static void addCoordinateElement( const char* name, const Coordinate& d, QDomElement& p, QDomDocument& doc )
{
  QDomElement e = doc.createElement( name );
  addXYElements( d, e, doc );
  p.appendChild( e );
};

QString ObjectImpFactory::serialize( const ObjectImp& d, QDomElement& parent,
                                     QDomDocument& doc ) const
{
  if( d.inherits( ObjectImp::ID_IntImp ) )
  {
    parent.appendChild(
      doc.createTextNode(
        QString::number( static_cast<const IntImp&>( d ).data() ) ) );
    return QString::fromLatin1( "int" );
  }
  else if ( d.inherits( ObjectImp::ID_DoubleImp ) )
  {
     parent.appendChild(
      doc.createTextNode(
        QString::number( static_cast<const DoubleImp&>( d ).data() ) ) );
    return QString::fromLatin1( "double" );
  }
  else if( d.inherits( ObjectImp::ID_StringImp ) )
  {
    parent.appendChild(
      doc.createTextNode(
        static_cast<const StringImp&>( d ).data() ) );
    return QString::fromLatin1( "string" );
  }
  else if( d.inherits( ObjectImp::ID_PointImp ) )
  {
    addXYElements( static_cast<const PointImp&>( d ).coordinate(),
                   parent, doc );
    return QString::fromLatin1( "point" );
  }
  else if( d.inherits( ObjectImp::ID_LineImp ) )
  {
    LineData l = static_cast<const AbstractLineImp&>( d ).data();
    addCoordinateElement( "a", l.a, parent, doc );
    addCoordinateElement( "b", l.b, parent, doc );
    if( d.inherits( ObjectImp::ID_SegmentImp ) )
      return QString::fromLatin1( "segment" );
    else if( d.inherits( ObjectImp::ID_RayImp ) )
      return QString::fromLatin1( "ray" );
    else return QString::fromLatin1( "line" );
  }
  else if( d.inherits( ObjectImp::ID_LabelImp ) )
  {
    assert( false );
  }
  else if( d.inherits( ObjectImp::ID_AngleImp ) )
  {
    addDoubleElement( "size", static_cast<const AngleImp&>( d ).size(), parent, doc );
    return QString::fromLatin1( "angle" );
  }
  else if( d.inherits( ObjectImp::ID_VectorImp ) )
  {
    Coordinate dir = static_cast<const VectorImp&>( d ).dir();
    addXYElements( dir, parent, doc );
    return QString::fromLatin1( "vector" );
  }
  else if( d.inherits( ObjectImp::ID_LocusImp ) )
  {
    QDomElement curve = doc.createElement( "curve" );
    const CurveImp* curveimp = static_cast<const LocusImp&>( d ).curve();
    QString type = serialize( *curveimp, curve, doc );
    curve.setAttribute( "type", type );
    parent.appendChild( curve );
    QDomElement hier = doc.createElement( "calculation" );
    // TODO
    assert( false );
    parent.appendChild( hier );
    assert( false );
  }
  else if( d.inherits( ObjectImp::ID_CircleImp ) )
  {
    const CircleImp& c = static_cast<const CircleImp&>( d );
    addCoordinateElement( "center", c.center(), parent, doc );
    addDoubleElement( "radius", c.radius(), parent, doc );
    return QString::fromLatin1( "circle" );
  }
  else if( d.inherits( ObjectImp::ID_ConicImp ) )
  {
    const ConicPolarData data = static_cast<const ConicImp&>( d ).polarData();
    addCoordinateElement( "focus1", data.focus1, parent, doc );
    addDoubleElement( "pdimen", data.pdimen, parent, doc );
    addDoubleElement( "ecostheta0", data.ecostheta0, parent, doc );
    addDoubleElement( "esintheta0", data.esintheta0, parent, doc );
    return QString::fromLatin1( "conic" );
  }
  else if( d.inherits( ObjectImp::ID_CubicImp ) )
  {
    const CubicCartesianData data = static_cast<const CubicImp&>( d ).data();
    QDomElement coeffs = doc.createElement( "coefficients" );
    addDoubleElement( "a000", data.coeffs[0], coeffs, doc );
    addDoubleElement( "a001", data.coeffs[1], coeffs, doc );
    addDoubleElement( "a002", data.coeffs[2], coeffs, doc );
    addDoubleElement( "a011", data.coeffs[3], coeffs, doc );
    addDoubleElement( "a012", data.coeffs[4], coeffs, doc );
    addDoubleElement( "a022", data.coeffs[5], coeffs, doc );
    addDoubleElement( "a111", data.coeffs[6], coeffs, doc );
    addDoubleElement( "a112", data.coeffs[7], coeffs, doc );
    addDoubleElement( "a122", data.coeffs[8], coeffs, doc );
    addDoubleElement( "a222", data.coeffs[9], coeffs, doc );
    parent.appendChild( coeffs );
    return QString::fromLatin1( "cubic" );
  }
  assert( false );
}

