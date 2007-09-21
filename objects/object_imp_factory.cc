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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "object_imp_factory.h"

#include "object_imp.h"
#include "bogus_imp.h"
#include "circle_imp.h"
#include "conic_imp.h"
#include "cubic_imp.h"
#include "line_imp.h"
#include "locus_imp.h"
#include "other_imp.h"
#include "point_imp.h"
#include "text_imp.h"

#include "../misc/coordinate.h"

#include <qdom.h>

#include <klocale.h>

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
}

static void addDoubleElement( const char* name, double d, QDomElement& parent, QDomDocument& doc )
{
  QDomElement e = doc.createElement( name );
  e.appendChild( doc.createTextNode( QString::number( d ) ) );
  parent.appendChild( e );
}

static void addCoordinateElement( const char* name, const Coordinate& d, QDomElement& p, QDomDocument& doc )
{
  QDomElement e = doc.createElement( name );
  addXYElements( d, e, doc );
  p.appendChild( e );
}

QString ObjectImpFactory::serialize( const ObjectImp& d, QDomElement& parent,
                                     QDomDocument& doc ) const
{
  if( d.inherits( IntImp::stype() ) )
  {
    parent.appendChild(
      doc.createTextNode(
        QString::number( static_cast<const IntImp&>( d ).data() ) ) );
    return QString::fromLatin1( "int" );
  }
  else if ( d.inherits( DoubleImp::stype() ) )
  {
    parent.appendChild(
      doc.createTextNode(
        QString::number( static_cast<const DoubleImp&>( d ).data() ) ) );
    return QString::fromLatin1( "double" );
  }
  else if( d.inherits( StringImp::stype() ) )
  {
    parent.appendChild(
      doc.createTextNode(
        static_cast<const StringImp&>( d ).data() ) );
    return QString::fromLatin1( "string" );
  }
  else if ( d.inherits( TestResultImp::stype() ) )
  {
    parent.appendChild(
      doc.createTextNode(
        static_cast<const TestResultImp&>( d ).data() ) );
    return QString::fromLatin1( "testresult" );
  }
  else if( d.inherits( HierarchyImp::stype() ) )
  {
    static_cast<const HierarchyImp&>( d ).data().serialize( parent, doc );
    return QString::fromLatin1( "hierarchy" );
  }
  else if ( d.inherits( TransformationImp::stype() ) )
  {
    const Transformation& trans = static_cast<const TransformationImp&>( d ).data();

    QDomElement matrixe = doc.createElement( "matrix" );
    for ( int i = 0; i < 3; ++i )
    {
      for ( int j = 0; j < 3; ++j )
      {
        QDomElement elel = doc.createElement( "element" );
        elel.setAttribute( "row", QString::number( i ) );
        elel.setAttribute( "column", QString::number( j ) );
        elel.appendChild( doc.createTextNode( QString::number( trans.data( i, j ) ) ) );
        matrixe.appendChild( elel );
      };
    }
    parent.appendChild( matrixe );

    QDomElement homothetye = doc.createElement( "homothetic" );
    const char* ishomothety = trans.isHomothetic() ? "true" : "false";
    homothetye.appendChild( doc.createTextNode( ishomothety ) );
    parent.appendChild( homothetye );

    return QString::fromLatin1( "transformation" );
  }
  else if( d.inherits( AbstractLineImp::stype() ) )
  {
    LineData l = static_cast<const AbstractLineImp&>( d ).data();
    addCoordinateElement( "a", l.a, parent, doc );
    addCoordinateElement( "b", l.b, parent, doc );
    if( d.inherits( SegmentImp::stype() ) )
      return QString::fromLatin1( "segment" );
    else if( d.inherits( RayImp::stype() ) )
      return QString::fromLatin1( "ray" );
    else return QString::fromLatin1( "line" );
  }
  else if( d.inherits( PointImp::stype() ) )
  {
    addXYElements( static_cast<const PointImp&>( d ).coordinate(),
                   parent, doc );
    return QString::fromLatin1( "point" );
  }
  else if( d.inherits( TextImp::stype() ) )
  {
    QString text = static_cast<const TextImp&>( d ).text();
    parent.appendChild(
      doc.createTextNode( text ) );
    return QString::fromLatin1( "text" );
  }
  else if( d.inherits( AngleImp::stype() ) )
  {
    addDoubleElement( "size", static_cast<const AngleImp&>( d ).size(), parent, doc );
    return QString::fromLatin1( "angle" );
  }
  else if ( d.inherits( ArcImp::stype() ) )
  {
    const ArcImp& a = static_cast<const ArcImp&>( d );
    addCoordinateElement( "center", a.center(), parent, doc );
    addDoubleElement( "radius", a.radius(), parent, doc );
    addDoubleElement( "startangle", a.startAngle(), parent, doc );
    addDoubleElement( "angle", a.angle(), parent, doc );
    return QString::fromLatin1( "arc" );
  }
  else if( d.inherits( VectorImp::stype() ) )
  {
    Coordinate dir = static_cast<const VectorImp&>( d ).dir();
    addXYElements( dir, parent, doc );
    return QString::fromLatin1( "vector" );
  }
  else if( d.inherits( LocusImp::stype() ) )
  {
    const LocusImp& locus = static_cast<const LocusImp&>( d );

    // serialize the curve..
    QDomElement curve = doc.createElement( "curve" );
    const CurveImp& curveimp = *locus.curve();
    QString type = serialize( curveimp, curve, doc );
    curve.setAttribute( "type", type );
    parent.appendChild( curve );

    // serialize the hierarchy..
    QDomElement hier = doc.createElement( "calculation" );
    locus.hierarchy().serialize( hier, doc );
    parent.appendChild( hier );

    return QString::fromLatin1( "locus" );
  }
  else if( d.inherits( CircleImp::stype() ) )
  {
    const CircleImp& c = static_cast<const CircleImp&>( d );
    addCoordinateElement( "center", c.center(), parent, doc );
    addDoubleElement( "radius", c.radius(), parent, doc );
    return QString::fromLatin1( "circle" );
  }
  else if( d.inherits( ConicImp::stype() ) )
  {
    const ConicPolarData data = static_cast<const ConicImp&>( d ).polarData();
    addCoordinateElement( "focus1", data.focus1, parent, doc );
    addDoubleElement( "pdimen", data.pdimen, parent, doc );
    addDoubleElement( "ecostheta0", data.ecostheta0, parent, doc );
    addDoubleElement( "esintheta0", data.esintheta0, parent, doc );
    return QString::fromLatin1( "conic" );
  }
  else if( d.inherits( CubicImp::stype() ) )
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
  return QString::null;
}

static Coordinate readXYElements( const QDomElement& e, bool& ok )
{
  double x, y;
  ok = true;
  QDomElement xe = e.firstChild().toElement();
  if ( xe.isNull() || xe.tagName() != "x" )
  {
    ok = false;
    return Coordinate();
  }
  else x = xe.text().toDouble( &ok );

  QDomElement ye = xe.nextSibling().toElement();
  if ( ye.isNull() || ye.tagName() != "y" )
  {
    ok = false;
    return Coordinate();
  }
  else y = ye.text().toDouble( &ok );

  return Coordinate( x, y );
}

static Coordinate readCoordinateElement( QDomNode n, bool& ok,
                                         const char* tagname )
{
  QDomElement e = n.toElement();
  if ( e.isNull() || e.tagName() != tagname )
  {
    ok = false;
    Coordinate ret;
    return ret;
  }
  return readXYElements( e, ok );
}

static double readDoubleElement( QDomNode n, bool& ok,
                                 const char* tagname )
{
  QDomElement e = n.toElement();
  if ( e.isNull() || e.tagName() != tagname )
  {
    ok = false;
    return 0.;
  };
  return e.text().toDouble( &ok );
}

ObjectImp* ObjectImpFactory::deserialize( const QString& type,
                                          const QDomElement& parent,
                                          QString& error ) const
{
#define KIG_GENERIC_PARSE_ERROR \
  { \
    error = i18n( "An error was encountered at line %1 in file %2." ) \
            .arg( __LINE__ ).arg( __FILE__ ); \
    return 0; \
  }

  bool ok = true;
  if ( type == "int" )
  {
    int ret = parent.text().toInt( &ok );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
    return new IntImp( ret );
  }
  else if ( type == "double" )
  {
    double ret = parent.text().toDouble( &ok );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
    return new DoubleImp( ret );
  }
  else if ( type == "string" )
  {
    return new StringImp( parent.text() );
  }
  else if ( type == "testresult" )
  {
    return new TestResultImp( parent.text() );
  }
  else if ( type == "hierarchy" )
  {
    ObjectHierarchy* hier = ObjectHierarchy::buildSafeObjectHierarchy( parent, error );
    if ( ! hier ) return 0;
    HierarchyImp* imp = new HierarchyImp( *hier );
    delete hier;
    return imp;
  }
  else if ( type == "transformation" )
  {
    double data[3][3];
    bool homothetic = false;
    for ( QDomElement childe = parent.firstChild().toElement();
          ! childe.isNull(); childe = childe.nextSibling().toElement() )
    {
      if ( childe.tagName() == "matrix" )
      {
        for ( QDomElement elel = childe.firstChild().toElement();
              ! elel.isNull(); elel = elel.nextSibling().toElement() )
        {
          if ( elel.tagName() != "element" ) KIG_GENERIC_PARSE_ERROR;
          bool ok = true;
          int row = elel.attribute( "row" ).toInt( &ok );
          if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
          int column = elel.attribute( "column" ).toInt( &ok );
          if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
          data[row][column] = elel.text().toDouble( &ok );
          if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
        };
      }
      else if ( childe.tagName() == "homothetic" )
      {
        homothetic = childe.text() == "true";
      }
      else continue;
    };
    Transformation trans( data, homothetic );
    return new TransformationImp( trans );
  }
  else if ( type == "point" )
  {
    Coordinate ret = readXYElements( parent, ok );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
    return new PointImp( ret );
  }
  else if ( type == "line" || type == "segment" || type == "ray" )
  {
    QDomNode n = parent.firstChild();
    Coordinate a = readCoordinateElement( n, ok, "a" );
    if ( !ok ) KIG_GENERIC_PARSE_ERROR;
    n = n.nextSibling();
    Coordinate b = readCoordinateElement( n, ok, "b" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
    if ( type == "line" ) return new LineImp( a, b );
    else if ( type == "segment" ) return new SegmentImp( a, b );
    else return new RayImp( a, b );
  }
  else if( type == "angle" )
  {
    double size = readDoubleElement( parent.firstChild(), ok, "size" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
    return new AngleImp( Coordinate(), 0, size );
  }
  else if ( type == "arc" )
  {
    QDomNode n = parent.firstChild();
    Coordinate center = readCoordinateElement( n, ok, "center" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
    n = n.nextSibling();
    double radius = readDoubleElement( n, ok, "radius" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
    n = n.nextSibling();
    double startangle = readDoubleElement( n, ok, "startangle" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
    n = n.nextSibling();
    double angle = readDoubleElement( n, ok, "angle" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
    return new ArcImp( center, radius, startangle, angle );
  }
  else if( type == "vector" )
  {
    Coordinate dir = readXYElements( parent, ok );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;
    return new VectorImp( Coordinate(), dir );
  }
  else if( type == "locus" )
  {
    QDomElement curvee = parent.firstChild().toElement();
    if ( curvee.isNull() || curvee.tagName() != "curve" ) KIG_GENERIC_PARSE_ERROR;
    QString type = curvee.attribute( "type" );
    ObjectImp* oi = deserialize( type, curvee, error );
    if ( ! oi || ! oi->inherits( CurveImp::stype() ) ) KIG_GENERIC_PARSE_ERROR;
    //CurveImp* curvei = static_cast<CurveImp*>( oi );

    QDomElement hiere = curvee.nextSibling().toElement();
    if ( hiere.isNull() || hiere.tagName() != "calculation" ) KIG_GENERIC_PARSE_ERROR;
    assert( false );    // TODO
//    return new LocusImp( curvei, hier );
  }
  else if( type == "circle" )
  {
    QDomNode n = parent.firstChild();
    Coordinate center = readCoordinateElement( n, ok, "center" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double radius = readDoubleElement( n, ok, "radius" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    return new CircleImp( center, radius );
  }
  else if( type == "conic" )
  {
    QDomNode n = parent.firstChild();
    Coordinate focus1 = readCoordinateElement( n, ok, "focus1" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double pdimen = readDoubleElement( n, ok, "pdimen" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double ecostheta0 = readDoubleElement( n, ok, "ecostheta0" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double esintheta0 = readDoubleElement( n, ok, "esintheta0" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    return new ConicImpPolar(
      ConicPolarData( focus1, pdimen, ecostheta0, esintheta0 ) );
  }
  else if( type == "cubic" )
  {
    QDomElement coeffse = parent.firstChild().toElement();
    if ( coeffse.isNull() || coeffse.tagName() != "coefficients" )
      KIG_GENERIC_PARSE_ERROR;

    QDomNode n = coeffse.firstChild();
    double a000 = readDoubleElement( n, ok, "a000" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double a001 = readDoubleElement( n, ok, "a001" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double a002 = readDoubleElement( n, ok, "a002" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double a011 = readDoubleElement( n, ok, "a011" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double a012 = readDoubleElement( n, ok, "a012" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double a022 = readDoubleElement( n, ok, "a022" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double a111 = readDoubleElement( n, ok, "a111" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double a112 = readDoubleElement( n, ok, "a112" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double a122 = readDoubleElement( n, ok, "a112" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    n = n.nextSibling();
    double a222 = readDoubleElement( n, ok, "a222" );
    if ( ! ok ) KIG_GENERIC_PARSE_ERROR;

    return new CubicImp( CubicCartesianData( a000, a001, a002,
                                             a011, a012, a022,
                                             a111, a112, a122,
                                             a222 ) );
  }

  error = i18n( "This Kig file uses an object of type \"%1\", "
                "which this Kig version does not support."
                "Perhaps you have compiled Kig without support "
                "for this object type,"
                "or perhaps you are using an older Kig version." ).arg( type );
  return 0;
}

