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


#include "common.h"

#include <cmath>

#include <kdebug.h>

Coordinate calcPointOnPerpend( const Coordinate& p1, const Coordinate& p2, const Coordinate& t )
{
  return calcPointOnPerpend( p2 - p1, t );
};

Coordinate calcPointOnPerpend( const Coordinate& dir, const Coordinate& t )
{
  return t + ( dir ).orthogonal();
};

Coordinate calcPointOnParallel( const Coordinate& p1, const Coordinate& p2, const Coordinate& t )
{
  return calcPointOnParallel( p2 - p1, t );
}

Coordinate calcPointOnParallel( const Coordinate& dir, const Coordinate& t )
{
  return t + dir*5;
};

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

  long double xa=dxa;
  long double xb=dxb;
  long double xc=dxc;
  long double xd=dxd;

  long double ya=dya;
  long double yb=dyb;
  long double yc=dyc;
  long double yd=dyd;

  long double nx, ny;
  if ((fabsl(xb - xa)> 0.00001) && (fabsl(xd - xc) > 0.00001))
  {
    // we calc these separately to avoid rounding problems..
//       long double a = (yb-ya)/(xb-xa);
//       long double b = (yd-yc)/(xd-xc);
//      nx = (yc - ya + xa*a - xc*b)/(a-b);
//      ny = (nx-xa)*a + ya;
    // now doesn't this look nice ? :)
    nx = (yc - ya +
          xa * ( yb - ya ) / ( xb - xa ) -
          xc * ( yd - yc ) / ( xd-xc) )
         * ( xb * xd -
             xb * xc -
             xa * xd +
             xa * xc )
         / ( yb * xd -
             yb * xc -
             ya * xd +
             ya * xc
             - ( yd * xb -
                 yd * xa -
                 yc * xb +
                 yc * xa ) );
    ny = (nx-xa)*(yb-ya)/(xb-xa)+ya;
  }
  else // we would have had a divide by zero
    if ( fabsl( yb-ya ) > 0.00001 && fabsl( yd-yc ) > 0.00001 )
    {
      // so now, we first calc the y values, and then the x's...
      long double a = (xb-xa)/(yb-ya);
      long double b = (xd-xc)/(yd-yc);
      ny = ( xc - xa + ya*a - yc*b ) / (a-b);
      nx = ( ny - ya)*a + xa;
    }
    else if ( fabsl(yb-ya) > 0.00001 && fabsl( xd - xc ) > 0.00001 )
    {
      nx = xb; ny = yd;
    }
    else if ( fabsl( yd - yc ) > 0.00001 && fabsl( xb - xa ) > 0.00001 )
    {
      nx = xd; ny = yb;
    }
    else if ( fabsl( yd - yc ) <= 0.00001 && fabsl( xd - xc ) <= 0.00001 )
    {
      // this is a bogus value, but better that than a SIGNULL,
      // right ?
      nx = xd; ny = yd;
    }
    else
    {
      // same here
      nx = xb; ny = yb;
    };
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
    // line is completely outside of the window...
    xa = ya = xb = yb = 0;
  };
};

void calcRayBorderPoints( const Coordinate& a, Coordinate& b, const Rect& r )
{
  calcRayBorderPoints( a.x, a.y, b.x, b.y, r );
};

void calcRayBorderPoints( const double xa, const double ya, double& xb,
                          double& yb, const Rect& r )
{
  // we calc where the line through a(xa,ya) and b(xb,yb) intersects with r:
  double left = (r.left()-xa)*(yb-ya)/(xb-xa)+ya;
  double right = (r.right()-xa)*(yb-ya)/(xb-xa)+ya;
  double top = (r.top()-ya)*(xb-xa)/(yb-ya)+xa;
  double bottom = (r.bottom()-ya)*(xb-xa)/(yb-ya)+xa;

  // now we see which we can use...
  if(
    // the ray intersects with the top side of the rect..
    top >= r.left() && top <= r.right()
    // and b is above a
    && yb > ya )
  {
    xb = top;
    yb = r.top();
    return;
  };
  if(
    // the ray intersects with the left side of the rect...
    left >= r.bottom() && left <= r.top()
    // and b is on the left of a..
    && xb < xa )
  {
    xb = r.left();
    yb=left;
    return;
  };
  if (
    // the ray intersects with the right side of the rect...
    right >= r.bottom() && right <= r.top()
    // and b is to the right of a..
    && xb > xa )
  {
    xb = r.right();
    yb=right;
    return;
  };
  if(
    // the ray intersects with the bottom side of the rect...
    bottom >= r.left() && bottom <= r.right()
    // and b is under a..
    && yb < ya ) {
    xb = bottom;
    yb = r.bottom();
    return;
  };
  kdError() << k_funcinfo << "damn" << endl;
};

bool isOnLine( const Coordinate o, const Coordinate a,
               const Coordinate b, const double fault )
{
  double x1 = a.x;
  double y1 = a.y;
  double x2 = b.x;
  double y2 = b.y;

  // check your math theory ( homogeneous coördinates ) for this
  double tmp = fabs( o.x * (y1-y2) + o.y*(x2-x1) + (x1*y2-y1*x2) );
  return tmp < ( fault * (b-a).length());
  // if o is on the line ( if the determinant of the matrix
  //       |---|---|---|
  //       | x | y | z |
  //       |---|---|---|
  //       | x1| y1| z1|
  //       |---|---|---|
  //       | x2| y2| z2|
  //       |---|---|---|
  // equals 0, then p(x,y,z) is on the line containing points
  // p1(x1,y1,z1) and p2 here, we're working with normal coords, no
  // homogeneous ones, so all z's equal 1
};

bool isOnSegment( const Coordinate o, const Coordinate a,
                  const Coordinate b, const double fault )
{
  return isOnLine( o, a, b, fault )
    // not too far to the right
    && (o.x - kigMax(a.x,b.x) < fault )
    // not too far to the left
    && ( kigMin (a.x, b.x) - o.x < fault )
    // not too high
    && ( kigMin (a.y, b.y) - o.y < fault )
    // not too low
    && ( o.y - kigMax (a.y, b.y) < fault );
};

bool isOnRay( const Coordinate o, const Coordinate a,
              const Coordinate b, const double fault )
{
  return isOnLine( o, a, b, fault )
    // not too far in front of a horizontally..
    && ( a.x - b.x < fault ) == ( a.x - o.x < fault )
    // not too far in front of a vertically..
    && ( a.y - b.y < fault ) == ( a.y - o.y < fault );
};

const Coordinate calcMirrorPoint( const Coordinate& a, const Coordinate& b,
                                  const Coordinate& p )
{
  Coordinate m =
    calcIntersectionPoint( a, b, p,
                           calcPointOnPerpend( a, b, p ) );
  return 2*m - p;
}

const Coordinate calcCircleLineIntersect( const Coordinate& c,
                                          const double sqr,
                                          const Coordinate& a,
                                          const Coordinate& b,
                                          bool side, bool& valid )
{
  Coordinate proj = calcPointProjection( c, a, b );
  Coordinate hvec = proj - c;
  Coordinate lvec = a - b;

  double sqdist = hvec.squareLength();
  double sql = sqr - sqdist;
  if ( sql < 0.0 )
  {
    valid = false;
    return Coordinate();
  }
  else
  {
    valid = true;
    double l = sqrt( sql );
    lvec = lvec.normalize( l );
    lvec *= side ? 1 : -1;

    return proj + lvec;
  };
};

const Coordinate calcConicLineIntersect( const double* coeffs,
                                         const Coordinate& a,
                                         const Coordinate& b,
                                         int which, bool& valid )
{
  double aa = coeffs[0];
  double bb = coeffs[1];
  double cc = coeffs[2];
  double dd = coeffs[3];
  double ee = coeffs[4];
  double ff = coeffs[5];

  double x = a.x;
  double y = a.y;
  double dx = b.x - a.x;
  double dy = b.y - a.y;

  double aaa = aa*dx*dx + bb*dy*dy + cc*dx*dy;
  double bbb = 2*aa*x*dx + 2*bb*y*dy + cc*x*dy + cc*y*dx + dd*dx + ee*dy;
  double ccc = aa*x*x + bb*y*y + cc*x*y + dd*x + ee*y + ff;

  double discrim = bbb*bbb - 4*aaa*ccc;
  if (discrim < 0.0)
  {
    valid = false;
    return Coordinate();
  }
  else
  {
    valid = true;
    double t = -bbb + which*sqrt(discrim);
    t /= 2*aaa;

    return a + t*(b - a);
  }
}

const Coordinate calcPointProjection( const Coordinate& p, const Coordinate& a,
                                      const Coordinate& b )
{
  Coordinate vect = b - a;
  Coordinate orth = vect.orthogonal();
  return p + orth.normalize( calcDistancePointLine( p, a, b ) );
};

double calcDistancePointLine( const Coordinate& p, const Coordinate& a,
                              const Coordinate& b )
{
  double xa = a.x;
  double ya = a.y;
  double xb = b.x;
  double yb = b.y;
  double x = p.x;
  double y = p.y;
  Coordinate dir = b - a;
  double norm = dir.length();
  return ( yb * x - ya * x - xb * y + xa * y + xb * ya - yb * xa ) / norm;
};

Coordinate calcRotatedPoint( const Coordinate& a, const Coordinate& c, const double arc )
{
  // we take a point p on a line through c and parallel with the
  // X-axis..
  Coordinate p( c.x + 5, c.y );
  // we then calc the arc that ac forms with cp...
  Coordinate d = a - c;
  d = d.normalize();
  double aarc = std::acos( d.x );
  if ( d.y < 0 ) aarc = 2*M_PI - aarc;

  // we now take the sum of the two arcs to find the arc between
  // pc and ca
  double asum = aarc + arc;

  Coordinate ret( std::cos( asum ), std::sin( asum ) );
  ret = ret.normalize( ( a -c ).length() );
  return ret + c;
};
