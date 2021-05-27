/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>
    SPDX-FileCopyrightText: 2000 Marc Bartsch <marc.bartsch@topmail.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/// note: this code comes from KGeo by Marc Bartsch.

#ifndef RESOURCE_H
#define RESOURCE_H

#include <iostream>
#include <QString>

///////////////////////////////////////////////////////////////////
// resource.h  -- contains macros used for commands


///////////////////////////////////////////////////////////////////
// COMMAND VALUES FOR MENUBAR AND TOOLBAR ENTRIES


///////////////////////////////////////////////////////////////////
// File-menu entries
#define ID_FILE_NEW_WINDOW 10010
#define ID_FILE_NEW 10020
#define ID_FILE_OPEN 10030
#define ID_FILE_OPEN_RECENT 10040
#define ID_FILE_CLOSE 10050

#define ID_FILE_SAVE 10060
#define ID_FILE_SAVE_AS 10070

#define ID_FILE_PRINT 10080

#define ID_FILE_QUIT 10090

///////////////////////////////////////////////////////////////////
// Edit-menu entries
#define ID_EDIT_COPY 11010
#define ID_EDIT_CUT 11020
#define ID_EDIT_PASTE 11030
// domi: disabled, breaks --enable-final, and is not used anyway.
//#define ID_EDIT_PREFERENCES 11040
#define ID_EDIT_FULLSCREEN 11050

///////////////////////////////////////////////////////////////////
// View-menu entries
#define ID_VIEW_TOOLBAR 12010
#define ID_VIEW_STATUSBAR 12020
#define ID_VIEW_FULLSCREEN 12030

///////////////////////////////////////////////////////////////////
// Help-menu entries
#define ID_HELP_CONTENTS 1002

///////////////////////////////////////////////////////////////////
// General application values
#define ID_STATUS_MSG 1001

#define IDS_STATUS_DEFAULT "Ready."

#define ID_infinite -1

#define ID_point 1
#define ID_pointxy 14
#define ID_pointOnLine 15
#define ID_pointOfConc 7
#define ID_bisection 5
#define ID_mirrorPoint 9

#define ID_segment 2
#define ID_circle 3
#define ID_line 4
#define ID_fixedCircle 6
#define ID_arc 8
#define ID_eraser 10
#define ID_attacher 11
#define ID_tracer 12
#define ID_triangle 13
#define ID_colorizer 16
#define ID_thicker 17
#define ID_geoPoint 18
#define ID_geoTool 19
#define ID_geoObject 20
#define ID_geoMeasure 21
#define ID_distance 22
#define ID_angle 23
#define ID_area 24
#define ID_slope 25
#define ID_circumference 26
#define ID_vector 27
#define ID_geoLine 28
#define ID_ray 29
#define ID_parallel 30
#define ID_perpendicular 31
#define ID_move 32
#define ID_rotation 33
#define ID_text 34

#define ID_buttonFileNew 100
#define ID_buttonKiosk 101

#define ID_buttonPoint 110
#define ID_buttonPointxy 111
#define ID_buttonPointOnLine 112
#define ID_buttonPointOfConc 113
#define ID_buttonBisection 114
#define ID_buttonMirrorPoint 115
#define ID_buttonMove 116
#define ID_buttonRotation 117

#define ID_buttonSegment 120
#define ID_buttonLine 121
#define ID_buttonVector 122
#define ID_buttonRay 123
#define ID_buttonParallel 124
#define ID_buttonPerpendicular 125
#define ID_buttonTriangle 126

#define ID_buttonBaseCircle 130
#define ID_buttonCircle 131
#define ID_buttonArc 132

#define ID_buttonDistance 140
#define ID_buttonAngle 141
#define ID_buttonArea 142
#define ID_buttonSlope 143
#define ID_buttonCircumference 144

#define ID_buttonBlack 150
#define ID_buttonDarkGray 151
#define ID_buttonLightGray 152
#define ID_buttonWhite 153
#define ID_buttonBlue 154
#define ID_buttonRed 155
#define ID_buttonGreen 156

#define ID_buttonThinLine 160
#define ID_buttonMiddleLine 161
#define ID_buttonThickLine 162

#define ID_buttonEraser 170
#define ID_buttonAttacher 171
#define ID_buttonTracer 172
#define ID_buttonText 173
#define ID_buttonMoveGrid 174
#define ID_buttonPointer 175
#define ID_buttonDrawColor 176
#define ID_buttonSizer 177

#define ID_drawingModeNoMode 0
#define ID_drawingModeMovingGrid 1
#define ID_drawingModeMovingObjects 2
#define ID_drawingModeConstructing 3

#define MinimumPointSize 3

#define Str_AppName "KGeo"


#define ID_overlayRectSize 24

#define PI 3.1415926535

#endif // RESOURCE_H
