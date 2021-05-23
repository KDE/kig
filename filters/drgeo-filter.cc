// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>
// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "drgeo-filter.h"

#include "filters-common.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../misc/common.h"
#include "../misc/coordinate.h"
#include "../objects/angle_type.h"
#include "../objects/arc_type.h"
#include "../objects/bogus_imp.h"
#include "../objects/circle_imp.h"
#include "../objects/circle_type.h"
#include "../objects/conic_imp.h"
#include "../objects/conic_types.h"
#include "../objects/curve_imp.h"
#include "../objects/intersection_types.h"
#include "../objects/line_imp.h"
#include "../objects/line_type.h"
#include "../objects/object_calcer.h"
#include "../objects/object_drawer.h"
#include "../objects/object_factory.h"
#include "../objects/object_holder.h"
#include "../objects/object_type.h"
#include "../objects/other_imp.h"
#include "../objects/other_type.h"
#include "../objects/point_imp.h"
#include "../objects/point_type.h"
#include "../objects/polygon_type.h"
#include "../objects/transform_types.h"
#include "../objects/vector_type.h"

#include <cmath>

#include <QDomDocument>
#include <QInputDialog>
#include <QFile>

#undef DRGEO_DEBUG
//#define DRGEO_DEBUG

struct DrGeoHierarchyElement
{
  QString id;
  std::vector<QString> parents;
};

KigFilterDrgeo::KigFilterDrgeo()
{
}

KigFilterDrgeo::~KigFilterDrgeo()
{
}

bool KigFilterDrgeo::supportMime( const QString& mime )
{
  return mime == QLatin1String("application/x-drgeo");
}

KigDocument* KigFilterDrgeo::load( const QString& file )
{
  QFile f( file );
  if ( ! f.open( QIODevice::ReadOnly ) )
  {
    fileNotFound( file );
    return 0;
  }

  QStringList figures;
  QDomDocument doc( QStringLiteral("drgenius") );
  if ( !doc.setContent( &f ) )
    KIG_FILTER_PARSE_ERROR;
  QDomElement main = doc.documentElement();
  int nmacros = 0;
  // reading figures...
  for ( QDomNode n = main.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    QDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    else if ( e.tagName() == QLatin1String("drgeo") )
      figures.append( e.attribute( QStringLiteral("name") ) );
    else if ( e.tagName() == QLatin1String("macro") )
      nmacros++;
  }
  if ( figures.isEmpty() ) {
    if( nmacros > 0 )
      warning( i18n( "The Dr. Geo file \"%1\" is a macro file so it contains no "
                     "figures.", file ) );
    else
      warning( i18n( "There are no figures in Dr. Geo file \"%1\".", file ) );
    return 0;
  }

  int nfig = figures.count();
  // no figures, no party...
  if ( nfig == 0 )
    return 0;

  QString myfig = figures.at( 0 );

  if ( nfig > 1 )
  {
    // Dr. Geo file has more than 1 figure, let the user choose one...
    bool ok = true;
    myfig = QInputDialog::getItem( nullptr,
                i18n( "Dr. Geo Filter" ),
                i18n( "The current Dr. Geo file contains more than one figure.\n"
                      "Please select which to import:" ),
                figures, 0, false, &ok );
    if ( !ok )
      return 0;
  }

#ifdef DRGEO_DEBUG
  qDebug() << "drgeo file " << file;
#endif

  for ( QDomNode n = main.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    QDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    else if ( e.tagName() == QLatin1String("drgeo") )
    {
      if ( e.attribute(QStringLiteral("name")) == myfig )
      {
#ifdef DRGEO_DEBUG
        qDebug() << "- Figure: '" << e.attribute("name") << "'";
#endif
        bool grid = !e.attribute( QStringLiteral("grid") ).isEmpty() &&
                    ( e.attribute( QStringLiteral("grid") ) != QLatin1String("False") );
        return importFigure( e.firstChild(), grid );
      }
    }
  }

  return 0;
}

static int convertDrgeoIndex( const std::vector<DrGeoHierarchyElement>& es, const QString& myid )
{
  for ( uint i = 0; i < es.size(); ++i )
    if ( es[i].id == myid )
      return i;
  return -1;
}

static const Coordinate convertDrgeoLineParam( const double param, const LineData& line )
{
  const double n = ( param - 0.5 ) * M_PI;
  const Coordinate c = line.dir() / line.dir().length();
  const Coordinate p = line.a + tan( n ) * c;
  return p;
}

static const Coordinate convertDrgeoHalflineParam( const double param, const LineData& line )
{
  const double n = param * M_PI * 0.5;
  const Coordinate c = line.dir() / line.dir().length();
  const Coordinate p = line.a + tan( n ) * c;
  return p;
}

KigDocument* KigFilterDrgeo::importFigure( const QDomNode& f, const bool grid )
{
  KigDocument* ret = new KigDocument();

  using namespace std;
  std::vector<DrGeoHierarchyElement> elems;
  int withoutid = 0;

  // 1st: fetch relationships and build an appropriate structure
  for (QDomNode a = f; ! a.isNull(); a = a.nextSibling() )
  {
    QDomElement domelem = a.toElement();
    if ( domelem.isNull() ) continue;
    else
    {
      DrGeoHierarchyElement elem;
#ifdef DRGEO_DEBUG
      qDebug() << "  * " << domelem.tagName() << "(" << domelem.attribute("type") << ")";
#endif
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == QLatin1String("parent") )
          elem.parents.push_back( ce.attribute( QStringLiteral("ref") ) );
      }
      QString curid = domelem.attribute( QStringLiteral("id") );
      elem.id = !curid.isNull() ? curid : QString::number( withoutid++ ) ;
      elems.push_back( elem );
    }
  }

#ifdef DRGEO_DEBUG
  QString x;
  qDebug() << "+++ elems";
  for ( uint i = 0; i < elems.size(); ++i )
  {
    x = "";
    for ( uint j = 0; j < elems[i].parents.size(); ++j )
    {
      x += elems[i].parents[j] + '_';
    }
    qDebug() << "  --> " << i << " - " << elems[i].id << " - " << x;
  }
#endif

  // 2nd: let's draw!
  int curid = 0;
  const ObjectFactory* fact = ObjectFactory::instance();
  std::vector<ObjectHolder*> holders;
  std::vector<ObjectHolder*> holders2;
  ObjectTypeCalcer* oc = 0;
  ObjectCalcer* oc2 = 0;
  int nignored = 0;

  // there's no need to sort the objects because it seems that DrGeo objects
  // appear in the right order... so let's go!
  for (QDomNode a = f; ! a.isNull(); a = a.nextSibling() )
  {
#ifdef DRGEO_DEBUG
    qDebug() << "+++ id: " << curid;
#endif
    const DrGeoHierarchyElement& el = elems[curid];
    std::vector<ObjectCalcer*> parents;
    for ( uint j = 0; j < el.parents.size(); ++j )
    {
      int parentid = convertDrgeoIndex( elems, el.parents[j] );
      if ( parentid == -1 )
        KIG_FILTER_PARSE_ERROR;
      parents.push_back( holders[parentid-nignored]->calcer() );
    };
    QDomElement domelem = a.toElement();

#ifdef DRGEO_DEBUG
    if ( parents.size() > 0 )
      for ( uint j = 0; j < parents.size(); ++j )
      {
        qDebug() << "+++++++++ parent[" << j << "]: " << parents[j] << " - "
                  << parents[j]->imp()->type()->internalName() << endl;
      }
    else
      qDebug() << "+++++++++ parents: NO";
    qDebug() << "+++++++++ " << domelem.tagName() << " - " << domelem.attribute("type");
#endif

    if ( domelem.isNull() ) continue;
    else if ( domelem.tagName() == QLatin1String("point") )
    {
      QString xs;
      QString ys;
      QString values;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == QLatin1String("x") )
          xs = ce.text();
        else if ( ce.tagName() == QLatin1String("y") )
          ys = ce.text();
        else if ( ce.tagName() == QLatin1String("value") )
          values = ce.text();
      }
      if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Free") )
      {
        bool ok;
        bool ok2;
        double x = xs.toDouble( &ok );
        double y = ys.toDouble( &ok2 );
        if ( ! ( ok && ok2 ) )
          KIG_FILTER_PARSE_ERROR;
        oc = fact->fixedPointCalcer( Coordinate( x, y ) );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Middle_2pts") )
        oc = new ObjectTypeCalcer( MidPointType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Middle_segment") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        if ( !parents[0]->imp()->inherits( SegmentImp::stype() ) )
          KIG_FILTER_PARSE_ERROR;
        ObjectPropertyCalcer* o1 = fact->propertyObjectCalcer( parents[0], "end-point-A" );
        o1->calc( *ret );
        ObjectPropertyCalcer* o2 = fact->propertyObjectCalcer( parents[0], "end-point-B" );
        o2->calc( *ret );
        std::vector<ObjectCalcer*> args;
        args.push_back( o1 );
        args.push_back( o2 );
        oc = new ObjectTypeCalcer( MidPointType::instance(), args );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("On_curve") )
      {
        bool ok3;
        double value = values.toDouble( &ok3 );
        if ( ! ok3 )
          KIG_FILTER_PARSE_ERROR;
        if ( ( parents[0]->imp()->inherits( CircleImp::stype() ) ) ||
             ( parents[0]->imp()->inherits( SegmentImp::stype() ) ) )
          oc = fact->constrainedPointCalcer( parents[0], value );
        else if ( parents[0]->imp()->inherits( LineImp::stype() ) )
        {
          const LineData l = static_cast<const LineImp*>( parents[0]->imp() )->data();
          const Coordinate p = convertDrgeoLineParam( value, l );
          oc = fact->constrainedPointCalcer( parents[0], p, *ret );
        }
        else if ( parents[0]->imp()->inherits( RayImp::stype() ) )
        {
          const LineData l = static_cast<const RayImp*>( parents[0]->imp() )->data();
          const Coordinate p = convertDrgeoHalflineParam( value, l );
          oc = fact->constrainedPointCalcer( parents[0], p, *ret );
        }
        else if ( parents[0]->imp()->inherits( ArcImp::stype() ) )
          oc = fact->constrainedPointCalcer( parents[0], 1 - value );
        else
        {
//          oc = fact->constrainedPointCalcer( parents[0], value );
          notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                              "which Kig does not currently support.", domelem.tagName() , 
                              domelem.attribute( QStringLiteral("type") ) ) );
          return 0;
        }
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Intersection") )
      {
        if ( ( parents[0]->imp()->inherits( AbstractLineImp::stype() ) ) &&
             ( parents[1]->imp()->inherits( AbstractLineImp::stype() ) ) )
          oc = new ObjectTypeCalcer( LineLineIntersectionType::instance(), parents );
        else
        {
          bool ok;
          int which = domelem.attribute( QStringLiteral("extra") ).toInt( &ok );
          if ( !ok ) KIG_FILTER_PARSE_ERROR;
          if ( which == 1 ) which = -1;
          else if ( which == 0 ) which = 1;
          else KIG_FILTER_PARSE_ERROR;
          std::vector<ObjectCalcer*> args = parents;
          const ObjectType* type = 0;
          args.push_back( new ObjectConstCalcer( new IntImp( which ) ) );
          if ( ( parents[0]->imp()->inherits( CircleImp::stype() ) ) &&
               ( parents[1]->imp()->inherits( CircleImp::stype() ) ) )
            type = CircleCircleIntersectionType::instance();
          else if ( ( parents[0]->imp()->inherits( CircleImp::stype() ) &&
                      parents[1]->imp()->inherits( AbstractLineImp::stype() ) ) ||
                    ( parents[1]->imp()->inherits( CircleImp::stype() ) &&
                      parents[0]->imp()->inherits( AbstractLineImp::stype() ) ) )
            type = ConicLineIntersectionType::instance();
          else if ( ( parents[0]->imp()->inherits( ArcImp::stype() ) &&
                      parents[1]->imp()->inherits( AbstractLineImp::stype() ) ) ||
                    ( parents[1]->imp()->inherits( ArcImp::stype() ) &&
                      parents[0]->imp()->inherits( AbstractLineImp::stype() ) ) )
            type = ArcLineIntersectionType::instance();
          else
          {
            notSupported( i18n( "This Dr. Geo file contains an intersection type, "
                                "which Kig does not currently support." ) );
            return 0;
          }
          oc = new ObjectTypeCalcer( type, args );
        }
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Coordinate") )
        oc = new ObjectTypeCalcer( PointByCoordsType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Reflexion") )
        oc = new ObjectTypeCalcer( LineReflectionType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Symmetry") )
        oc = new ObjectTypeCalcer( PointReflectionType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Translation") )
        oc = new ObjectTypeCalcer( TranslatedType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Rotation") )
        oc = new ObjectTypeCalcer( RotationType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Scale") )
        oc = new ObjectTypeCalcer( ScalingOverCenterType::instance(), parents );
      else
      {
        notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                            "which Kig does not currently support.", domelem.tagName() , 
                            domelem.attribute( QStringLiteral("type") ) ) );
        return 0;
      }
#ifdef DRGEO_DEBUG
      qDebug() << "+++++++++ oc:" << oc;
#endif
    }
    else if( ( domelem.tagName() == QLatin1String("line") ) ||
             ( domelem.tagName() == QLatin1String("halfLine") ) ||
             ( domelem.tagName() == QLatin1String("segment") ) ||
             ( domelem.tagName() == QLatin1String("vector") ) ||
             ( domelem.tagName() == QLatin1String("circle") ) ||
             ( domelem.tagName() == QLatin1String("arcCircle") ) ||
             ( domelem.tagName() == QLatin1String("polygon") ) )
    {
      const ObjectType* type = 0;
      if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("2pts") )
      {
        if( domelem.tagName() == QLatin1String("line") )
          type = LineABType::instance();
        else if( domelem.tagName() == QLatin1String("halfLine") )
          type = RayABType::instance();
        else if( domelem.tagName() == QLatin1String("segment") )
          type = SegmentABType::instance();
        else if( domelem.tagName() == QLatin1String("vector") )
          type = VectorType::instance();
        else if( domelem.tagName() == QLatin1String("circle") )
          type = CircleBCPType::instance();
        else
        {
          notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                              "which Kig does not currently support.", domelem.tagName() , 
                              domelem.attribute( QStringLiteral("type") ) ) );
          return 0;
        }
        oc = new ObjectTypeCalcer( type, parents );
      }
      else if( domelem.attribute( QStringLiteral("type") ) == QLatin1String("3pts") )
      {
        if( domelem.tagName() == QLatin1String("arcCircle") )
          type = ArcBTPType::instance();
        else
        {
          notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                              "which Kig does not currently support.", domelem.tagName() , 
                              domelem.attribute( QStringLiteral("type") ) ) );
          return 0;
        }
        oc = new ObjectTypeCalcer( type, parents );
      }
      else if ( ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("segment") ) ||
                ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("radius") ) )
      {
        if( domelem.tagName() == QLatin1String("circle") )
        {
          type = CircleBPRType::instance();
          if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("segment") )
          {
            ObjectPropertyCalcer* o = fact->propertyObjectCalcer( parents[1], "length" );
            o->calc( *ret );
            parents.pop_back();
            parents.push_back( o );
          }
        }
        else
        {
          notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                              "which Kig does not currently support.", domelem.tagName() , 
                              domelem.attribute( QStringLiteral("type") ) ) );
          return 0;
        }
        oc = new ObjectTypeCalcer( type, parents );
      }
      else if( domelem.attribute( QStringLiteral("type") ) == QLatin1String("npts") )
      {
        if( domelem.tagName() == QLatin1String("polygon") )
        {
          if ( parents.size() < 3 ) KIG_FILTER_PARSE_ERROR;
          type = PolygonBNPType::instance();
        }
        else
        {
          notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                              "which Kig does not currently support.", domelem.tagName() , 
                              domelem.attribute( QStringLiteral("type") ) ) );
          return 0;
        }
        oc = new ObjectTypeCalcer( type, parents );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("perpendicular") )
        oc = new ObjectTypeCalcer( LinePerpendLPType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("parallel") )
        oc = new ObjectTypeCalcer( LineParallelLPType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Reflexion") )
        oc = new ObjectTypeCalcer( LineReflectionType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Symmetry") )
        oc = new ObjectTypeCalcer( PointReflectionType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Translation") )
        oc = new ObjectTypeCalcer( TranslatedType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Rotation") )
        oc = new ObjectTypeCalcer( RotationType::instance(), parents );
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("Scale") )
        oc = new ObjectTypeCalcer( ScalingOverCenterType::instance(), parents );
      else
      {
        notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                            "which Kig does not currently support.", domelem.tagName() , 
                            domelem.attribute( QStringLiteral("type") ) ) );
        return 0;
      }
#ifdef DRGEO_DEBUG
      qDebug() << "+++++++++ oc:" << oc;
#endif
    }
    else if( ( domelem.tagName() == QLatin1String("numeric") ) ||
             ( domelem.tagName() == QLatin1String("equation") ) )
    {
      QString xs;
      QString ys;
      QString value;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == QLatin1String("x") )
          xs = ce.text();
        else if ( ce.tagName() == QLatin1String("y") )
          ys = ce.text();
        else if ( ce.tagName() == QLatin1String("value") )
          value = ce.text();
      }
      bool ok;
      bool ok2;
      double x = xs.toDouble( &ok );
      double y = ys.toDouble( &ok2 );
      if ( ! ( ok && ok2 ) )
        KIG_FILTER_PARSE_ERROR;
      Coordinate m( x, y );
      // types of 'numeric'
      if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("value") )
      {
        bool ok3;
        double dvalue = value.toDouble( &ok3 );
        if ( !ok3 )
          KIG_FILTER_PARSE_ERROR;
        oc = fact->numericValueCalcer( dvalue, m, *ret );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("pt_abscissa") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "coordinate-x", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("pt_ordinate") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "coordinate-y", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("segment_length") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "length", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("circle_perimeter") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "circumference", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("arc_length") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "arc-length", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("distance_2pts") )
      {
        if ( parents.size() != 2 ) KIG_FILTER_PARSE_ERROR;
        ObjectTypeCalcer* so = new ObjectTypeCalcer( SegmentABType::instance(), parents );
        so->calc( *ret );
        oc = filtersConstructTextObject( m, so, "length", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("vector_norm") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "length", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("vector_abscissa") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "length-x", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("vector_ordinate") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "length-y", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("slope") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "slope", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("distance_pt_line") )
      {
        if ( parents.size() != 2 ) KIG_FILTER_PARSE_ERROR;
        std::vector<ObjectCalcer*> args;
        args.push_back( parents[1] );
        args.push_back( parents[0] );
        ObjectTypeCalcer* po = new ObjectTypeCalcer( LinePerpendLPType::instance(), args );
        po->calc( *ret );
        args.clear();
        args.push_back( parents[1] );
        args.push_back( po );
        ObjectTypeCalcer* io = new ObjectTypeCalcer( LineLineIntersectionType::instance(), args );
        io->calc( *ret );
        args.clear();
        args.push_back( parents[0] );
        args.push_back( io );
        ObjectTypeCalcer* so = new ObjectTypeCalcer( SegmentABType::instance(), args );
        so->calc( *ret );
        oc = filtersConstructTextObject( m, so, "length", *ret, false );
      }
      // types of 'equation'
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("line") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "equation", *ret, false );
      }
      else if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("circle") )
      {
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        oc = filtersConstructTextObject( m, parents[0], "simply-cartesian-equation", *ret, false );
      }
      else
      {
        notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                            "which Kig does not currently support.", domelem.tagName() , 
                            domelem.attribute( QStringLiteral("type") ) ) );
        return 0;
      }
#ifdef DRGEO_DEBUG
      qDebug() << "+++++++++ oc:" << oc;
#endif
    }
    else if ( domelem.tagName() == QLatin1String("angle") )
    {
      if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("3pts") )
      {
        if ( parents.size() != 3 ) KIG_FILTER_PARSE_ERROR;
        oc = new ObjectTypeCalcer( HalfAngleType::instance(), parents );
      }
      else
      {
        notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                            "which Kig does not currently support.", domelem.tagName() , 
                            domelem.attribute( QStringLiteral("type") ) ) );
        return 0;
      }
#ifdef DRGEO_DEBUG
      qDebug() << "+++++++++ oc:" << oc;
#endif
    }
    else if ( domelem.tagName() == QLatin1String("script") )
    {
      QString xs;
      QString ys;
      QString text;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == QLatin1String("x") )
          xs = ce.text();
        else if ( ce.tagName() == QLatin1String("y") )
          ys = ce.text();
        else if ( ce.tagName() == QLatin1String("code") )
          text = ce.text();
      }
      bool ok;
      bool ok2;
      double x = xs.toDouble( &ok );
      double y = ys.toDouble( &ok2 );
      if ( ! ( ok && ok2 ) )
        KIG_FILTER_PARSE_ERROR;
      // since Kig doesn't support Guile scripts, it will write script's text
      // in a label, so the user can freely see the code and make whatever
      // he/she wants
      // possible idea: construct a new script object with the parents of Guile
      // one and the Guile code inserted commented... depends on a better
      // handling of arguments in scripts?
      if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("nitems") )
        oc = fact->labelCalcer( text, Coordinate( x, y ), false, std::vector<ObjectCalcer*>(), *ret );
      else
      {
        notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                            "which Kig does not currently support.", domelem.tagName() , 
                            domelem.attribute( QStringLiteral("type") ) ) );
        return 0;
      }
    }
    else if ( domelem.tagName() == QLatin1String("locus") )
    {
      if ( domelem.attribute( QStringLiteral("type") ) == QLatin1String("None") )
        oc = fact->locusCalcer( parents[0], parents[1] );
      else
      {
        notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                            "which Kig does not currently support.", domelem.tagName() , 
                            domelem.attribute( QStringLiteral("type") ) ) );
        return 0;
      }
#ifdef DRGEO_DEBUG
      qDebug() << "+++++++++ oc:" << oc;
#endif
    }
    else if ( ( domelem.tagName() == QLatin1String("boundingBox") ) ||
              ( domelem.tagName() == QLatin1String("customUI") ) )
    {
      // ignoring these elements, since they are not useful to us...
      nignored++;
    }
    else
    {
#ifdef DRGEO_DEBUG
      qDebug() << ">>>>>>>>> UNKNOWN OBJECT";
#endif
      notSupported( i18n( "This Dr. Geo file contains a \"%1 %2\" object, "
                          "which Kig does not currently support.", domelem.tagName() , 
                          domelem.attribute( QStringLiteral("type") ) ) );
      return 0;
    }
    curid++;
    if ( oc == 0 )
      continue;

// reading color
    QColor co( domelem.attribute( QStringLiteral("color") ) );
    if ( ! co.isValid() )
    {
      if ( domelem.attribute( QStringLiteral("color") ) == QLatin1String("Bordeaux") )
        co.setRgb( 145, 0, 0 );
      else
        co = Qt::blue;
    }
// reading width and style
// Dashed -> the little one
// Normal -> the medium
// Thick  -> the biggest one
    int w = -1;
    Qt::PenStyle s = Qt::SolidLine;
    if ( domelem.tagName() == QLatin1String("point") )
    {
      if ( domelem.attribute( QStringLiteral("thickness") ) == QLatin1String("Normal") )
        w = 7;
      else if ( domelem.attribute( QStringLiteral("thickness") ) == QLatin1String("Thick") )
        w = 9;
    }
    else
    {
      if ( domelem.attribute( QStringLiteral("thickness") ) == QLatin1String("Dashed") )
        s = Qt::DotLine;
      if ( domelem.attribute( QStringLiteral("thickness") ) == QLatin1String("Thick") )
        w = 2;
    }
    QString ps = domelem.attribute( QStringLiteral("style") );
    Kig::PointStyle pointstyle = Kig::pointStyleFromString( ps );
// show this object?
    bool show = ( ( domelem.attribute( QStringLiteral("masked") ) != QLatin1String("True") ) &&
                  ( domelem.attribute( QStringLiteral("masked") ) != QLatin1String("Alway") ) );
// costructing the ObjectDrawer*
    ObjectDrawer* d = new ObjectDrawer( co, w, show, s, pointstyle );
// reading object name
    QString strname = domelem.attribute( QStringLiteral("name") );
    ObjectConstCalcer* name = new ObjectConstCalcer( new StringImp( strname ) );

// creating the ObjectHolder*
    ObjectHolder* o = new ObjectHolder( oc, d, name );
    holders.push_back( o );
// calc()
#ifdef DRGEO_DEBUG
    qDebug() << ">>>>>>>>> calc";
#endif
    holders[curid-1-nignored]->calc( *ret );
#ifdef DRGEO_DEBUG
    qDebug() << "+++++++++ oc: " << oc << " - "
              << oc->imp()->type()->internalName() << endl;
#endif

    if ( domelem.tagName() == QLatin1String("point") )
    {
      if ( !strname.isEmpty() )
      {
        std::vector<ObjectCalcer*> args2;
        args2.push_back( o->nameCalcer() );
        oc2 = fact->attachedLabelCalcer( QStringLiteral( "%1" ), oc,
                 static_cast<const PointImp*>( oc->imp() )->coordinate(),
                 false, args2, *ret );
        co = Qt::black;
      }
    }
    else if ( domelem.tagName() == QLatin1String("angle") )
    {
      oc2 = filtersConstructTextObject(
               static_cast<const PointImp*>( holders[curid-1-nignored]->calcer()->parents()[1]->imp() )->coordinate(),
               holders[curid-1-nignored]->calcer(), "angle-degrees", *ret, false );
    }

    oc = 0;

    if ( oc2 != 0 )
    {
      oc2->calc( *ret );
      ObjectDrawer* d2 = new ObjectDrawer( co );
      ObjectHolder* o2 = new ObjectHolder( oc2, d2 );
      holders2.push_back( o2 );
      oc2 = 0;
    }
  }

  ret->addObjects( holders );
  ret->addObjects( holders2 );
  ret->setGrid( grid );
  ret->setAxes( grid );
  return ret;
}

KigFilterDrgeo* KigFilterDrgeo::instance()
{
  static KigFilterDrgeo f;
  return &f;
}
