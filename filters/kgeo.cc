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


#include "kgeo.h"

#include "../objects/object.h"
#include "../misc/objects.h"
#include "../misc/hierarchy.h"
#include "../objects/point.h"
#include "../objects/segment.h"
#include "../objects/circle.h"
#include "../objects/line.h"
// #include "../objects/

#include "kgeo_resource.h"

#include <ksimpleconfig.h>

#include <qtextstream.h>
#include <qfile.h>
#include <qdom.h>

bool KigFilterKGeo::supportMime( const QString mime )
{
  if (mime == QString::fromLatin1 ("application/x-kgeo")) return true;
  else return false;
};

KigFilter::Result KigFilterKGeo::convert( const QString sFrom, KTempFile& to )
{
  // the output file...
  QFile* fTo = new QFile (to.name());
  fTo->open(IO_WriteOnly);
  
  // kgeo uses a KSimpleConfig to save its contents...
  KSimpleConfig config ( sFrom );

  Result r;
  r = loadMetrics ( &config );
  if ( r != OK ) return r;
  r = loadObjects ( &config );
  if (r != OK) return r;

  // start building our file...
  QTextStream stream (fTo);

  // cf. KigDocument::saveFile()...
  QDomDocument doc ("KigDocument");
  QDomElement elem = doc.createElement ("KigDocument");
  elem.setAttribute( "Version", "2.0.000");

  Objects go;
  Objects fo;
  for (std::vector<Object*>::iterator i = objs.begin(); i != objs.end(); ++i)
    {
      fo.add(*i);
    };
  ObjectHierarchy hier ( go, fo, 0);
  hier.calc();
  hier.saveXML ( doc, elem );
  doc.appendChild(elem);

  stream << doc.toCString() << endl;
  kdDebug() << k_funcinfo << endl << doc.toCString() << endl;
  fTo->close();
  delete fTo;
  return OK;
}

KigFilter::Result KigFilterKGeo::loadMetrics(KSimpleConfig* c )
{
  c->setGroup("Main");
  xMax = c->readNumEntry("XMax", 16);
  yMax = c->readNumEntry("YMax", 11);
  // the rest is not relevant to us (yet)...
  return OK;
}

KigFilter::Result KigFilterKGeo::loadObjects(KSimpleConfig* c)
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
      switch (objID)
	{
	case ID_point:
	  {
	    Point* p = new Point;
	    objs.push_back(p);
	    bool ok;
	    QString strX = c->readEntry("QPointX");
	    QString strY = c->readEntry("QPointY");
	    double x = strX.toDouble(&ok);
	    if (!ok) return ParseError;
	    double y = strY.toDouble(&ok);
	    if (!ok) return ParseError;
	    p->setX(x);
	    p->setY(y);
	    break;
	  }
	case ID_segment:
	  {
	    objs.push_back(new Segment);
	    break;
	  };
	case ID_circle:
	  {
	    objs.push_back( new CircleBCP );
	    break;
	  };
	case ID_line:
	  {
	    objs.push_back( new LineTTP );
	    break;
	  }
 	case ID_bisection:
 	  {
 	    objs.push_back( new MidPoint() );
 	    break;
 	  };
 	case ID_perpendicular:
	  {
	    objs.push_back( new LinePerpend() );
	    break;
	  }
 	case ID_parallel:
	  {
	    objs.push_back( new LineParallel() );
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
    }; // for loop (creating objects...

  // now we iterate again to set the parents correct...
  for (int i = 0; i < number; ++i)
    {
      QStrList parents;
      Q_ASSERT (parents.isEmpty());
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
	    objs[i]->selectArg(objs[parentIndex-1]);
	};
    }; // for loop ( setting parents...
  return OK;
}

KigFilterKGeo::KigFilterKGeo()
  : hier (0)
{
}

KigFilterKGeo::~KigFilterKGeo()
{
  delete hier;
  for (vector<Object*>::iterator i = objs.begin(); i != objs.end(); ++i)
    {
      delete *i;
    };
}
