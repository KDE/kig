/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese

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
#include "../../objects/object.h"
#include "../../objects/normalpoint.h"
#include "../../objects/midpoint.h"
#include "../../objects/segment.h"
#include "../../objects/circle.h"
#include "../../objects/line.h"

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
  // the rest is not relevant to us (yet)...
  return OK;
}

KigFilter::Result KigFilterKGeo::loadObjects( KSimpleConfig* c, Objects& os )
{
  QString group;
  c->setGroup("Main");
  int number = c->readNumEntry ("Number");
  kdDebug() << k_funcinfo << "number of objects: " << number << endl;

  // create all objects:
  for (int i = 0; i < number; ++i)
    {
      group = "";
      group.setNum(i+1);
      group.prepend("Object ");
      c->setGroup(group);
      int objID = c->readNumEntry( "Geo" );
      kdDebug() << k_funcinfo << "objID: " << objID << endl;
      Object* no = 0;
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
            NormalPoint* p = NormalPoint::fixedPoint( Coordinate( x, y ) );
	    no = p;
	    break;
	  }
	case ID_segment:
	  {
	    no = new Segment;
	    break;
	  };
	case ID_circle:
	  {
	    no = new CircleBCP;
	    break;
	  };
	case ID_line:
	  {
	    no = new LineTTP;
	    break;
	  }
 	case ID_bisection:
 	  {
 	    no = new MidPoint();
 	    break;
 	  };
 	case ID_perpendicular:
	  {
	    no = new LinePerpend();
	    break;
	  }
 	case ID_parallel:
	  {
	    no = new LineParallel();
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
// 	case ID_mirrorPoint:
// 	  objs.push_back( new MirrorPoint() );
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
// 	case ID_vector:
// 	  objs.push_back( new Vector() );
// 	  break;
// 	case ID_ray:
// 	  objs.push_back( new Ray() );
// 	  break;
// 	case ID_move:
// 	  objs.push_back( new Move() );
// 	  break;
// 	case ID_rotation:
// 	  objs.push_back( new Rotation() );
// 	  break;
	default:
	  return ParseError;
	}; // switch of objID

      // color...
      QColor co = c->readColorEntry( "Color" );
      if( !co.isValid() ) return ParseError;
      no->setColor( co );
      os.push_back( no );
    }; // for loop (creating objects...

  // now we iterate again to set the parents correct...
  for (int i = 0; i < number; ++i)
    {
      QStrList parents;
      group = "";
      group.setNum(i+1);
      group.prepend ("Object ");
      c->setGroup(group);
      c->readListEntry("Parents", parents);
      char* parent;
      int parentIndex;
      for ( parent = parents.first(); parent; parent = parents.next())
      {
        bool ok;
        parentIndex = QString(parent).toInt(&ok);
        if (!ok)
          return ParseError;
        if (parentIndex != 0 )
          os[i]->selectArg(os[parentIndex-1]);
      };
    }; // for loop ( setting parents...
  return OK;
}

KigFilterKGeo::KigFilterKGeo()
{
}

KigFilterKGeo::~KigFilterKGeo()
{
}
