/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "kgeo-filter.h"

#include "kgeo-resource.h"
#include "filters-common.h"

#include "../kig/kig_part.h"
#include "../kig/kig_document.h"
#include "../objects/angle_type.h"
#include "../objects/bogus_imp.h"
#include "../objects/circle_imp.h"
#include "../objects/circle_type.h"
#include "../objects/intersection_types.h"
#include "../objects/line_type.h"
#include "../objects/object_calcer.h"
#include "../objects/object_drawer.h"
#include "../objects/object_factory.h"
#include "../objects/object_holder.h"
#include "../objects/other_type.h"
#include "../objects/point_imp.h"
#include "../objects/point_type.h"
#include "../objects/text_type.h"
#include "../objects/transform_types.h"
#include "../objects/vector_type.h"

#include <ksimpleconfig.h>

#include <algorithm>
#include <functional>

bool KigFilterKGeo::supportMime( const QString& mime )
{
  return mime == "application/x-kgeo";
}

KigDocument* KigFilterKGeo::load( const QString& sFrom )
{
  // kgeo uses a KSimpleConfig to save its contents...
  KSimpleConfig config ( sFrom );

  loadMetrics ( &config );
  return loadObjects ( sFrom, &config );
}

void KigFilterKGeo::loadMetrics(KSimpleConfig* c )
{
  c->setGroup("Main");
  xMax = c->readNumEntry("XMax", 16);
  yMax = c->readNumEntry("YMax", 11);
  grid = c->readBoolEntry( "Grid", true );
  axes = c->readBoolEntry( "Axes", true );
  // the rest is not relevant to us (yet ?)...
}

struct KGeoHierarchyElement
{
  int id;
  std::vector<int> parents;
};

static void visitElem( std::vector<KGeoHierarchyElement>& ret,
                       const std::vector<KGeoHierarchyElement>& elems,
                       std::vector<bool>& seen,
                       int i )
{
  if ( !seen[i] )
  {
    for ( uint j = 0; j < elems[i].parents.size(); ++j )
      visitElem( ret, elems, seen, elems[i].parents[j] );
    ret.push_back( elems[i] );
    seen[i] = true;
  };
}

static std::vector<KGeoHierarchyElement> sortElems( const std::vector<KGeoHierarchyElement> elems )
{
  std::vector<KGeoHierarchyElement> ret;
  std::vector<bool> seenElems( elems.size(), false );
  for ( uint i = 0; i < elems.size(); ++i )
    visitElem( ret, elems, seenElems, i );
  return ret;
}

KigDocument* KigFilterKGeo::loadObjects( const QString& file, KSimpleConfig* c )
{
  KigDocument* ret = new KigDocument();

  using namespace std;

  QString group;
  bool ok = true;
  c->setGroup("Main");
  int number = c->readNumEntry ("Number");

  // first we determine the parent relationships, and sort the
  // elements in an order that we can be sure all of an object's
  // parents will have been handled before it is handled itself..
  // ( aka topological sort of the parent relations graph..
  std::vector<KGeoHierarchyElement> elems;
  elems.reserve( number );

  for ( int i = 0; i < number; ++i )
  {
    KGeoHierarchyElement elem;
    elem.id = i;
    group.setNum( i + 1 );
    group.prepend( "Object " );
    c->setGroup( group );
    QStrList parents;
    c->readListEntry( "Parents", parents );
    elems.push_back( elem );
    for ( const char* parent = parents.first(); parent; parent = parents.next() )
    {
      int parentIndex = QString::fromLatin1( parent ).toInt( &ok );
      if ( ! ok ) KIG_FILTER_PARSE_ERROR;
      if ( parentIndex != 0 )
        elems[i].parents.push_back( parentIndex - 1 );
    };
  };

  std::vector<KGeoHierarchyElement> sortedElems = sortElems( elems );
  std::vector<ObjectHolder*> os;
  os.resize( number, 0 );
  const ObjectFactory* factory = ObjectFactory::instance();

  // now we iterate over the elems again in the newly determined
  // order..
  for ( uint i = 0; i < sortedElems.size(); ++i )
  {
    const KGeoHierarchyElement& e = sortedElems[i];
    int id = e.id;
    group.setNum( id + 1 );
    group.prepend( "Object " );
    c->setGroup( group );
    int objID = c->readNumEntry( "Geo" );

    std::vector<ObjectCalcer*> parents;
    for ( uint j = 0; j < e.parents.size(); ++j )
    {
      int parentid = e.parents[j];
      parents.push_back( os[parentid]->calcer() );
    };

    ObjectCalcer* o = 0;
    switch (objID)
    {
    case ID_point:
    {
      // fetch the coordinates...
      QString strX = c->readEntry("QPointX");
      QString strY = c->readEntry("QPointY");
      double x = strX.toDouble(&ok);
      if (!ok) KIG_FILTER_PARSE_ERROR;
      double y = strY.toDouble(&ok);
      if (!ok) KIG_FILTER_PARSE_ERROR;
      Coordinate m( x, y );
      uint nparents = parents.size();
      if ( nparents == 0 )
        o = factory->fixedPointCalcer( m );
      else if ( nparents == 1 )
        o = factory->constrainedPointCalcer( parents[0], m, *ret );
      else
        KIG_FILTER_PARSE_ERROR;
      break;
    }
    case ID_segment:
    {
      o = new ObjectTypeCalcer( SegmentABType::instance(), parents );
      break;
    }
    case ID_circle:
    {
      o = new ObjectTypeCalcer( CircleBCPType::instance(), parents );
      break;
    }
    case ID_line:
    {
      o = new ObjectTypeCalcer( LineABType::instance(), parents );
      break;
    }
    case ID_bisection:
    {
      // if this is the bisection of two points, first build a segment
      // between them..
      if ( parents.size() == 2 )
      {
        ObjectTypeCalcer* seg = new ObjectTypeCalcer( SegmentABType::instance(), parents );
        parents.clear();
        parents.push_back( seg );
      }
      if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
      o = factory->propertyObjectCalcer( parents[0], "mid-point" );
      break;
    };
    case ID_perpendicular:
    {
      o = new ObjectTypeCalcer( LinePerpendLPType::instance(), parents );
      break;
    }
    case ID_parallel:
    {
      o = new ObjectTypeCalcer( LineParallelLPType::instance(), parents );
      break;
    }
    case ID_vector:
    {
      o = new ObjectTypeCalcer( VectorType::instance(), parents );
      break;
    }
    case ID_ray:
    {
      o = new ObjectTypeCalcer( RayABType::instance(), parents );
      break;
    }
    case ID_move:
    {
      o = new ObjectTypeCalcer( TranslatedType::instance(), parents );
      break;
    }
    case ID_mirrorPoint:
    {
      o = new ObjectTypeCalcer( PointReflectionType::instance(), parents );
      break;
    }
    case ID_pointOfConc:
    {
      o = new ObjectTypeCalcer( LineLineIntersectionType::instance(), parents );
      break;
    }
    case ID_text:
    {
      bool frame = c->readBoolEntry( "Frame" );
      double x = c->readDoubleNumEntry( "TextRectCenterX" );
      double y = c->readDoubleNumEntry( "TextRectCenterY" );
      QString text = c->readEntry( "TextRectEntry" );
      double height = c->readNumEntry( "TextRectHeight" );
      double width  = c->readNumEntry( "TextRectWidth" );
      // we don't want the center, but the top left..
      x -= width / 80;
      y -= height / 80;
      o = factory->labelCalcer(
        text, Coordinate( x, y ), frame, std::vector<ObjectCalcer*>(), *ret );
      break;
    }
    case ID_fixedCircle:
    {
      double r = c->readDoubleNumEntry( "Radius" );
      parents.push_back( new ObjectConstCalcer( new DoubleImp( r ) ) );
      o = new ObjectTypeCalcer( CircleBPRType::instance(), parents );
      break;
    }
    case ID_angle:
    {
      if ( parents.size() == 3 )
      {
        ObjectTypeCalcer* ao = new ObjectTypeCalcer( AngleType::instance(), parents );
        ao->calc( *ret );
        parents.clear();
        parents.push_back( ao );
      };
      if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* angle = parents[0];
      parents.clear();
      const Coordinate c =
        static_cast<const PointImp*>( angle->parents()[1]->imp() )->coordinate();
      o = filtersConstructTextObject( c, angle, "angle-degrees", *ret, true );
      break;
    }
    case ID_distance:
    {
      if ( parents.size() != 2 ) KIG_FILTER_PARSE_ERROR;
      ObjectTypeCalcer* segment = new ObjectTypeCalcer( SegmentABType::instance(), parents );
      segment->calc( *ret );
      Coordinate m = ( static_cast<const PointImp*>( parents[0]->imp() )->coordinate() +
                       static_cast<const PointImp*>( parents[1]->imp() )->coordinate() ) / 2;
      o = filtersConstructTextObject( m, segment, "length", *ret, true );
      break;
    }
    case ID_arc:
    {
      o = new ObjectTypeCalcer( AngleType::instance(), parents );
      break;
    }
    case ID_area:
    {
      if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
      const CircleImp* circle = static_cast<const CircleImp*>( parents[0]->imp() );
      const Coordinate c = circle->center() + Coordinate( circle->radius(), 0 );
      o = filtersConstructTextObject( c, parents[0], "surface", *ret, true );
      break;
    }
    case ID_slope:
    {
      // if parents contains a segment, line, vector or whatever, we
      // take its parents cause we want points..
      if ( parents.size() == 1 ) parents = parents[0]->parents();
      if ( parents.size() != 2 ) KIG_FILTER_PARSE_ERROR;
      const Coordinate c = (
        static_cast<const PointImp*>( parents[0]->imp() )->coordinate() +
        static_cast<const PointImp*>( parents[1]->imp() )->coordinate() ) / 2;
      ObjectTypeCalcer* line = new ObjectTypeCalcer( LineABType::instance(), parents );
      line->calc( *ret );
      o = filtersConstructTextObject( c, line, "slope", *ret, true );
      break;
    }
    case ID_circumference:
    {
      if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
      const CircleImp* c = static_cast<const CircleImp*>( parents[0]->imp() );
      const Coordinate m = c->center() + Coordinate( c->radius(), 0 );
      o = filtersConstructTextObject( m, parents[0], "circumference", *ret, true );
      break;
    }
    case ID_rotation:
    {
      // in kig, the rotated object should be last..
      ObjectCalcer* t = parents[2];
      parents[2] = parents[0];
      parents[0] = t;
      o = new ObjectTypeCalcer( RotationType::instance(), parents );
      break;
    }
    default:
      KIG_FILTER_PARSE_ERROR;
    };

    // set the color...
    QColor co = c->readColorEntry( "Color" );
    if( !co.isValid() )
      co = Qt::blue;
    ObjectDrawer* d = new ObjectDrawer( co );

    os[i] = new ObjectHolder( o, d );
    os[i]->calc( *ret );
  }; // for loop (creating KGeoHierarchyElements..

  ret->addObjects( os );
  ret->setGrid( grid );
  ret->setAxes( axes );
  return ret;
}

KigFilterKGeo::KigFilterKGeo()
{
}

KigFilterKGeo::~KigFilterKGeo()
{
}

KigFilterKGeo* KigFilterKGeo::instance()
{
  static KigFilterKGeo f;
  return &f;
}
