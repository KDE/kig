#include "kgeo.h"

#include "../objects/object.h"
#include "../misc/objects.h"
#include "../misc/hierarchy.h"
#include "../objects/point.h"
#include "../objects/segment.h"

#include <ksimpleconfig.h>

#include <qtextstream.h>
#include <qfile.h>
#include <qdom.h>

// here i have included the file resource.h from the KGeo
// distribution, it is distributed under the GPL like Kig, so this is
// no pb... 

/***************************************************************************
                          resource.h  -  description
                             -------------------
    begin                : Die Okt  3 09:00:59 CEST 2000
    copyright            : (C) 2000 by Marc Bartsch
    email                : marc.bartsch@topmail.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RESOURCE_H
#define RESOURCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <qstring.h>
#include <klocale.h>

///////////////////////////////////////////////////////////////////
// resource.h  -- contains macros used for commands


///////////////////////////////////////////////////////////////////
// COMMAND VALUES FOR MENUBAR AND TOOLBAR ENTRIES


///////////////////////////////////////////////////////////////////
// File-menu entries
#define ID_FILE_NEW_WINDOW          10010
#define ID_FILE_NEW                 10020
#define ID_FILE_OPEN                10030
#define ID_FILE_OPEN_RECENT         10040
#define ID_FILE_CLOSE               10050

#define ID_FILE_SAVE                10060
#define ID_FILE_SAVE_AS             10070

#define ID_FILE_PRINT               10080

#define ID_FILE_QUIT                10090

///////////////////////////////////////////////////////////////////
// Edit-menu entries
#define ID_EDIT_COPY                11010
#define ID_EDIT_CUT                 11020
#define ID_EDIT_PASTE               11030
#define ID_EDIT_PREFERENCES         11040
#define ID_EDIT_FULLSCREEN					11050

///////////////////////////////////////////////////////////////////
// View-menu entries                    
#define ID_VIEW_TOOLBAR             12010
#define ID_VIEW_STATUSBAR           12020
#define ID_VIEW_FULLSCREEN					12030

///////////////////////////////////////////////////////////////////
// Help-menu entries
#define ID_HELP_CONTENTS            1002

///////////////////////////////////////////////////////////////////
// General application values
#define ID_STATUS_MSG               1001

#define IDS_STATUS_DEFAULT          "Ready."

#define ID_infinite									-1

#define ID_point 										1
#define ID_pointxy									14
#define ID_pointOnLine							15
#define ID_pointOfConc							7
#define ID_bisection								5
#define ID_mirrorPoint							9

#define ID_segment 									2
#define ID_circle 									3
#define ID_line											4
#define ID_fixedCircle							6
#define ID_arc			 								8
#define ID_eraser										10
#define ID_attacher									11
#define ID_tracer										12
#define ID_triangle									13
#define ID_colorizer								16
#define ID_thicker									17
#define ID_geoPoint 								18
#define ID_geoTool	 								19
#define ID_geoObject 								20
#define ID_geoMeasure								21
#define ID_distance 								22
#define ID_angle										23
#define ID_area			 								24
#define ID_slope			 							25
#define ID_circumference						26
#define ID_vector										27
#define ID_geoLine  								28
#define ID_ray											29
#define ID_parallel									30
#define ID_perpendicular						31
#define ID_move											32
#define ID_rotation									33
#define ID_text   									34

#define ID_buttonFileNew 						100
#define ID_buttonKiosk 							101

#define ID_buttonPoint 							110
#define ID_buttonPointxy 						111
#define ID_buttonPointOnLine 				112
#define ID_buttonPointOfConc				113
#define ID_buttonBisection					114
#define ID_buttonMirrorPoint				115
#define ID_buttonMove								116
#define ID_buttonRotation						117

#define ID_buttonSegment 						120
#define ID_buttonLine								121
#define ID_buttonVector							122
#define ID_buttonRay								123
#define ID_buttonParallel						124
#define ID_buttonPerpendicular			125
#define ID_buttonTriangle						126

#define ID_buttonBaseCircle					130
#define ID_buttonCircle							131
#define ID_buttonArc   							132

#define	ID_buttonDistance						140
#define	ID_buttonAngle              141
#define	ID_buttonArea               142
#define	ID_buttonSlope              143
#define	ID_buttonCircumference      144

#define	ID_buttonBlack 							150
#define	ID_buttonDarkGray 					151
#define	ID_buttonLightGray					152
#define	ID_buttonWhite 							153
#define	ID_buttonBlue								154
#define	ID_buttonRed 								155
#define	ID_buttonGreen							156

#define	ID_buttonThinLine 					160
#define	ID_buttonMiddleLine					161
#define	ID_buttonThickLine 					162

#define ID_buttonEraser 						170
#define ID_buttonAttacher 					171
#define ID_buttonTracer 						172
#define ID_buttonText	  						173
#define ID_buttonMoveGrid						174
#define ID_buttonPointer						175
#define	ID_buttonDrawColor					176
#define	ID_buttonSizer							177

#define ID_drawingModeNoMode 					0
#define ID_drawingModeMovingGrid		 	1
#define ID_drawingModeMovingObjects 	2
#define ID_drawingModeConstructing 		3

#define MinimumPointSize							3

#define Str_AppName   								"KGeo"


#define ID_overlayRectSize					 24

#define PI													3.1415926535
	
#endif // RESOURCE_H


// here comes our own code...

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
	  }
	  break;
	case ID_segment:
	  {
	    objs.push_back(new Segment);
	  };
	  break;
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
