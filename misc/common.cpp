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


#include "common.h"

#include <cmath>

#include <kdebug.h>

#define max(x,y) ((x)<(y) ? (y) : (x))

Coordinate calcPointOnPerpend( const Coordinate& p1, const Coordinate& p2, const Coordinate& t )
{
  double x2,y2; // coords of another point on the perpend line
  double xa = p1.x;
  double xb = p2.x;
  double ya = p1.y;
  double yb = p2.y;
  if ( fabs(yb - ya) > 0.001 )
    {
      x2 = 0;
      y2 = (xb-xa)*(t.x)/(yb-ya) + t.y;
    }
  else // the line/segment = horizontal, so the perpend is vertical
    {
      x2 = t.x;
      y2 = 0;
    };
  return Coordinate(x2,y2);
};

Coordinate calcPointOnParallel( const Coordinate& p1, const Coordinate& p2, const Coordinate& t )
{
  Coordinate dir = p1-p2;
  return t + dir*5;
}

Coordinate calcIntersectionPoint( const Coordinate& p1, const Coordinate& p2, const Coordinate& p3, const Coordinate& p4 )
{
  double
    dxa = p1.x,
    dxb = p2.x,
    dxc = p3.x,
    dxd = p4.x;
  double
    dya = p1.y,
    dyb = p2.y,
    dyc = p3.y,
    dyd = p4.y;
//   calcBorderPoints( dxa, dya, dxb, dyb, Rect(0,0,500,500));
//   calcBorderPoints( dxc, dyc, dxd, dyd, Rect(0,0,500,500));

  long double xa=dxa;
  long double xb=dxb;
  long double xc=dxc;
  long double xd=dxd;

  long double ya=dya;
  long double yb=dyb;
  long double yc=dyc;
  long double yd=dyd;
  
  long double nx, ny;
  if ((fabsl(xb - xa)> 0.001) && (fabsl(xd - xc) > 0.001))
    {
      long double a = (yb-ya)/(xb-xa);
      long double b = (yd-yc)/(xd-xc);
      nx = (yc - ya + xa*a - xc*b)/(a-b);
      ny = (nx-xa)*a+ya;
    }
  else
    {
      // we would have had a divide by zero
      if ( fabsl( yb-ya ) > 0.001 && fabsl( yd-yc ) > 0.001 )
	{
	  // so now, we first calc the y values, and then the x's...
	  long double a = (xb-xa)/(yb-ya);
	  long double b = (xd-xc)/(yd-yc);
	  ny = ( xc - xa + ya*a - yc*b ) / (a-b);
	  nx = ( ny - ya)*a + xa;
	}
      else
	{
	  if ( fabsl(yb-ya) <= 0.001 )
	    {
//	    FIXME: what to do now ?
	      kdError() << "damn" << endl;
	    };
//          FIXME too: what here...
	  kdError() << "damn" << endl;
	};
    }
  return Coordinate( nx, ny );
};


void calcBorderPoints( Coordinate& p1, Coordinate& p2, const Rect& r )
{
  calcBorderPoints( p1.x, p1.y, p2.x, p2.y, r );
};

void calcBorderPoints( double& xa, double& ya, double& xb, double& yb, const Rect& r )
{
  // we calc where the line through a(xa,ya) and b(xb,yb) intersects with r:
  double left = (r.left()-xa)*(yb-ya)/(xb-xa)+ya;
  double right = (r.right()-xa)*(yb-ya)/(xb-xa)+ya;
  double top = (r.top()-ya)*(xb-xa)/(yb-ya)+xa;
  double bottom = (r.bottom()-ya)*(xb-xa)/(yb-ya)+xa;

  // now we go looking for valid points
  int novp = 0; // number of valid points we have already found

  if (!(top < r.left() || top > r.right())) {
    // the line intersects with the top side of the rect.
    ++novp;
    xa = top; ya = r.top();
  };
  if (!(left < r.bottom() || left > r.top())) {
    // the line intersects with the left side of the rect.
    if (novp++) { xb = r.left(); yb=left; }
    else { xa = r.left(); ya=left; };
  };
  if (!(right < r.bottom() || right > r.top())) {
    // the line intersects with the right side of the rect.
    if (novp++) { xb = r.right(); yb=right; }
    else { xa = r.right(); ya=right; };
  };
  if (!(bottom < r.left() || bottom > r.right())) {
    // the line intersects with the bottom side of the rect.
    ++novp;
    xb = bottom; yb = r.bottom();
  };
  if (novp < 2) {
    kdError()<< k_funcinfo << "novp < 2 :(( " << endl;
    xa = r.left(); ya=r.top(); xb=r.right(); yb=r.bottom();
  };
};
