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
 * overload...
 */
void calcBorderPoints( double& xa, double& xb, double& ya, double& yb, const Rect& r);

#endif
