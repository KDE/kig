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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#include "kgeo-filter.h"

#include "kgeo-resource.h"

#include "../kig/kig_part.h"
#include "../misc/objects.h"
#include "../objects/object.h"
#include "../objects/bogus_imp.h"
#include "../objects/point_imp.h"
#include "../objects/text_type.h"
#include "../objects/circle_imp.h"
#include "../objects/object_factory.h"
#include "../objects/line_type.h"
#include "../objects/circle_type.h"
#include "../objects/point_type.h"
#include "../objects/other_type.h"
#include "../objects/transform_types.h"
#include "../objects/intersection_types.h"

#include <ksimpleconfig.h>

#include <algorithm>
#include <functional>

bool KigFilterKGeo::supportMime( const QString& mime )
{
  return mime == "application/x-kgeo";
};

bool KigFilterKGeo::load( const QString& sFrom, KigDocument& doc )
{
  // kgeo uses a KSimpleConfig to save its contents...
  KSimpleConfig config ( sFrom );

  loadMetrics ( &config );
  return loadObjects ( sFrom, &config, doc );
}

void KigFilterKGeo::loadMetrics(KSimpleConfig* c )
{
  c->setGroup("Main");
  xMax = c->readNumEntry("XMax", 16);
  yMax = c->readNumEntry("YMax", 11);
  // the rest is not relevant to us (yet ?)...
}

namespace {
struct HierarchyElement
{
  int id;
  std::vector<int> parents;
};
};

static void visitElem( std::vector<HierarchyElement>& ret,
                       const std::vector<HierarchyElement>& elems,
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
};

static std::vector<HierarchyElement> sortElems( const std::vector<HierarchyElement> elems )
{
  std::vector<HierarchyElement> ret;
  std::vector<bool> seenElems( elems.size(), false );
  for ( uint i = 0; i < elems.size(); ++i )
    visitElem( ret, elems, seenElems, i );
  return ret;
};

// constructs a text object with text "%1", location c, and variable
// parts given by the argument arg of obj o.
static Object* constructTextObject( const Coordinate& c, Object* o,
                                    const QCString& arg,
                                    const KigDocument& doc )
{
  ObjectFactory* fact = ObjectFactory::instance();
  Object* propo = fact->propertyObject( o, arg );
  propo->calc( doc );
  return fact->label( QString::fromLatin1( "%1" ), c, true,
                      Objects( propo ) );
};

bool KigFilterKGeo::loadObjects( const QString& file, KSimpleConfig* c, KigDocument& doc )
{
  using namespace std;
  QString group;
  bool ok = true;
  c->setGroup("Main");
  int number = c->readNumEntry ("Number");

  // first we determine the parent relationships, and sort the
  // elements in an order that we can be sure all of an object's
  // parents will have been handled before it is handled itself..
  // ( aka topological sort of the parent relations graph..
  std::vector<HierarchyElement> elems;
  elems.reserve( number );

  for ( int i = 0; i < number; ++i )
  {
    HierarchyElement elem;
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

  std::vector<HierarchyElement> sortedElems = sortElems( elems );
  Objects os;
  os.resize( number, 0 );
  ObjectFactory* factory = ObjectFactory::instance();

  // now we iterate over the elems again in the newly determined
  // order..
  for ( uint i = 0; i < sortedElems.size(); ++i )
  {
    const HierarchyElement& e = sortedElems[i];
    int id = e.id;
    group.setNum( id + 1 );
    group.prepend( "Object " );
    c->setGroup( group );
    int objID = c->readNumEntry( "Geo" );

    Objects parents;
    for ( uint j = 0; j < e.parents.size(); ++j )
    {
      int parentid = e.parents[j];
      parents.push_back( os[parentid] );
    };

    switch (objID)
    {
    case ID_point:
    {
      if ( ! parents.empty() )
        KIG_FILTER_PARSE_ERROR;
      // fetch the coordinates...
      QString strX = c->readEntry("QPointX");
      QString strY = c->readEntry("QPointY");
      double x = strX.toDouble(&ok);
      if (!ok) KIG_FILTER_PARSE_ERROR;
      double y = strY.toDouble(&ok);
      if (!ok) KIG_FILTER_PARSE_ERROR;
      Object* pos = factory->fixedPoint( Coordinate( x, y ) );
      pos->calc( doc );
      os[id] = pos;
      break;
    }
    case ID_segment:
    {
      os[id] = new RealObject( SegmentABType::instance(), parents );
      break;
    }
    case ID_circle:
    {
      os[id] = new RealObject( CircleBCPType::instance(), parents );
      break;
    }
    case ID_line:
    {
      os[id] = new RealObject( LineABType::instance(), parents );
      break;
    }
    case ID_bisection:
    {
      // if this is the bisection of a segment, just take the
      // segment's two parents..
      if ( parents.size() == 1 )
        parents = parents[0]->parents();
      if ( parents.size() != 2 ) KIG_FILTER_PARSE_ERROR;
      os[id] = new RealObject( MidPointType::instance(), parents );
      break;
    };
    case ID_perpendicular:
    {
      os[id] = new RealObject( LinePerpendLPType::instance(), parents );
      break;
    }
    case ID_parallel:
    {
      os[id] = new RealObject( LineParallelLPType::instance(), parents );
      break;
    }
    case ID_vector:
    {
      os[id] = new RealObject( VectorType::instance(), parents );
      break;
    }
    case ID_ray:
    {
      os[id] = new RealObject( RayABType::instance(), parents );
      break;
    }
    case ID_move:
    {
      os[id] = new RealObject( TranslatedType::instance(), parents );
      break;
    }
    case ID_mirrorPoint:
    {
      os[id] = new RealObject( PointReflectionType::instance(), parents );
      break;
    }
    case ID_pointOfConc:
    {
      os[id] = new RealObject( LineLineIntersectionType::instance(), parents );
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
      Object* label = factory->label( text, Coordinate( x, y ), frame );
      label->calc( doc );
      os[id] = label;
      break;
    }
    case ID_fixedCircle:
    {
      double r = c->readDoubleNumEntry( "Radius" );
      parents.push_back( new DataObject( new DoubleImp( r ) ) );
      os[id] = new RealObject( CircleBPRType::instance(), parents );
      break;
    }
    case ID_angle:
    {
      if ( parents.size() == 3 )
      {
        Object* ao = new RealObject( AngleType::instance(), parents );
        ao->setShown( false );
        ao->calc( doc );
        parents = Objects( ao );
      };
      if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
      Object* angle = parents[0];
      parents.clear();
      const Coordinate c =
        static_cast<const PointImp*>( angle->parents()[1]->imp() )->coordinate();
      os[i] = constructTextObject( c, angle, "angle-degrees", doc );
      break;
    }
    case ID_distance:
    {
      if ( parents.size() != 2 ) KIG_FILTER_PARSE_ERROR;
      Object* segment = new RealObject( SegmentABType::instance(), parents );
      segment->calc( doc );
      segment->setShown( false );
      Coordinate m = ( static_cast<const PointImp*>( parents[0]->imp() )->coordinate() +
                       static_cast<const PointImp*>( parents[1]->imp() )->coordinate() ) / 2;
      os[i] = constructTextObject( m, segment, "length", doc );
      break;
    }
    case ID_arc:
    {
      os[id] = new RealObject( AngleType::instance(), parents );
      break;
    }
    case ID_area:
    {
      if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
      const CircleImp* circle = static_cast<const CircleImp*>( parents[0]->imp() );
      const Coordinate c = circle->center() + Coordinate( circle->radius(), 0 );
      os[i] = constructTextObject( c, parents[0], "surface", doc );
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
      Object* line = new RealObject( LineABType::instance(), parents );
      line->setShown( false );
      line->calc( doc );
      os[i] = constructTextObject( c, line, "slope", doc );
      break;
    }
    case ID_circumference:
    {
      if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
      const CircleImp* c = static_cast<const CircleImp*>( parents[0]->imp() );
      const Coordinate m = c->center() + Coordinate( c->radius(), 0 );
      os[i] = constructTextObject( m, parents[0], "circumference", doc );
      break;
    }
    case ID_rotation:
    {
      // in kig, the rotated object should be last..
      Object* t = parents[2];
      parents[2] = parents[0];
      parents[0] = t;
      os[i] = new RealObject( RotationType::instance(), parents );
      break;
    }
    default:
      KIG_FILTER_PARSE_ERROR;
    };

    os[i]->calc( doc );

    // set the color...
    QColor co = c->readColorEntry( "Color" );
    if( co.isValid() )
      os[i]->setColor( co );
  }; // for loop (creating HierarchyElements..

  doc.setObjects( os );

  return true;
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
