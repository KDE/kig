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


#include "filter.h"

#include "resource.h"

#include "../../misc/objects.h"
#include "../../misc/hierarchy.h"
#include "../../objects/object.h"
#include "../../objects/normalpoint.h"

#include <ksimpleconfig.h>

bool KigFilterKGeo::supportMime( const QString mime )
{
  if (mime == QString::fromLatin1 ("application/x-kgeo")) return true;
  else return false;
};

KigFilter::Result KigFilterKGeo::load( const QString sFrom, Objects& os )
{
  // kgeo uses a KSimpleConfig to save its contents...
  KSimpleConfig config ( sFrom );

  Result r;
  r = loadMetrics ( &config );
  if ( r != OK ) return r;
  r = loadObjects ( &config, os );
  return r;
}

KigFilter::Result KigFilterKGeo::loadMetrics(KSimpleConfig* c )
{
  c->setGroup("Main");
  xMax = c->readNumEntry("XMax", 16);
  yMax = c->readNumEntry("YMax", 11);
  // the rest is not relevant to us (yet ?)...
  return OK;
}

KigFilter::Result KigFilterKGeo::loadObjects( KSimpleConfig* c, Objects& os )
{
  QString group;
  c->setGroup("Main");
  uint number = c->readNumEntry ("Number");
  kdDebug() << k_funcinfo << "number of objects: " << number << endl;

  // how do we work: we construct ourselves an ObjectHierarchy by
  // manually using constructing the HierarchyElements, and then
  // calling ObjectHierarchy::fillUp()..  Most Objects don't need
  // params, but for those who do, we only set the most important
  // ones, and trust the defaults for the rest..
  ElemList allElems;

  // create all objects:
  for( uint i = 0; i < number; ++i )
  {
    // fetch the information...
    group = "";
    group.setNum(i+1);
    group.prepend("Object ");
    c->setGroup(group);
    int objID = c->readNumEntry( "Geo" );
    kdDebug() << k_funcinfo << "objID: " << objID << endl;

    // here we construct a hierarchyelement for the current
    // object.. parents are dealt with later...
    HierarchyElement* e = 0;

    switch (objID)
    {
    case ID_point:
    {
      // fetch the coordinates...
      bool ok;
      QString strX = c->readEntry("QPointX");
      QString strY = c->readEntry("QPointY");
      double x = strX.toDouble(&ok);
      if (!ok) return ParseError;
      double y = strY.toDouble(&ok);
      if (!ok) return ParseError;
      e = new HierarchyElement( "NormalPoint", i );
      e->setParam( "implementation-type", "Fixed" );
      e->setParam( "x", QString::number( x ) );
      e->setParam( "y", QString::number( y ) );
      break;
    }
    case ID_segment:
    {
      e = new HierarchyElement( "Segment", i );
      break;
    }
    case ID_circle:
    {
      e = new HierarchyElement( "CircleBCP", i );
      break;
    }
    case ID_line:
    {
      e = new HierarchyElement( "LineTTP", i );
      break;
    }
    case ID_bisection:
    {
      e = new HierarchyElement( "MidPoint", i );
      break;
    };
    case ID_perpendicular:
    {
      e = new HierarchyElement( "LinePerpend", i );
      break;
    }
    case ID_parallel:
    {
      e = new HierarchyElement( "LineParallel", i );
      break;
    }
    case ID_vector:
    {
      e = new HierarchyElement( "Vector", i );
      break;
    }
    case ID_ray:
    {
      e = new HierarchyElement( "Ray", i );
      break;
    }
    case ID_move:
    {
      e = new HierarchyElement( "TranslatedPoint", i );
      break;
    }
    case ID_mirrorPoint:
    {
      e = new HierarchyElement( "MirrorPoint", i );
      break;
    }

//  	case ID_pointOfConc:
// 	  {
// 	    objs.push_back( new PointOfConc() );
// 	    break;
// 	  };
// 	case ID_text:
// 	  objs.push_back( new Text() );
// 	  break;
// 	case ID_fixedCircle:
// 	  objs.push_back( new FixedCircle() );
// 	  break;
// 	case ID_angle:
// 	  objs.push_back( new Angle() );
// 	  break;
// 	case ID_distance:
// 	  objs.push_back( new Distance() );
// 	  break;
// 	case ID_arc:
// 	  objs.push_back( new Arc() );
// 	  break;
// 	case ID_area:
// 	  objs.push_back( new Area() );
// 	  break;
// 	case ID_slope:
// 	  objs.push_back( new Slope() );
// 	  break;
// 	case ID_circumference:
// 	  objs.push_back( new Circumference() );
// 	  break;
// 	case ID_rotation:
// 	  objs.push_back( new Rotation() );
// 	  break;
    default:
      return ParseError;
    }; // switch of objID

    // set the color...
    QColor co = c->readColorEntry( "Color" );
    if( !co.isValid() ) return ParseError;
    e->setParam( "color", co.name() );

    allElems.push_back( e );
  }; // for loop (creating HierarchyElements..

  assert( allElems.size() == number );

  // now we iterate again to set the parents correct...
  for ( uint i = 0; i < number; ++i )
  {
    QStrList parents;
    group = "";
    group.setNum(i+1);
    group.prepend ("Object ");
    c->setGroup(group);
    c->readListEntry("Parents", parents);
    char* parent;
    int parentIndex;
    for( parent = parents.first(); parent; parent = parents.next())
    {
      bool ok;
      parentIndex = QString(parent).toInt(&ok);
      if (!ok)
        return ParseError;
      if( parentIndex != 0 )
        allElems[i]->addParent( allElems[parentIndex-1] );
    };
  }; // for loop ( setting parents...

  ObjectHierarchy hier( allElems );
  os = hier.fillUp( Objects() );

  return OK;
}

KigFilterKGeo::KigFilterKGeo()
{
}

KigFilterKGeo::~KigFilterKGeo()
{
}
