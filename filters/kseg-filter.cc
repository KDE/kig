// kseg-filter.cc
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

#include "kseg-filter.h"

#include "kseg-defs.h"

#include "../kig/kig_part.h"
#include "../objects/object.h"
#include "../objects/bogus_imp.h"
#include "../objects/object_factory.h"
#include "../objects/line_type.h"
#include "../objects/circle_type.h"
#include "../objects/conic_types.h"
#include "../objects/other_type.h"
#include "../objects/point_type.h"
#include "../objects/transform_types.h"
#include "../objects/intersection_types.h"
#include "../misc/coordinate.h"

#include <qfont.h>
#include <qpen.h>
#include <qbrush.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qbuffer.h>

KigFilterKSeg::KigFilterKSeg()
{
}

KigFilterKSeg::~KigFilterKSeg()
{
}

bool KigFilterKSeg::supportMime( const QString& mime )
{
  return mime == "application/x-kseg";
}

struct drawstyle
{
  Q_INT8 pointstyle;
  QFont font;
  QPen pen;
  QBrush brush;
};

static Coordinate readKSegCoordinate( QDataStream& stream )
{
  // read the coord..
  float inx, iny;
  stream >> inx >> iny;
  // KSeg uses a coordinate system, where the topleft is (0,0), and
  // the bottom right is the widget coordinate in the window: if the
  // KSeg window had a width of 600 pixels and a height of 600, then
  // the bottom right will be at (600,600).  We assume a window of
  // such a height here, and transform it into Kig Coordinates.  This
  // function is quite similar to ScreenInfo::fromScreen, and it's
  // basically a simple modification of that code to floats..

  // invert the y-axis: 0 is at the bottom !
  Coordinate t( inx, 600 - iny );
  t *= 14;
  t /= 600;
  return t + Coordinate( -7, -7 );
};

static RealObject* intersectionPoint( const Objects& parents, Objects& ret, int which )
{
  if ( parents.size() != 2 ) return 0;
  int nlines = 0;
  int nconics = 0;
  for ( int i = 0; i < 2; ++i )
  {
    if ( parents[i]->hasimp( ObjectImp::ID_LineImp ) ) ++nlines;
    else if ( parents[i]->hasimp( ObjectImp::ID_ConicImp ) ) ++nconics;
    else return 0;
  };
  if ( nlines == 2 )
    return which == 1 ? new RealObject( LineLineIntersectionType::instance(), parents ) : 0;
  else if ( nlines == 1 && nconics == 1 )
  {
    Objects intparents( parents );
    ret.push_back( new DataObject( new IntImp( which ) ) );
    intparents.push_back( ret.back() );
    return new RealObject( ConicLineIntersectionType::instance(), intparents );
  }
  else if ( nlines == 0 && nconics == 2 )
  {
    Objects rparents( parents );
    ret.push_back( new DataObject( new IntImp( 1 ) ) );
    rparents.push_back( ret.back() );
    ret.push_back( new DataObject( new IntImp( 1 ) ) );
    rparents.push_back( ret.back() );
    ret.push_back( new RealObject( ConicRadicalType::instance(), rparents ) );
    ret.back()->setShown( false );
    Objects iparents;
    iparents.push_back( parents[0] );
    iparents.push_back( ret.back() );
    ret.push_back( new DataObject( new IntImp( which ) ) );
    iparents.push_back( ret.back() );
    return new RealObject( ConicLineIntersectionType::instance(), iparents );
  }
  else return 0;
};

bool KigFilterKSeg::load( const QString& file, KigDocument& todoc )
{
  QFile ffile( file );
  if ( ! ffile.open( IO_ReadOnly ) )
  {
    fileNotFound( file );
    return false;
  };
  QDataStream fstream( &ffile );

  QString versionstring;
  fstream >> versionstring;
  if ( !versionstring.startsWith( "KSeg Document Version " ) )
    KIG_FILTER_PARSE_ERROR;

  QByteArray array;
  fstream >> array;
  QBuffer buf( array );
  buf.open( IO_ReadOnly );
  QDataStream stream( &buf );

  stream.setVersion( 3 );

  // G_drawstyles:
  short numstyles;
  stream >> numstyles;
  std::vector<drawstyle> drawstyles( numstyles );
  for ( short i = 0; i < numstyles; ++i )
  {
    stream >> drawstyles[i].pointstyle;
    stream >> drawstyles[i].font;
    stream >> drawstyles[i].pen;
    stream >> drawstyles[i].brush;
  };

  Objects ret;

  // G_refs
  unsigned int count;
  stream >> count;

  ret.resize( count, 0 );

  // KSeg topologically sorts the objects before saving, that means we
  // can read the entire file in one iteration..
  for ( uint i = 0; i < count; ++i )
  {
    short styleid;
    stream >> styleid;
    short nparents;
    stream >> nparents;
    Objects parents( nparents, 0 );
    for ( short j = 0; j < nparents; ++j )
    {
      int parent;
      stream >> parent;
      parents[j] = ret[parent];
    };

    // read the object..
    short info;
    stream >> info;
    int type = 1 << (info & 31);
    info >>= 5;
    int descendtype = (info & 15);
    info >>= 4;
    bool visible = info & 1;
    bool labelVisible = info & 2;
    bool given = info & 4;
    bool final = info & 8;

    // avoid g++ warnings about unused vars..
    // this doesn't really do anything..
    (void) visible;
    (void) labelVisible;
    (void) given;
    (void) final;

    drawstyle style = drawstyles[styleid];

    if ( type == G_LOOP ) continue;
    // read the label..
    QString labeltext;
    stream >> labeltext;
    Coordinate relcoord = readKSegCoordinate( stream );
    // shut up gcc
    (void) relcoord;
    if ( type & G_CURVE )
    {
      Coordinate relcurvecoord = readKSegCoordinate( stream );
      // shut up gcc
      (void) relcurvecoord;
    };

    // now load the object data..
    Object* object = 0;

    if ( descendtype <= G_REFLECTED )
    {
      RealObject* o = 0;
      // this is a transformed object..
      switch( descendtype )
      {
      case G_TRANSLATED:
      {
        Objects vectorparents( parents.begin() + 1, parents.end() );
        RealObject* vector = new RealObject( VectorType::instance(), vectorparents );
        vector->setShown( false );
        ret.push_back( vector );
        vector->calc( todoc );

        Objects transparents;
        transparents.push_back( parents[0] );
        transparents.push_back( vector );
        o = new RealObject( TranslatedType::instance(), transparents );
        break;
      }
      case G_ROTATED:
      {
        Objects angleparents( parents.begin() + 2, parents.end() );
        RealObject* angle = new RealObject( AngleType::instance(), angleparents );
        angle->setShown( false );
        ret.push_back( angle );
        angle->calc( todoc );

        Objects rotparents;
        rotparents.push_back( parents[1] );
        rotparents.push_back( parents[0] );
        rotparents.push_back( angle );
        o = new RealObject( RotationType::instance(), rotparents );
        break;
      }
      case G_SCALED:
      {
        // TODO
        notSupported( file, i18n( "This KSeg document uses a scaling "
                                  "transformation, which Kig currently "
                                  "cannot import." ) );
        return false;
        o = new RealObject( ScalingOverCenterType::instance(), parents );
        break;
      }
      case G_REFLECTED:
      {
        o = new RealObject( LineReflectionType::instance(), parents );
        break;
      }
      };
      assert( o );
      if ( type == G_POINT )
      {
        o->setWidth( style.pointstyle == SMALL_CIRCLE ? 2 :
                     style.pointstyle == MEDIUM_CIRCLE ? 3 : 5 );
        o->setColor( style.brush.color() );
      }
      else
      {
        o->setWidth( style.pen.width() );
        o->setColor( style.pen.color() );
      };
      object = o;
    }
    else
      switch( type )
      {
      case G_POINT:
      {
        RealObject* point = 0;
        switch( descendtype )
        {
        case G_FREE_POINT:
        {
          // fixed point
          if ( nparents != 0 ) KIG_FILTER_PARSE_ERROR;
          Coordinate c = readKSegCoordinate( stream );
          Objects os = ObjectFactory::instance()->fixedPoint( c );
          point = static_cast<RealObject*>( os[2] );
          copy( os.begin(), os.begin() + 2, back_inserter( ret ) );
          break;
        }
        case G_CONSTRAINED_POINT:
        {
          // constrained point
          double p;
          stream >> p;
          assert( nparents == 1 );
          Object* parent = parents[0];
          assert( parent );
          Objects os = ObjectFactory::instance()->constrainedPoint( parent, p );
          point = static_cast<RealObject*>( os[1] );
          ret.push_back( os[0] );
          break;
        }
        case G_INTERSECTION_POINT:
        {
          // KSeg has somewhat weird intersection objects..
          // for all objects G_INTERSECTION_POINT gets the
          // first intersection of its parents, G_INTERSECTION2_POINT
          // represents the second, if present.
          point = intersectionPoint( parents, ret, 1 );
          if ( ! point ) KIG_FILTER_PARSE_ERROR;
          break;
        }
        case G_INTERSECTION2_POINT:
          point = intersectionPoint( parents, ret, -1 );
          if ( ! point ) KIG_FILTER_PARSE_ERROR;
          break;
        case G_MID_POINT:
        {
          // midpoint of a segment..
          if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
          if ( !parents[0]->hasimp( ObjectImp::ID_SegmentImp ) )
            KIG_FILTER_PARSE_ERROR;
          int index = parents[0]->propertiesInternalNames().findIndex( "mid-point" );
          assert( index != -1 );
          ret.push_back( new PropertyObject( parents[0], index ) );
          Objects nparents( ret.end() - 1, ret.end() );
          ret.back()->calc( todoc );
          point = new RealObject( CopyObjectType::instance(), nparents );
          break;
        }
        default:
          KIG_FILTER_PARSE_ERROR;
        };
        assert( point );
        point->setWidth( style.pointstyle == SMALL_CIRCLE ? 2 :
                         style.pointstyle == MEDIUM_CIRCLE ? 3 : 5 );
        point->setColor( style.brush.color() );
        object = point;
        break;
      };
      case G_SEGMENT:
      {
        switch( descendtype )
        {
        case G_ENDPOINTS_SEGMENT:
        {
          if ( nparents != 2 ) KIG_FILTER_PARSE_ERROR;
          RealObject* o = new RealObject( SegmentABType::instance(), parents );
          o->setWidth( style.pen.width() );
          o->setColor( style.pen.color() );
          object = o;
          break;
        }
        default:
          KIG_FILTER_PARSE_ERROR;
        };
        break;
      };
      case G_RAY:
      {
        switch( descendtype )
        {
        case G_TWOPOINTS_RAY:
        {
          if ( nparents != 2 ) KIG_FILTER_PARSE_ERROR;
          RealObject* o = new RealObject( RayABType::instance(), parents );
          o->setWidth( style.pen.width() );
          o->setColor( style.pen.color() );
          object = o;
          break;
        }
        case G_BISECTOR_RAY:
          return false;
          // TODO
//           if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
//           if ( !parents[0]->hasimp( ObjectImp::ID_AngleImp ) ) KIG_FILTER_PARSE_ERROR;
//           int index = parents[0]->propertiesInternalNames().findIndex( "mid-point" );
//           assert( index != -1 );
//           ret.push_back( new PropertyObject( parents[0], index ) );
//           Objects nparents( ret.end() - 1, ret.end() );
//           ret.back()->calc( todoc );
//           point = new RealObject( CopyObjectType::instance(), nparents );
          break;
        default:
          KIG_FILTER_PARSE_ERROR;
        };
        break;
      };
      case G_LINE:
      {
        if ( nparents != 2 ) KIG_FILTER_PARSE_ERROR;
        RealObject* o = 0;
        switch( descendtype )
        {
        case G_TWOPOINTS_LINE:
          o = new RealObject( LineABType::instance(), parents );
          break;
        case G_PARALLEL_LINE:
          o = new RealObject( LineParallelLPType::instance(), parents );
          break;
        case G_PERPENDICULAR_LINE:
          o = new RealObject( LinePerpendLPType::instance(), parents );
          break;
        default:
          KIG_FILTER_PARSE_ERROR;
        };
        assert( o );
        o->setWidth( style.pen.width() );
        o->setColor( style.pen.color() );
        object = o;
        break;
      };
      case G_CIRCLE:
      {
        switch( descendtype )
        {
        case G_CENTERPOINT_CIRCLE:
        {
          if ( nparents != 2 ) KIG_FILTER_PARSE_ERROR;
          RealObject* o = new RealObject( CircleBCPType::instance(), parents );
          o->setWidth( style.pen.width() );
          o->setColor( style.pen.color() );
          object = o;
          break;
        }
        case G_CENTERRADIUS_CIRCLE:
        {
          Object* point;
          Object* segment;
          if ( parents[0]->hasimp( ObjectImp::ID_PointImp ) )
          {
            point = parents[0];
            segment = parents[1];
          }
          else
          {
            point = parents[1];
            segment = parents[0];
          };
          int index = segment->propertiesInternalNames().findIndex( "length" );
          if ( index == -1 ) KIG_FILTER_PARSE_ERROR;
          Object* length = new PropertyObject( segment, index );
          ret.push_back( length );
          Objects cparents;
          cparents.push_back( length );
          cparents.push_back( point );
          RealObject* o = new RealObject( CircleBPRType::instance(), cparents );
          o->setWidth( style.pen.width() );
          o->setColor( style.pen.color() );
          object = o;
          break;
        }
        default:
          KIG_FILTER_PARSE_ERROR;
        };
        break;
      };
      case G_ARC:
      {
        if ( nparents != 3 ) KIG_FILTER_PARSE_ERROR;
        RealObject* o = new RealObject( ArcBTPType::instance(), parents );
        o->setWidth( style.pen.width() );
        o->setColor( style.pen.color() );
        object = o;
        break;
      };
      case G_POLYGON:
      {
        notSupported( file, i18n( "This KSeg file contains a polygon object, "
                                  "which Kig does not currently support." ) );
        return false;
      };
      case G_CIRCLEINTERIOR:
      {
        notSupported( file, i18n( "This KSeg file contains a filled circle, "
                                  "which Kig does not currently support." ) );
        return false;
      };
      case G_ARCSECTOR:
      {
        notSupported( file, i18n( "This KSeg file contains an arc sector, "
                                  "which Kig does not currently support." ) );
        return false;
      };
      case G_ARCSEGMENT:
      {
        notSupported( file, i18n( "This KSeg file contains an arc segment, "
                                  "which Kig does not currently support." ) );
        return false;
      };
      case G_LOCUS:
      {
        if ( nparents != 2 ) KIG_FILTER_PARSE_ERROR;
        Objects os = ObjectFactory::instance()->locus( parents );
        assert( os.size() == 2 );
        ret.push_back( os[0] );
        object = os.back();
        static_cast<RealObject*>( object )->setWidth( style.pen.width() );
        static_cast<RealObject*>( object )->setColor( style.pen.color() );
        break;
      };
      case G_MEASURE:
        KIG_FILTER_PARSE_ERROR;
      case G_CALCULATE:
        KIG_FILTER_PARSE_ERROR;
      case G_ANNOTATION:
        KIG_FILTER_PARSE_ERROR;
      case G_LOOP:
        KIG_FILTER_PARSE_ERROR;
      };
    assert( object );
    ret[i] = object;
    object->calc( todoc );
  };

  // selection groups ( we ignore them, but we pretend to read them
  // out anyway, so we can find what comes after them.. )
  int selgroupcount;
  stream >> selgroupcount;
  for ( int i = 0; i < selgroupcount; ++i )
  {
    QString name;
    stream >> name;
    int size;
    stream >> size;
    for ( int i = 0; i < size; ++i )
    {
      short object;
      stream >> object;
      (void) object;
    };
  };

  // no more data in the file..
  todoc.setObjects( ret );
  return true;
}

KigFilterKSeg* KigFilterKSeg::instance()
{
  static KigFilterKSeg f;
  return &f;
}
