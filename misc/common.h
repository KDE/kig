/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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


#ifndef KIGCALC_H
#define KIGCALC_H

#include "coordinate.h"
#include "rect.h"

#include <qptrlist.h>
#include <qrect.h>

#include <vector>

/**
 * Here, we define some algorithms which we need in
 * various places...
 */

/**
 * this returns a point, so that the line through point t
 * and the point returned is perpendicular on the line p1p2.
 */
Coordinate calcPointOnPerpend( const Coordinate& p1, const Coordinate& p2, const Coordinate& t );

/**
 * this returns a point, so that the line throught point t
 * and the point returned is parallel with p1p2
 */
Coordinate calcPointOnParallel( const Coordinate& p1, const Coordinate& p2, const Coordinate& t );

/**
 * this calcs the point where p1p2 and p3p4 intersect...
 */
Coordinate calcIntersectionPoint( const Coordinate& p1, const Coordinate& p2, const Coordinate& p3, const Coordinate& p4 );

/**
 * this sets p1 and p2 to p1' and p2' so that p1'p2' is the same line
 * as p1p2, and so that p1' and p2' are on the border of the Rect...
 */
void calcBorderPoints( Coordinate& p1, Coordinate& p2, const Rect& r );

/**
 * this does the same as the above function, but only for b..
 */
void calcRayBorderPoints( const Coordinate& a, Coordinate& b, const Rect& r );

/**
 * overload...
 */
void calcRayBorderPoints( const double xa, const double xb, double& ya,
                          double& yb, const Rect& r );

/**
 * overload...
 */
void calcBorderPoints( double& xa, double& xb, double& ya, double& yb, const Rect& r);

/**
 * calc the mirror point of p over the line defined by a and b...
 */
const Coordinate calcMirrorPoint( const Coordinate& a, const Coordinate& b,
                                  const Coordinate& p );

/**
 * is o on the line defined by point a and point b ?
 * fault is the allowed difference...
 */
bool isOnLine( const Coordinate o, const Coordinate a,
               const Coordinate b, const double fault );

/**
 * is o on the segment defined by point a and point b ?
 * this calls isOnLine(), but also checks if o is "between" a and b...
 * fault is the allowed difference...
 */
bool isOnSegment( const Coordinate o, const Coordinate a,
                  const Coordinate b, const double fault );

bool isOnRay( const Coordinate o, const Coordinate a,
              const Coordinate b, const double fault );

template <typename T>
T kigMin( const T& a, const T& b)
{
    return a < b ? a : b;
}

template <class T>
T kigMax( const T& a, const T& b )
{
    return a > b ? a : b;
};

#endif
