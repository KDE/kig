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

#include "../kig/kig_view.h"
#include "../objects/object_imp.h"

#include <cmath>

#include <kdebug.h>
#include <knumvalidator.h>
#include <klineeditdlg.h>
#include <klocale.h>

Coordinate calcPointOnPerpend( const LineData& l, const Coordinate& t )
{
  return calcPointOnPerpend( l.b - l.a, t );
};

Coordinate calcPointOnPerpend( const Coordinate& dir, const Coordinate& t )
{
  return t + ( dir ).orthogonal();
};

Coordinate calcPointOnParallel( const LineData& l, const Coordinate& t )
{
  return calcPointOnParallel( l.b - l.a, t );
}

Coordinate calcPointOnParallel( const Coordinate& dir, const Coordinate& t )
{
  return t + dir*5;
};

Coordinate calcIntersectionPoint( const LineData& l1, const LineData& l2 )
{
  const Coordinate& p1 = l1.a;
  const Coordinate& p2 = l1.b;
  const Coordinate& p3 = l2.a;
  const Coordinate& p4 = l2.b;

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
  if ((fabs(xb - xa)> 0.00001) && (fabs(xd - xc) > 0.00001))
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
    if ( fabs( yb-ya ) > 0.00001 && fabs( yd-yc ) > 0.00001 )
    {
      // so now, we first calc the y values, and then the x's...
      long double a = (xb-xa)/(yb-ya);
      long double b = (xd-xc)/(yd-yc);
      ny = ( xc - xa + ya*a - yc*b ) / (a-b);
      nx = ( ny - ya)*a + xa;
    }
    else if ( fabs(yb-ya) > 0.00001 && fabs( xd - xc ) > 0.00001 )
    {
      nx = xb; ny = yd;
    }
    else if ( fabs( yd - yc ) > 0.00001 && fabs( xb - xa ) > 0.00001 )
    {
      nx = xd; ny = yb;
    }
    else if ( fabs( yd - yc ) <= 0.00001 && fabs( xd - xc ) <= 0.00001 )
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

const LineData calcBorderPoints( const LineData& l, const Rect& r )
{
  LineData ret( l );
  calcBorderPoints( ret.a.x, ret.a.y, ret.b.x, ret.b.y, r );
  return ret;
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

bool isOnLine( const Coordinate& o, const Coordinate& a,
               const Coordinate& b, const double fault )
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

bool isOnSegment( const Coordinate& o, const Coordinate& a,
                  const Coordinate& b, const double fault )
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

bool isOnRay( const Coordinate& o, const Coordinate& a,
              const Coordinate& b, const double fault )
{
  return isOnLine( o, a, b, fault )
    // not too far in front of a horizontally..
    && ( a.x - b.x < fault ) == ( a.x - o.x < fault )
    // not too far in front of a vertically..
    && ( a.y - b.y < fault ) == ( a.y - o.y < fault );
};

const Coordinate calcMirrorPoint( const LineData& l,
                                  const Coordinate& p )
{
  Coordinate m =
    calcIntersectionPoint( l,
                           LineData( p,
                                     calcPointOnPerpend( l, p )
                             )
      );
  return 2*m - p;
}

const Coordinate calcCircleLineIntersect( const Coordinate& c,
                                          const double sqr,
                                          const LineData& l,
                                          int side, bool& valid )
{
  Coordinate proj = calcPointProjection( c, l );
  Coordinate hvec = proj - c;
  Coordinate lvec = -l.dir();

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
    lvec *= side;

    return proj + lvec;
  };
};

const Coordinate calcPointProjection( const Coordinate& p,
                                      const LineData& l )
{
  Coordinate orth = l.dir().orthogonal();
  return p + orth.normalize( calcDistancePointLine( p, l ) );
};

double calcDistancePointLine( const Coordinate& p,
                              const LineData& l )
{
  double xa = l.a.x;
  double ya = l.a.y;
  double xb = l.b.x;
  double yb = l.b.y;
  double x = p.x;
  double y = p.y;
  double norm = l.dir().length();
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

Coordinate calcCircleRadicalStartPoint( const Coordinate& ca, const Coordinate& cb,
                                        double sqra, double sqrb )
{
  Coordinate direc = cb - ca;
  Coordinate m = (ca + cb)/2;

  double dsq = direc.squareLength();
  double lambda = dsq == 0.0 ? 0.0
                  : (sqra - sqrb) / (2*dsq);

  direc *= lambda;
  return m + direc;
};

double getDoubleFromUser( const QString& caption, const QString& label, double value,
                          QWidget* parent, bool* ok, double min, double max, int decimals )
{
#ifdef KIG_USE_KDOUBLEVALIDATOR
  KDoubleValidator vtor( min, max, decimals, 0, 0 );
#else
  KFloatValidator vtor( min, max, (QWidget*) 0, 0 );
#endif
  QString input =
    KLineEditDlg::getText( caption, label,
                           KGlobal::locale()->formatNumber( value, decimals ),
                           ok, parent, &vtor );
  bool myok = true;
  double ret = KGlobal::locale()->readNumber( input, &myok );
  if ( ! myok )
    ret = input.toDouble( & myok );
  if ( ok ) *ok = myok;
  return ret;
};

const Coordinate calcCenter(
  const Coordinate& a, const Coordinate& b, const Coordinate& c )
{
  // this algorithm is written by my brother, Christophe Devriese
  // <oelewapperke@ulyssis.org> ...
  // I don't get it myself :)

  double xdo = b.x-a.x;
  double ydo = b.y-a.y;

  double xao = c.x-a.x;
  double yao = c.y-a.y;

  double a2 = xdo*xdo + ydo*ydo;
  double b2 = xao*xao + yao*yao;

  double numerator = (xdo * yao - xao * ydo);
  if ( numerator == 0 )
  {
    // problem:  xdo * yao == xao * ydo <=> xdo/ydo == xao / yao
    // this means that the lines ac and ab have the same direction,
    // which means they're the same line..
    // FIXME: i would normally throw an error here, but KDE doesn't
    // use exceptions, so i'm returning a bogus point :(
    return (a+c)/2;
  };
  double denominator = 0.5 / numerator;

  double centerx = a.x - (ydo * b2 - yao * a2) * denominator;
  double centery = a.y + (xdo * b2 - xao * a2) * denominator;

  return Coordinate(centerx, centery);
}

bool lineInRect( const Rect& r, const Coordinate& a, const Coordinate& b,
                 const int width, const ObjectImp* imp, const KigWidget& w )
{
  double miss = w.screenInfo().normalMiss( width );
  if ( fabs( a.x - b.x ) <= 1e-7 )
  {
    // too small to be useful..
    return r.contains( Coordinate( a.x, r.center().y ), miss );
  }
  Coordinate dir = b - a;
  double m = dir.y / dir.x;
  double lefty = a.y + m * ( r.left() - a.x );
  double righty = a.y + m * ( r.right() - a.x );
  double minv = dir.x / dir.y;
  double bottomx = a.x + minv * ( r.bottom() - a.y );
  double topx = a.x + minv * ( r.top() - a.y );

  // these are the intersections between the line, and the lines
  // defined by the sides of the rectangle.
  Coordinate leftint( r.left(), lefty );
  Coordinate rightint( r.right(), righty );
  Coordinate bottomint( bottomx, r.bottom() );
  Coordinate topint( topx, r.top() );

  // For each intersection, we now check if we contain the
  // intersection ( this might not be the case for a segment, when the
  // intersection is not between the begin and end point.. ) and if
  // the rect contains the intersection..  If it does, we have a winner..
  return
    ( imp->contains( leftint, width, w ) && r.contains( leftint, miss ) ) ||
    ( imp->contains( rightint, width, w ) && r.contains( rightint, miss ) ) ||
    ( imp->contains( bottomint, width, w ) && r.contains( bottomint, miss ) ) ||
    ( imp->contains( topint, width, w ) && r.contains( topint, miss ) );
};

bool operator==( const LineData& l, const LineData& r )
{
  return l.a == r.a && l.b == r.b;
};

const double double_inf = HUGE_VAL;

