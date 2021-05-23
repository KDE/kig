/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>
    SPDX-FileCopyrightText: 2006 Pino Toscano <toscano.pino@tiscali.it>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cabri-filter.h"

#include "cabri-utils.h"

#include "../kig/kig_document.h"
#include "../misc/coordinate.h"
#include "../misc/rect.h"
#include "../objects/arc_type.h"
#include "../objects/angle_type.h"
#include "../objects/bogus_imp.h"
#include "../objects/circle_imp.h"
#include "../objects/circle_type.h"
#include "../objects/conic_types.h"
#include "../objects/curve_imp.h"
#include "../objects/intersection_types.h"
#include "../objects/inversion_type.h"
#include "../objects/line_imp.h"
#include "../objects/line_type.h"
#include "../objects/object_calcer.h"
#include "../objects/object_drawer.h"
#include "../objects/object_factory.h"
#include "../objects/object_holder.h"
#include "../objects/other_imp.h"
#include "../objects/other_type.h"
#include "../objects/point_imp.h"
#include "../objects/point_type.h"
#include "../objects/polygon_imp.h"
#include "../objects/polygon_type.h"
#include "../objects/transform_types.h"
#include "../objects/vector_type.h"

#include <QColor>
#include <QFile>
#include <QRegExp>

#include <QDebug>

#include <iterator>

static ObjectTypeCalcer* intersectionPoints( const std::vector<ObjectCalcer*>& parents, int which )
{
  if ( parents.size() != 2 ) return 0;
  int nlines = 0;
  int ncircles = 0;
  int nconics = 0;
  int narcs = 0;
  for ( int i = 0; i < 2; ++i )
  {
    if ( parents[i]->imp()->inherits( AbstractLineImp::stype() ) ) ++nlines;
    else if ( parents[i]->imp()->inherits( CircleImp::stype() ) ) { ++ncircles; ++nconics; }
    else if ( parents[i]->imp()->inherits( ConicImp::stype() ) ) ++nconics;
    else if ( parents[i]->imp()->inherits( ArcImp::stype() ) ) ++narcs;
    else return 0;
  };
#ifdef CABRI_DEBUG
  qDebug() << "which: " << which;
#endif
  if ( nlines == 2 )
    return which == -1 ? new ObjectTypeCalcer( LineLineIntersectionType::instance(), parents ) : 0;
  else if ( nlines == 1 && nconics == 1 )
  {
    std::vector<ObjectCalcer*> intparents( parents );
    intparents.push_back( new ObjectConstCalcer( new IntImp( which ) ) );
    return new ObjectTypeCalcer( ConicLineIntersectionType::instance(), intparents );
  }
/*
  else if ( ncircles == 2 )
  {
    std::vector<ObjectCalcer*> intparents( parents );
    intparents.push_back( new ObjectConstCalcer( new IntImp( which ) ) );
    return new ObjectTypeCalcer( CircleCircleIntersectionType::instance(), intparents );
  }
*/
  else if ( nlines == 0 && nconics == 2 )
  {
    std::vector<ObjectCalcer*> rparents( parents );
    rparents.push_back( new ObjectConstCalcer( new IntImp( 1 ) ) );
    rparents.push_back( new ObjectConstCalcer( new IntImp( 1 ) ) );
    rparents.push_back( new ObjectTypeCalcer( ConicRadicalType::instance(), rparents ) );
    std::vector<ObjectCalcer*> iparents;
    iparents.push_back( parents[0] );
    iparents.push_back( rparents.back() );
    iparents.push_back( new ObjectConstCalcer( new IntImp( which ) ) );
    return new ObjectTypeCalcer( ConicLineIntersectionType::instance(), iparents );
  }
  else if ( nlines == 1 && narcs == 1 )
  {
    std::vector<ObjectCalcer*> intparents( parents );
    intparents.push_back( new ObjectConstCalcer( new IntImp( which ) ) );
    return new ObjectTypeCalcer( ArcLineIntersectionType::instance(), intparents );
  }
  else return 0;
}

static int priorTo( const std::vector<uint>& ids, uint id )
{
  int res = 0;
  for ( std::vector<uint>::const_iterator it = ids.begin(); it != ids.end(); ++it )
    if ( id > *it ) ++res;
  return res;
}

KigFilterCabri::KigFilterCabri()
{
}

KigFilterCabri::~KigFilterCabri()
{
}

bool KigFilterCabri::supportMime( const QString& mime )
{
  // ugly hack to avoid duplicate extension ( XFig and Cabri files
  // have the same .fig extension ).
  return ( mime == QLatin1String("image/x-xfig") ) ||
         ( mime == QLatin1String("application/x-cabri") );
}

KigDocument* KigFilterCabri::load( const QString& file )
{
  QFile f( file );
  if ( ! f.open( QIODevice::ReadOnly ) )
  {
    fileNotFound( file );
    return 0;
  }

  QString s = CabriNS::readLine( f );
  QRegExp header( "Figure Cabri\\s?II (Plus )?vers\\. (MS-Windows|DOS) ((\\d+)\\.(\\d+)(\\.(\\d+))?(\\.(\\d+))?|(\\d+)\\.x \\((\\d+)\\.(\\d+)\\))(, ([^,]+), t=(\\d+)s)?" );
  header.setCaseSensitivity( Qt::CaseInsensitive );
  if ( !header.exactMatch( s ) )
  {
    if ( s.startsWith( QLatin1String( "#FIG " ) ) )
    {
      notSupported( i18n( "This is an XFig file, not a Cabri figure." ) );
      return 0;
    }
    else
      KIG_FILTER_PARSE_ERROR;
  }

  bool ok = true;
  QString majorstr = header.cap( 4 );
  if ( majorstr.isEmpty() )
    majorstr = header.cap( 11 );
  int major = majorstr.toInt( &ok );
  bool ok2 = true;
  QString minorstr = header.cap( 5 );
  if ( minorstr.isEmpty() )
    minorstr = header.cap( 12 );
  int minor = minorstr.toInt( &ok2 );
  if ( !ok || !ok2 )
    KIG_FILTER_PARSE_ERROR;

  CabriNS::CabriVersion curVer;
  CabriReader* reader = 0;

#ifdef CABRI_DEBUG
  qDebug() << ">>>>>>>>> version: " << header.cap( 3 );
  if ( !header.cap( 13 ).isEmpty() )
  {
    qDebug() << ">>>>>>>>> session file:";
    qDebug() << ">>>>>>>>>  -> time = " << header.cap( 15 ) << " sec";
    qDebug() << ">>>>>>>>>  -> action = \"" << header.cap( 14 ) << "\"";
  }
#endif
  if ( ( major == 1 ) && ( minor == 0 ) )
  {
    curVer = CabriNS::CV_1_0;
    reader = new CabriReader_v10( this );
  }
  else if ( ( major == 1 ) && ( minor == 2 || minor == 4 ) )
  {
    curVer = CabriNS::CV_1_2;
    reader = new CabriReader_v12( this );
  }
  else
  {
    notSupported( i18n( "This Cabri version (%1) is not supported yet.\n"
                        "Please contact the Kig authors to help supporting this Cabri "
                        "version.", header.cap( 3 ) ) );
    return 0;
  }

  s = CabriNS::readLine( f );
  if ( !reader->readWindowMetrics( f ) )
    return 0;

  // reading center point
  CabriObject* center_point = reader->readObject( f );
  if ( !center_point )
    return 0;
  // reading axes
  CabriObject* axes = reader->readObject( f );
  if ( !axes )
    return 0;
  bool haveaxes = axes->visible;
  bool havegrid = false;

  KigDocument* ret = new KigDocument();

  std::vector<ObjectHolder*> holders;
  std::vector<ObjectCalcer*> calcers;

  const ObjectFactory* fact = ObjectFactory::instance();

  std::vector<ObjectCalcer*> args;
  ObjectCalcer* oc = 0;
  std::vector<uint> idsToSkip;
  int previd = 0;
  uint lowerbound = 2;

  while ( ! f.atEnd() )
  {
#ifdef CABRI_DEBUG
    qDebug() << ">>>>>>>>> -------------------------------";
#endif
    // we do one object each iteration..
    CabriObject* obj = reader->readObject( f );
    if ( !obj )
      return 0;

#ifdef CABRI_DEBUG
    qDebug() << "+++++ " << obj->id << " - " << obj->type;
#endif

    if ( obj->type == "Grid" )
    {
      havegrid = obj->visible;
      idsToSkip.push_back( obj->id );
      if ( obj->id == lowerbound + 1 )
        ++lowerbound;
      continue;
    }
    else if ( obj->type == "Int" )
    {
      int ids = obj->id - previd - 1;
      for ( int i = 1; i <= ids; ++i )
        idsToSkip.push_back( obj->id - i );
    }

    args.clear();
    for ( std::vector<int>::iterator i = obj->parents.begin();
          i != obj->parents.end(); ++i )
      if ( *i <= static_cast<int>( lowerbound ) )
      {
        if ( obj->type == "Pt/" )
          obj->type = "Pt";
#ifdef CABRI_DEBUG
        else
        {
          qDebug() << ">>>>>>>>> OBJECT TO AXIS/GRID: \"" << obj->type << "\"";
        }
#endif
      }
      else
      {
        args.push_back( calcers[*i-3-priorTo( idsToSkip, *i )] );
      }

    if ( obj->id - priorTo( idsToSkip, obj->id ) != calcers.size() + 3 ) KIG_FILTER_PARSE_ERROR;
    oc = 0;
#ifdef CABRI_DEBUG
    if ( args.size() > 0 )
      for ( uint j = 0; j < args.size(); ++j )
      {
        qDebug() << "+++++++++ arg[" << j << "]: " << args[j] << " - "
                  << args[j]->imp()->type()->internalName() << endl;
      }
    else
      qDebug() << "+++++++++ args: NO";
    qDebug() << "+++++++++ data: " << obj->data.size();
#endif
    if ( obj->type == "Pt" )
    {
      if ( !args.empty() || obj->data.size() != 2 ) KIG_FILTER_PARSE_ERROR;
      oc = fact->fixedPointCalcer( Coordinate( obj->data[0], obj->data[1] ) );
    }
    else if ( obj->type == "Cir" )
    {
      if ( args.size() == 1 )
      {
        if ( obj->data.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        ObjectConstCalcer* radc =
          new ObjectConstCalcer( new DoubleImp( obj->data[0] ) );
        args.push_back( radc );
        oc = new ObjectTypeCalcer( CircleBPRType::instance(), args );
      }
      else if ( args.size() == 2 )
      {
        if ( ! obj->data.empty() ) KIG_FILTER_PARSE_ERROR;
        oc = new ObjectTypeCalcer( CircleBCPType::instance(), args );
      }
      else KIG_FILTER_PARSE_ERROR;
    }
    else if ( obj->type == "Line" || obj->type == "Ray" || obj->type == "Seg" ||
              obj->type == "Vec" )
    {
      if ( args.size() == 1 )
      {
        if ( obj->data.size() != 2 ) KIG_FILTER_PARSE_ERROR;
        Coordinate vect( obj->data[0], obj->data[1] );
        ObjectConstCalcer* vectorcalcer =
          new ObjectConstCalcer( new VectorImp( Coordinate( 0, 0 ), vect ) );
        args.push_back( vectorcalcer );
        ObjectTypeCalcer* secondpoint =
          new ObjectTypeCalcer( TranslatedType::instance(), args );
        secondpoint->calc( *ret );
        args[1] = secondpoint;
      }
      if ( args.size() != 2 ) KIG_FILTER_PARSE_ERROR;
      const ObjectType* t = 0;
      if ( obj->type == "Line" ) t = LineABType::instance();
      else if ( obj->type == "Ray" ) t = RayABType::instance();
      else if ( obj->type == "Seg" ) t = SegmentABType::instance();
      else if ( obj->type == "Vec" ) t = VectorType::instance();
      else assert( t );
      oc = new ObjectTypeCalcer( t, args );
    }
    else if ( obj->type == "Pt/" )
    {
      if ( args.size() != 1 || obj->data.size() != 2 )
        KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      if ( !parent->imp()->inherits( CurveImp::stype() ) )
        KIG_FILTER_PARSE_ERROR;
      oc = fact->constrainedPointCalcer( parent, Coordinate( obj->data[0], obj->data[1] ), *ret );
    }
    else if ( obj->type == "Int" && curVer == CabriNS::CV_1_2 ) // Cabri1.2
    {
      if ( args.size() != 2 || !obj->data.empty() ) KIG_FILTER_PARSE_ERROR;
      oc = intersectionPoints( args, obj->intersectionId );
    }
    else if ( obj->type == "Perp" || obj->type == "Par" )
    {
      if ( args.size() == 2
           && ( args[1]->imp()->inherits( FilledPolygonImp::stype() )
             || args[1]->imp()->inherits( FilledPolygonImp::stype3() )
             || args[1]->imp()->inherits( FilledPolygonImp::stype4() ) ) )
      {
        std::vector<ObjectCalcer*> sideargs;
        sideargs.push_back( args[1] );
        sideargs.push_back( new ObjectConstCalcer( new IntImp( obj->side ) ) );
        ObjectCalcer* c =
          new ObjectTypeCalcer( PolygonSideType::instance(), sideargs );
        c->calc( *ret );
        args.pop_back();
        args.push_back( c );
      }
      else if ( args.size() == 2 && args[1]->imp()->inherits( VectorImp::stype() ) )
      {
        ObjectCalcer* c =
          new ObjectTypeCalcer( LineByVectorType::instance(), args );
        c->calc( *ret );
        args.pop_back();
        args.push_back( c );
      }
      if ( args.size() != 2 || obj->data.size() != 0 )
        KIG_FILTER_PARSE_ERROR;
      const ObjectType* t = 0;
      if ( obj->type == "Perp" ) t = LinePerpendLPType::instance();
      else if ( obj->type == "Par" ) t = LineParallelLPType::instance();
      else assert( false );
      oc = new ObjectTypeCalcer( t, args );
    }
    else if ( obj->type == "Arc" )
    {
      if ( args.size() != 3 || ! obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( ArcBTPType::instance(), args );
    }
    else if ( obj->type == "Con" )
    {
      if ( args.size() != 5 || !obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( ConicB5PType::instance(), args );
    }
    else if ( obj->type == "Mid" )
    {
      if ( args.size() == 2 )
      {
        ObjectCalcer* c =
          new ObjectTypeCalcer( SegmentABType::instance(), args );
        c->calc( *ret );
        args.clear();
        args.push_back( c );
      }
      if ( args.size() == 1
           && ( args[0]->imp()->inherits( FilledPolygonImp::stype() )
             || args[0]->imp()->inherits( FilledPolygonImp::stype3() )
             || args[0]->imp()->inherits( FilledPolygonImp::stype4() ) ) )
      {
        std::vector<ObjectCalcer*> sideargs;
        sideargs.push_back( args[0] );
        sideargs.push_back( new ObjectConstCalcer( new IntImp( obj->side ) ) );
        ObjectCalcer* c =
          new ObjectTypeCalcer( PolygonSideType::instance(), sideargs );
        c->calc( *ret );
        args.clear();
        args.push_back( c );
      }
      // midpoint -> this can be the midpoint of a segment, two
      // points, or a vector...
      if ( args.size() != 1 || !obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      if ( parent->imp()->inherits( SegmentImp::stype() ) )
        oc = fact->propertyObjectCalcer( parent, "mid-point" ) ;
      else if ( parent->imp()->inherits( VectorImp::stype() ) )
        oc = fact->propertyObjectCalcer( parent, "vect-mid-point" );
      else KIG_FILTER_PARSE_ERROR;
    }
    else if ( obj->type == "PBiss" )
    {
      if ( args.size() == 2 )
      {
        ObjectCalcer* c =
          new ObjectTypeCalcer( SegmentABType::instance(), args );
        c->calc( *ret );
        args.clear();
        args.push_back( c );
      }
      if ( args.size() == 1
           && ( args[0]->imp()->inherits( FilledPolygonImp::stype() )
             || args[0]->imp()->inherits( FilledPolygonImp::stype3() )
             || args[0]->imp()->inherits( FilledPolygonImp::stype4() ) ) )
      {
        std::vector<ObjectCalcer*> sideargs;
        sideargs.push_back( args[0] );
        sideargs.push_back( new ObjectConstCalcer( new IntImp( obj->side ) ) );
        ObjectCalcer* c =
          new ObjectTypeCalcer( PolygonSideType::instance(), sideargs );
        c->calc( *ret );
        args.clear();
        args.push_back( c );
      }
      if ( args.size() != 1 || !obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      ObjectCalcer* midpoint = 0;
      if ( parent->imp()->inherits( SegmentImp::stype() ) )
        midpoint = fact->propertyObjectCalcer( parent, "mid-point" ) ;
//       else if ( parent->imp()->inherits( VectorImp::stype() ) )
//         midpoint = fact->propertyObjectCalcer( parent, "vect-mid-point" );
      else KIG_FILTER_PARSE_ERROR;
      midpoint->calc( *ret );
      args.push_back( midpoint );
      oc = new ObjectTypeCalcer( LinePerpendLPType::instance(), args );
    }
    else if ( obj->type == "Biss" && curVer == CabriNS::CV_1_2 ) // Cabri1.2
    {
      if ( args.size() == 3 )
      {
        ObjectTypeCalcer* ao = new ObjectTypeCalcer( AngleType::instance(), args );
        ao->calc( *ret );
        args.clear();
        args.push_back( ao );
      };
      if ( args.size() != 1 || !obj->data.empty() ) KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      if ( parent->imp()->inherits( AngleImp::stype() ) )
        oc = fact->propertyObjectCalcer( parent, "angle-bisector" ) ;
      else KIG_FILTER_PARSE_ERROR;
    }
    else if ( obj->type == "Angle" )
    {
      if ( args.size() != 3 ) KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( AngleType::instance(), args );
    }
    else if ( obj->type == "Text" )
    {
//      if ( !args.empty() /* || obj->data.size() != 2 */ ) KIG_FILTER_PARSE_ERROR;
      if ( !obj->textRect.valid() ) KIG_FILTER_PARSE_ERROR;
      // replacing "# with %1, %2, etc.. in the label text
      QString txt = obj->text;
      int pos = -1;
      int index = 1;
      while ( ( pos = txt.indexOf( QLatin1String("\"#") ) ) > -1 )
        txt.replace( pos, 2, QStringLiteral( "%%1" ).arg( index++ ) );
      // getting the references to the arguments
      std::vector<ObjectCalcer*> txtincs;
      for ( std::vector<int>::iterator i = obj->incs.begin();
            i != obj->incs.end(); ++i )
        txtincs.push_back( calcers[*i-3-priorTo( idsToSkip, *i )] );
      oc = fact->labelCalcer( txt, obj->textRect.topLeft(), true, txtincs, *ret );
    }
    else if ( obj->type == "Pol" )
    {
      if ( args.size() < 3 || !obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( PolygonBNPType::instance(), args );
    }
    else if ( obj->type == "Lgth" )
    {
      if ( args.size() == 2 )
      {
        ObjectCalcer* c =
          new ObjectTypeCalcer( SegmentABType::instance(), args );
        c->calc( *ret );
        args.clear();
        args.push_back( c );
      }
      if ( args.size() != 1 /* || !obj->data.empty() */ )
        KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      QByteArray prop;
      if ( ( parent->imp()->inherits( SegmentImp::stype() ) ) ||
           ( parent->imp()->inherits( VectorImp::stype() ) ) )
        prop = "length";
      else if ( parent->imp()->inherits( ArcImp::stype() ) )
        prop = "arc-length";
      else if ( parent->imp()->inherits( CircleImp::stype() ) )
        prop = "circumference";
      else
        KIG_FILTER_PARSE_ERROR;
      oc = fact->propertyObjectCalcer( parent, prop );
    }
    else if ( obj->type == "Slo" )
    {
      if ( args.size() != 1 /* || !obj->data.empty() */ )
        KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      if ( !parent->imp()->inherits( AbstractLineImp::stype() ) )
        KIG_FILTER_PARSE_ERROR;
      oc = fact->propertyObjectCalcer( parent, "slope" );
    }
    else if ( obj->type == "AngVal" )
    {
      if ( args.size() == 3 )
      {
        ObjectTypeCalcer* angle = new ObjectTypeCalcer( AngleType::instance(), args );
        angle->calc( *ret );
        args.clear();
        args.push_back( angle );
      }
      if ( args.size() != 1 /* || !obj->data.empty() */ )
        KIG_FILTER_PARSE_ERROR;
      QByteArray prop;
      switch ( obj->gonio )
      {
        case CabriNS::CG_Deg: prop = "angle-degrees"; break;
        case CabriNS::CG_Rad: prop = "angle-radian"; break;
        default:
          KIG_FILTER_PARSE_ERROR;
      }
      oc = fact->propertyObjectCalcer( args[0], prop );
    }
    else if ( obj->type == "Eq/Co" && curVer == CabriNS::CV_1_0 )
    {
      if ( args.size() < 1 )
        KIG_FILTER_PARSE_ERROR;

      ObjectCalcer* thisarg = args[0];
      QByteArray prop;
      if ( thisarg->imp()->inherits( PointImp::stype() ) )
      {
        switch ( obj->specification )
        {
          case 0:
            prop = "coordinate-x";
            break;
          case 1:
            prop = "coordinate-y";
            break;
          default:
            KIG_FILTER_PARSE_ERROR;
        }
      }
      else
        KIG_FILTER_PARSE_ERROR;

      assert( !prop.isEmpty() );

      oc = fact->propertyObjectCalcer( thisarg, prop );
    }
    else if ( obj->type == "Tr" )
    {
      if ( args.size() != 3 || !obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( PolygonBNPType::instance(), args );
    }
    else if ( obj->type == "Locus" )
    {
      if ( args.size() != 2 || !obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = fact->locusCalcer( args[0], args[1] );
    }
    else if ( obj->type == "Refl" )
    {
      if ( args.size() != 2 || !obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( LineReflectionType::instance(), args );
    }
    else if ( obj->type == "Sym" )
    {
      if ( args.size() != 2 || !obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( PointReflectionType::instance(), args );
    }
    else if ( obj->type == "Tran" )
    {
      if ( args.size() != 2 || !obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( TranslatedType::instance(), args );
    }
    else if ( obj->type == "Rot" )
    {
      if ( args.size() == 5 )
      {
        std::vector<ObjectCalcer*> angleargs;
        std::copy( args.begin() + 2, args.end(), std::back_inserter( angleargs ) );
        ObjectTypeCalcer* angle = new ObjectTypeCalcer( AngleType::instance(), angleargs );
        angle->calc( *ret );
        args.pop_back();
        args.pop_back();
        args.pop_back();
        args.push_back( angle );
      }
      if ( args.size() == 3 )
      {
        if ( ! obj->data.empty() ) KIG_FILTER_PARSE_ERROR;
        oc = new ObjectTypeCalcer( RotationType::instance(), args );
      }
      else KIG_FILTER_PARSE_ERROR;
    }
    else if ( obj->type == "Inv" )
    {
      if ( args.size() != 2 || !obj->data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( InvertPointType::instance(), args );
    }
    else
    {
      notSupported( i18n( "This Cabri file contains a \"%1\" object, "
                          "which Kig does not currently support.", QString( obj->type ) ) );
      return 0;
    }
#ifdef CABRI_DEBUG
    qDebug() << "+++++++++ oc: " << oc;
#endif

    if ( oc == 0 ) KIG_FILTER_PARSE_ERROR;

    oc->calc( *ret );
#ifdef CABRI_DEBUG
    qDebug() << "+++++++++ oc: " << oc << " - "
              << oc->imp()->type()->internalName() << endl;
#endif
    calcers.push_back( oc );

    // decoding the style and building the ObjectDrawer
    Qt::PenStyle ls = Qt::SolidLine;
    Kig::PointStyle ps = Kig::Round;
    reader->decodeStyle( obj, ls, ps );
    ObjectDrawer* d = new ObjectDrawer( obj->color, obj->thick, obj->visible, ls, ps );

    ObjectHolder* oh = 0;
    if ( !obj->name.isEmpty() )
    {
      ObjectConstCalcer* name = new ObjectConstCalcer( new StringImp( obj->name ) );
      oh = new ObjectHolder( oc, d, name );
    }
    else
    {
      oh = new ObjectHolder( oc, d );
    }
    assert( oh );
    holders.push_back( oh );

    oc = 0;
    previd = obj->id;
    delete obj;
  }
  // freeing memory
  delete center_point;
  delete axes;
  delete reader;

  ret->addObjects( holders );
  ret->setGrid( havegrid );
  ret->setAxes( haveaxes );
  return ret;
}

KigFilterCabri* KigFilterCabri::instance()
{
  static KigFilterCabri t;
  return &t;
}
