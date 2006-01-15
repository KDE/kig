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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "common.h"

#include "../kig/kig_view.h"
#include "../objects/object_imp.h"

#include <cmath>

#include <kdebug.h>
#include <knumvalidator.h>
#include <klocale.h>
#if KDE_IS_VERSION( 3, 1, 90 )
#include <kinputdialog.h>
#else
#include <klineeditdlg.h>
#endif

Coordinate calcPointOnPerpend( const LineData& l, const Coordinate& t )
{
  return calcPointOnPerpend( l.b - l.a, t );
}

Coordinate calcPointOnPerpend( const Coordinate& dir, const Coordinate& t )
{
  return t + ( dir ).orthogonal();
}

Coordinate calcPointOnParallel( const LineData& l, const Coordinate& t )
{
  return calcPointOnParallel( l.b - l.a, t );
}

Coordinate calcPointOnParallel( const Coordinate& dir, const Coordinate& t )
{
  return t + dir*5;
}

Coordinate calcIntersectionPoint( const LineData& l1, const LineData& l2 )
{
  const Coordinate& pa = l1.a;
  const Coordinate& pb = l1.b;
  const Coordinate& pc = l2.a;
  const Coordinate& pd = l2.b;

  double
    xab = pb.x - pa.x,
    xdc = pd.x - pc.x,
    xac = pc.x - pa.x,
    yab = pb.y - pa.y,
    ydc = pd.y - pc.y,
    yac = pc.y - pa.y;

  double det = xab*ydc - xdc*yab;
  double detn = xac*ydc - xdc*yac;

  // test for parallelism
  if ( fabs (det) < 1e-6 ) return Coordinate::invalidCoord();
  double t = detn/det;

  return pa + t*(pb - pa);
}

void calcBorderPoints( Coordinate& p1, Coordinate& p2, const Rect& r )
{
  calcBorderPoints( p1.x, p1.y, p2.x, p2.y, r );
}

const LineData calcBorderPoints( const LineData& l, const Rect& r )
{
  LineData ret( l );
  calcBorderPoints( ret.a.x, ret.a.y, ret.b.x, ret.b.y, r );
  return ret;
}

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
}

void calcRayBorderPoints( const Coordinate& a, Coordinate& b, const Rect& r )
{
  calcRayBorderPoints( a.x, a.y, b.x, b.y, r );
}

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
}

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
}

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
}

bool isOnRay( const Coordinate& o, const Coordinate& a,
              const Coordinate& b, const double fault )
{
  return isOnLine( o, a, b, fault )
    // not too far in front of a horizontally..
//    && ( a.x - b.x < fault ) == ( a.x - o.x < fault )
    && ( ( a.x < b.x ) ? ( a.x - o.x < fault ) : ( a.x - o.x > -fault ) )
    // not too far in front of a vertically..
//    && ( a.y - b.y < fault ) == ( a.y - o.y < fault );
    && ( ( a.y < b.y ) ? ( a.y - o.y < fault ) : ( a.y - o.y > -fault ) );
}

bool isOnArc( const Coordinate& o, const Coordinate& c, const double r,
              const double sa, const double a, const double fault )
{
  if ( fabs( ( c - o ).length() - r ) > fault )
    return false;
  Coordinate d = o - c;
  double angle = atan2( d.y, d.x );

  if ( angle < sa ) angle += 2 * M_PI;
  return angle - sa - a < 1e-4;
}

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
                                          int side )
{
  Coordinate proj = calcPointProjection( c, l );
  Coordinate hvec = proj - c;
  Coordinate lvec = -l.dir();

  double sqdist = hvec.squareLength();
  double sql = sqr - sqdist;
  if ( sql < 0.0 )
    return Coordinate::invalidCoord();
  else
  {
    double l = sqrt( sql );
    lvec = lvec.normalize( l );
    lvec *= side;

    return proj + lvec;
  };
}

const Coordinate calcArcLineIntersect( const Coordinate& c, const double sqr,
                                       const double sa, const double angle,
                                       const LineData& l, int side )
{
  const Coordinate possiblepoint = calcCircleLineIntersect( c, sqr, l, side );
  if ( isOnArc( possiblepoint, c, sqrt( sqr ), sa, angle, test_threshold ) )
    return possiblepoint;
  else return Coordinate::invalidCoord();
}

const Coordinate calcPointProjection( const Coordinate& p,
                                      const LineData& l )
{
  Coordinate orth = l.dir().orthogonal();
  return p + orth.normalize( calcDistancePointLine( p, l ) );
}

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
}

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
}

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
}

double getDoubleFromUser( const QString& caption, const QString& label, double value,
                          QWidget* parent, bool* ok, double min, double max, int decimals )
{
#ifdef KIG_USE_KDOUBLEVALIDATOR
  KDoubleValidator vtor( min, max, decimals, 0, 0 );
#else
  KFloatValidator vtor( min, max, (QWidget*) 0, 0 );
#endif
#if KDE_IS_VERSION( 3, 1, 90 )
  QString input = KInputDialog::getText(
    caption, label, KGlobal::locale()->formatNumber( value, decimals ),
    ok, parent, "getDoubleFromUserDialog", &vtor );
#else
  QString input =
    KLineEditDlg::getText( caption, label,
                           KGlobal::locale()->formatNumber( value, decimals ),
                           ok, parent, &vtor );
#endif

  bool myok = true;
  double ret = KGlobal::locale()->readNumber( input, &myok );
  if ( ! myok )
    ret = input.toDouble( & myok );
  if ( ok ) *ok = myok;
  return ret;
}

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
    return a.invalidCoord();
    /* return (a+c)/2; */
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

//mp: the following test didn't work for vertical segments;
// fortunately the ieee floating point standard allows us to avoid
// the test altogether, since it would produce an infinity value that
// makes the final r.contains to fail
// in any case the corresponding test for a.y - b.y was missing.

//  if ( fabs( a.x - b.x ) <= 1e-7 )
//  {
//    // too small to be useful..
//    return r.contains( Coordinate( a.x, r.center().y ), miss );
//  }

// in case we have a segment we need also to check for the case when
// the segment is entirely contained in the rect, in which case the
// final tests all fail.
// it is ok to just check for the midpoint in the rect since:
// - if we have a segment completely contained in the rect this is true
// - if the midpoint is in the rect than returning true is safe (also
//   in the cases where we have a ray or a line)

  if ( r.contains( 0.5*( a + b ), miss ) ) return true;

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
}

bool operator==( const LineData& l, const LineData& r )
{
  return l.a == r.a && l.b == r.b;
}

bool LineData::isParallelTo( const LineData& l ) const
{
  const Coordinate& p1 = a;
  const Coordinate& p2 = b;
  const Coordinate& p3 = l.a;
  const Coordinate& p4 = l.b;

  double dx1 = p2.x - p1.x;
  double dy1 = p2.y - p1.y;
  double dx2 = p4.x - p3.x;
  double dy2 = p4.y - p3.y;

  return isSingular( dx1, dy1, dx2, dy2 );
}

bool LineData::isOrthogonalTo( const LineData& l ) const
{
  const Coordinate& p1 = a;
  const Coordinate& p2 = b;
  const Coordinate& p3 = l.a;
  const Coordinate& p4 = l.b;

  double dx1 = p2.x - p1.x;
  double dy1 = p2.y - p1.y;
  double dx2 = p4.x - p3.x;
  double dy2 = p4.y - p3.y;

  return isSingular( dx1, dy1, -dy2, dx2 );
}

bool areCollinear( const Coordinate& p1,
                   const Coordinate& p2, const Coordinate& p3 )
{
  return isSingular( p2.x - p1.x, p2.y - p1.y, p3.x - p1.x, p3.y - p1.y );
}

bool isSingular( const double& a, const double& b,
                 const double& c, const double& d )
{
  double det = a*d - b*c;
  double norm1 = std::fabs(a) + std::fabs(b);
  double norm2 = std::fabs(c) + std::fabs(d);

/*
 * test must be done relative to the magnitude of the two
 * row (or column) vectors!
 */
  return ( std::fabs(det) < test_threshold*norm1*norm2 );
}

const double double_inf = HUGE_VAL;
const double test_threshold = 1e-6;
