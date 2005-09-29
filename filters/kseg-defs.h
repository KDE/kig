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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,  
// MA  02110-1301, USA.

// this is a collection of definitions we need from KSeg.  It includes
// code from defs.H and G_drawstyle.H.  Thanks to Ilya Baran for
// making KSeg GPL, so there are no license probs or whatever..

/*
 *  KSeg
 *  Copyright (C) 1999-2003 Ilya Baran
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
 *
 *  Send comments and/or bug reports to:
 *                 ibaran@mit.edu
 */


#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <qglobal.h>

using namespace std;

#define DRAW_MAX 5000 // maximum coordinate.  assumes you have a screen resolution less than this.

#define BIG (1e+37)
#define SMALL (1e-10)

inline int ROUND(double x) { return ((int)(x + 0.5)); }
inline int SIGN(double x) { return (x < 0) ? -1 : 1; }
inline int INTRAND(int a, int b) { return QMIN(a, b) + rand() % abs(a - b); }
#define SQR(x) ((x) * (x))
#define CUBE(x) ((x) * (x) * (x))
#define QUAD(x) (((x) * (x)) * ((x) * (x)))

enum G_Type
{
  G_POINT = 1,
  G_SEGMENT = 2,
  G_RAY = 4,
  G_LINE = 8,
  G_CIRCLE = 16,
  G_ARC = 32,
  G_POLYGON = 64,
  G_CIRCLEINTERIOR = 128,
  G_ARCSECTOR = 256,
  G_ARCSEGMENT = 512,
  //non-primitive geometric types now:
  G_LOCUS = 1024,
  G_MEASURE = 2048,
  G_CALCULATE = 4096,
  G_ANNOTATION = 8192,
  //fake type for scripting:
  G_LOOP = 16384,
  //compound types now:
  G_STRAIGHT = G_SEGMENT | G_LINE | G_RAY,
  G_CURVE = G_STRAIGHT | G_ARC | G_CIRCLE,
  G_FILLED = G_POLYGON | G_CIRCLEINTERIOR | G_ARCSECTOR | G_ARCSEGMENT,
  G_GEOMETRIC = G_POINT | G_CURVE | G_FILLED | G_LOCUS,
  G_VALUE = G_MEASURE | G_CALCULATE,
  G_TEXT = G_VALUE | G_ANNOTATION,
  G_ANY = G_GEOMETRIC | G_TEXT | G_LOOP
};

enum G_AnyType
{
  G_TRANSLATED,
  G_ROTATED,
  G_SCALED,
  G_REFLECTED
};

#define IS_TRANSFORM(x) ((x) == G_TRANSLATED || (x) == G_ROTATED || (x) == G_SCALED || (x) == G_REFLECTED)

enum G_PointType
{
  G_FREE_POINT = G_REFLECTED + 1,
  G_CONSTRAINED_POINT,
  G_INTERSECTION_POINT,
  G_INTERSECTION2_POINT,
  G_MID_POINT
};

enum G_SegmentType
{
  G_ENDPOINTS_SEGMENT = G_REFLECTED + 1
};

enum G_RayType
{
  G_TWOPOINTS_RAY = G_REFLECTED + 1,
  G_BISECTOR_RAY
};

enum G_LineType
{
  G_TWOPOINTS_LINE = G_REFLECTED + 1,
  G_PARALLEL_LINE,
  G_PERPENDICULAR_LINE
};

enum G_CircleType
{
  G_CENTERPOINT_CIRCLE = G_REFLECTED + 1,
  G_CENTERRADIUS_CIRCLE
};

enum G_ArcType
{
  G_THREEPOINTS_ARC = G_REFLECTED + 1
};

enum G_FilledType
{
  G_DEFAULT_FILLED = G_REFLECTED + 1
};

enum G_LocusType
{
  G_OBJECT_LOCUS = G_REFLECTED + 1
};

enum G_MeasureType
{
  G_DISTANCE_MEASURE,
  G_LENGTH_MEASURE,
  G_RADIUS_MEASURE,
  G_ANGLE_MEASURE,
  G_RATIO_MEASURE,
  G_SLOPE_MEASURE,
  G_AREA_MEASURE
};

enum G_CalculateType
{
  G_REGULAR_CALCULATE
};


enum MenuIDs
{
  ID_NEW_SEGMENT = 1,
  ID_NEW_MIDPOINT,
  ID_NEW_LINE,
  ID_NEW_PERPENDICULAR,
  ID_NEW_RAY,
  ID_NEW_BISECTOR,
  ID_NEW_CIRCLE,
  ID_NEW_INTERSECTION,
  ID_NEW_ARC,
  ID_NEW_LOCUS,
  ID_NEW_ARCSECTOR,
  ID_NEW_ARCSEGMENT,
  ID_NEW_CIRCLEINTERIOR,
  ID_NEW_POLYGON,

  ID_EDIT_UNDO,
  ID_EDIT_REDO,
  ID_EDIT_DELETE,
  ID_EDIT_TOGGLELABELS,
  ID_EDIT_SHOWLABELS,
  ID_EDIT_HIDELABELS,
  ID_EDIT_CHANGELABEL,
  ID_EDIT_HIDE,
  ID_EDIT_SHOWHIDDEN,
  ID_EDIT_COLOR,
  ID_EDIT_POINTSTYLE,
  ID_EDIT_LINESTYLE,
  ID_EDIT_FONT,
  ID_EDIT_CHANGE_NUMBER_OF_SAMPLES,
  ID_EDIT_PREFERENCES,

  ID_EDIT_COLOR_BLACK,
  ID_EDIT_COLOR_GRAY,
  ID_EDIT_COLOR_RED,
  ID_EDIT_COLOR_GREEN,
  ID_EDIT_COLOR_BLUE,
  ID_EDIT_COLOR_YELLOW,
  ID_EDIT_COLOR_PURPLE,
  ID_EDIT_COLOR_CYAN,
  ID_EDIT_COLOR_OTHER,

  ID_EDIT_POINTSTYLE_LARGECIRCLE,
  ID_EDIT_POINTSTYLE_MEDIUMCIRCLE,
  ID_EDIT_POINTSTYLE_SMALLCIRCLE,

  ID_EDIT_LINESTYLE_SOLID,
  ID_EDIT_LINESTYLE_DASHED,
  ID_EDIT_LINESTYLE_DOTTED,
  ID_EDIT_LINESTYLE_THIN,
  ID_EDIT_LINESTYLE_NORMAL,
  ID_EDIT_LINESTYLE_THICK,

  ID_EDIT_FONT_10,
  ID_EDIT_FONT_12,
  ID_EDIT_FONT_14,
  ID_EDIT_FONT_20,
  ID_EDIT_FONT_30,
  ID_EDIT_FONT_FONT,

  ID_MEASURE_DISTANCE,
  ID_MEASURE_LENGTH,
  ID_MEASURE_RADIUS,
  ID_MEASURE_ANGLE,
  ID_MEASURE_RATIO,
  ID_MEASURE_SLOPE,
  ID_MEASURE_AREA,
  ID_MEASURE_CALCULATE,

  ID_TRANSFORM_CHOOSE_VECTOR,
  ID_TRANSFORM_CHOOSE_MIRROR,
  ID_TRANSFORM_CHOOSE_CENTER,
  ID_TRANSFORM_CHOOSE_RATIO,
  ID_TRANSFORM_CHOOSE_ANGLE,
  ID_TRANSFORM_CLEAR_CHOSEN,
  ID_TRANSFORM_TRANSLATE,
  ID_TRANSFORM_ROTATE,
  ID_TRANSFORM_REFLECT,
  ID_TRANSFORM_SCALE,

  ID_CONSTRUCTION_MAKE_NORMAL,
  ID_CONSTRUCTION_MAKE_GIVEN,
  ID_CONSTRUCTION_MAKE_FINAL,
  ID_CONSTRUCTION_MAKE_INITIAL,
  ID_CONSTRUCTION_RECURSE,

  ID_PLAY_QUICKPLAY,

  ID_QUICKPLAY_SET_DIRECTORY,

  ID_FILE_RECENTLIST_START //should be the last entry
};

#endif //DEFS_H


/*
 *  KSeg
 *  Copyright (C) 1999-2003 Ilya Baran
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
 *
 *  Send comments and/or bug reports to:
 *                 ibaran@mit.edu
 */


#ifndef G_DRAWSTYLE_H
#define G_DRAWSTYLE_H

enum PointStyle
{
  ANY = 0,
  SMALL_CIRCLE,
  MEDIUM_CIRCLE,
  LARGE_CIRCLE
};

#endif //G_DRAWSTYLE_H
