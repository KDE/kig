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

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../misc/coordinate.h"
#include "../objects/angle_type.h"
#include "../objects/arc_type.h"
#include "../objects/bogus_imp.h"
#include "../objects/circle_type.h"
#include "../objects/conic_imp.h"
#include "../objects/conic_types.h"
#include "../objects/intersection_types.h"
#include "../objects/line_imp.h"
#include "../objects/line_type.h"
#include "../objects/object_calcer.h"
#include "../objects/object_drawer.h"
#include "../objects/object_factory.h"
#include "../objects/object_holder.h"
#include "../objects/other_type.h"
#include "../objects/point_imp.h"
#include "../objects/point_type.h"
#include "../objects/transform_types.h"
#include "../objects/vector_type.h"

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
}

static ObjectTypeCalcer* intersectionPoint( const std::vector<ObjectCalcer*>& parents, int which )
{
  if ( parents.size() != 2 ) return 0;
  int nlines = 0;
  int nconics = 0;
  for ( int i = 0; i < 2; ++i )
  {
    if ( parents[i]->imp()->inherits( AbstractLineImp::stype() ) ) ++nlines;
    else if ( parents[i]->imp()->inherits( ConicImp::stype() ) ) ++nconics;
    else return 0;
  };
  if ( nlines == 2 )
    return which == 1 ? new ObjectTypeCalcer( LineLineIntersectionType::instance(), parents ) : 0;
  else if ( nlines == 1 && nconics == 1 )
  {
    std::vector<ObjectCalcer*> intparents( parents );
    intparents.push_back( new ObjectConstCalcer( new IntImp( which ) ) );
    return new ObjectTypeCalcer( ConicLineIntersectionType::instance(), intparents );
  }
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
  else return 0;
}

KigDocument* KigFilterKSeg::load( const QString& file )
{
  QFile ffile( file );
  if ( ! ffile.open( IO_ReadOnly ) )
  {
    fileNotFound( file );
    return false;
  };

  KigDocument* retdoc = new KigDocument();

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

  std::vector<ObjectHolder*> ret;

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
    std::vector<ObjectCalcer*> parents( nparents, 0 );
    for ( short j = 0; j < nparents; ++j )
    {
      int parent;
      stream >> parent;
      parents[j] = ret[parent]->calcer();
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
    ObjectHolder* object = 0;

    if ( descendtype <= G_REFLECTED )
    {
      ObjectTypeCalcer* o = 0;
      // this is a transformed object..
      switch( descendtype )
      {
      case G_TRANSLATED:
      {
        std::vector<ObjectCalcer*> vectorparents( parents.begin() + 1, parents.end() );
        ObjectTypeCalcer* vector = new ObjectTypeCalcer( VectorType::instance(), vectorparents );
        vector->calc( *retdoc );

        std::vector<ObjectCalcer*> transparents;
        transparents.push_back( parents[0] );
        transparents.push_back( vector );
        o = new ObjectTypeCalcer( TranslatedType::instance(), transparents );
        break;
      }
      case G_ROTATED:
      {
        std::vector<ObjectCalcer*> angleparents( parents.begin() + 2, parents.end() );
        ObjectTypeCalcer* angle = new ObjectTypeCalcer( AngleType::instance(), angleparents );
        angle->calc( *retdoc );

        std::vector<ObjectCalcer*> rotparents;
        rotparents.push_back( parents[0] );
        rotparents.push_back( parents[1] );
        rotparents.push_back( angle );
        o = new ObjectTypeCalcer( RotationType::instance(), rotparents );
        break;
      }
      case G_SCALED:
      {
        // TODO
        notSupported( file, i18n( "This KSeg document uses a scaling "
                                  "transformation, which Kig currently "
                                  "cannot import." ) );
        return false;
        o = new ObjectTypeCalcer( ScalingOverCenterType::instance(), parents );
        break;
      }
      case G_REFLECTED:
      {
        std::vector<ObjectCalcer*> mirparents( parents.begin(), parents.end() );
        o = new ObjectTypeCalcer( LineReflectionType::instance(), mirparents );
        break;
      }
      }
      assert( o );
      ObjectDrawer* d;
      if ( type == G_POINT )
      {
        int width;
        switch( style.pointstyle )
        {
        case SMALL_CIRCLE:
          width = 2;
          break;
        case MEDIUM_CIRCLE:
          width = 3;
          break;
        default:
          width = 5;
          break;
        }
        d = new ObjectDrawer( style.brush.color(), width, visible );
      }
      else
      {
        d = new ObjectDrawer( style.pen.color(), style.pen.width(), visible, style.pen.style() );
      };
      assert( d );
      object = new ObjectHolder( o, d );
    }
    else
      switch( type )
      {
      case G_POINT:
      {
        ObjectCalcer* point = 0;
        switch( descendtype )
        {
        case G_FREE_POINT:
        {
          // fixed point
          if ( nparents != 0 ) KIG_FILTER_PARSE_ERROR;
          Coordinate c = readKSegCoordinate( stream );
          point = ObjectFactory::instance()->fixedPointCalcer( c );
          break;
        }
        case G_CONSTRAINED_POINT:
        {
          // constrained point
          double p;
          stream >> p;
          assert( nparents == 1 );
          ObjectCalcer* parent = parents[0];
          assert( parent );
          point = ObjectFactory::instance()->constrainedPointCalcer( parent, p );
          break;
        }
        case G_INTERSECTION_POINT:
        {
          // KSeg has somewhat weird intersection objects..
          // for all objects G_INTERSECTION_POINT gets the
          // first intersection of its parents, G_INTERSECTION2_POINT
          // represents the second, if present.
          point = intersectionPoint( parents, 1 );
          if ( ! point ) KIG_FILTER_PARSE_ERROR;
          break;
        }
        case G_INTERSECTION2_POINT:
          point = intersectionPoint( parents, -1 );
          if ( ! point ) KIG_FILTER_PARSE_ERROR;
          break;
        case G_MID_POINT:
        {
          // midpoint of a segment..
          if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
          if ( !parents[0]->imp()->inherits( SegmentImp::stype() ) )
            KIG_FILTER_PARSE_ERROR;
          int index = parents[0]->imp()->propertiesInternalNames().findIndex( "mid-point" );
          assert( index != -1 );
          point = new ObjectPropertyCalcer( parents[0], index );
          break;
        }
        default:
          KIG_FILTER_PARSE_ERROR;
        };
        assert( point );
        int width = style.pointstyle == SMALL_CIRCLE ? 2 : style.pointstyle == MEDIUM_CIRCLE ? 3 : 5;
        ObjectDrawer* d =
          new ObjectDrawer( style.brush.color(), width, visible, style.pen.style() );
        object = new ObjectHolder( point, d );
        break;
      };
      case G_SEGMENT:
      {
        switch( descendtype )
        {
        case G_ENDPOINTS_SEGMENT:
        {
          if ( nparents != 2 ) KIG_FILTER_PARSE_ERROR;
          ObjectTypeCalcer* o = new ObjectTypeCalcer( SegmentABType::instance(), parents );
          ObjectDrawer* d = new ObjectDrawer( style.pen.color(), style.pen.width(), visible, style.pen.style() );
          object = new ObjectHolder( o, d );
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
          ObjectTypeCalcer* o = new ObjectTypeCalcer( RayABType::instance(), parents );
          ObjectDrawer* d = new ObjectDrawer( style.pen.color(), style.pen.width(), visible, style.pen.style() );
          object = new ObjectHolder( o, d );
          break;
        }
        case G_BISECTOR_RAY:
          return false;
          // TODO
//           if ( nparents != 1 ) KIG_FILTER_PARSE_ERROR;
//           if ( !parents[0]->imp()->inherits( AngleImp::stype() ) ) KIG_FILTER_PARSE_ERROR;
//           int index = parents[0]->propertiesInternalNames().findIndex( "angle-bisector" );
//           assert( index != -1 );
//           ObjectPropertyCalcer* o = new ObjectPropertyCalcer( parents[0], index );
//           ObjectDrawer* d = new ObjectDrawer( style.pen.color(), style.pen.width(), visible, style.pen.style() );
//           object = new ObjectHolder( o, d );
          break;
        default:
          KIG_FILTER_PARSE_ERROR;
        };
        break;
      };
      case G_LINE:
      {
        if ( nparents != 2 ) KIG_FILTER_PARSE_ERROR;
        ObjectTypeCalcer* o = 0;
        switch( descendtype )
        {
        case G_TWOPOINTS_LINE:
          o = new ObjectTypeCalcer( LineABType::instance(), parents );
          break;
        case G_PARALLEL_LINE:
          o = new ObjectTypeCalcer( LineParallelLPType::instance(), parents );
          break;
        case G_PERPENDICULAR_LINE:
          o = new ObjectTypeCalcer( LinePerpendLPType::instance(), parents );
          break;
        default:
          KIG_FILTER_PARSE_ERROR;
        };
        assert( o );
        ObjectDrawer* d = new ObjectDrawer( style.pen.color(), style.pen.width(), visible, style.pen.style() );
        object = new ObjectHolder( o, d );
        break;
      };
      case G_CIRCLE:
      {
        ObjectTypeCalcer* o = 0;
        switch( descendtype )
        {
        case G_CENTERPOINT_CIRCLE:
        {
          if ( nparents != 2 ) KIG_FILTER_PARSE_ERROR;
          o = new ObjectTypeCalcer( CircleBCPType::instance(), parents );
          break;
        }
        case G_CENTERRADIUS_CIRCLE:
        {
          ObjectCalcer* point;
          ObjectCalcer* segment;
          if ( parents[0]->imp()->inherits( PointImp::stype() ) )
          {
            point = parents[0];
            segment = parents[1];
          }
          else
          {
            point = parents[1];
            segment = parents[0];
          };
          int index = segment->imp()->propertiesInternalNames().findIndex( "length" );
          if ( index == -1 ) KIG_FILTER_PARSE_ERROR;
          ObjectPropertyCalcer* length = new ObjectPropertyCalcer( segment, index );
          std::vector<ObjectCalcer*> cparents;
          cparents.push_back( point );
          cparents.push_back( length );
          o = new ObjectTypeCalcer( CircleBPRType::instance(), cparents );
          break;
        }
        default:
          KIG_FILTER_PARSE_ERROR;
        };
        assert( o );
        ObjectDrawer* d = new ObjectDrawer( style.pen.color(), style.pen.width(), visible, style.pen.style() );
        object = new ObjectHolder( o, d );
        break;
      };
      case G_ARC:
      {
        if ( nparents != 3 ) KIG_FILTER_PARSE_ERROR;
        ObjectTypeCalcer* o = new ObjectTypeCalcer( ArcBTPType::instance(), parents );
        ObjectDrawer* d = new ObjectDrawer( style.pen.color(), style.pen.width(), visible, style.pen.style() );
        object = new ObjectHolder( o, d );
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
        ObjectTypeCalcer* o = ObjectFactory::instance()->locusCalcer( parents[0], parents[1] );
        ObjectDrawer* d = new ObjectDrawer( style.pen.color(), style.pen.width(), visible, style.pen.style() );
        object = new ObjectHolder( o, d );
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
    object->calc( *retdoc );
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
  retdoc->addObjects( ret );
  return retdoc;
}

KigFilterKSeg* KigFilterKSeg::instance()
{
  static KigFilterKSeg f;
  return &f;
}
