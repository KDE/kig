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

#include "kigpainter.h"

#include "../kig/kig_view.h"
#include "../objects/object.h"
#include "../objects/conic.h"
#include "../objects/cubic.h"
#include "common.h"
#include "coordinate_system.h"

#include <qpen.h>

#include <cmath>
#include <stack>

KigPainter::KigPainter( const ScreenInfo& si, QPaintDevice* device, bool no )
  : mP ( device ),
    msi( si ),
    mNeedOverlay( no )
{
}

KigPainter::~KigPainter()
{
}

void KigPainter::drawRect( const Rect& r )
{
  Rect rt = r.normalized();
  QRect qr = toScreen(rt);
  qr.normalize();
  mP.drawRect(qr);
  if( mNeedOverlay ) mOverlay.push_back( qr );
}

void KigPainter::drawRect( const QRect& r )
{
  mP.drawRect(r);
  if( mNeedOverlay ) mOverlay.push_back( r );
}

void KigPainter::drawCircle( const Coordinate& center, const double radius )
{
  Coordinate bottomLeft = center - Coordinate(radius, radius);
  Coordinate topRight = center + Coordinate(radius, radius);
  Rect r( bottomLeft, topRight );
  QRect qr = toScreen( r );
  mP.drawEllipse ( qr );
  if( mNeedOverlay ) circleOverlay( center, radius );
}

void KigPainter::drawSegment( const Coordinate& from, const Coordinate& to )
{
  QPoint tF = toScreen(from), tT = toScreen(to);
  mP.drawLine( tF, tT );
  if( mNeedOverlay ) segmentOverlay( from, to );
}

void KigPainter::drawFatPoint( const Coordinate& p, double radius )
{
  setBrushStyle( Qt::SolidPattern );
  Coordinate rad( radius, radius );
  rad /= 2;
  Coordinate tl = p - rad;
  Coordinate br = p + rad;
  Rect r( tl, br );
  QRect qr = toScreen( r );
  mP.drawEllipse( qr );
  if( mNeedOverlay ) mOverlay.push_back( qr );
}

void KigPainter::drawPoint( const Coordinate& p )
{
  mP.drawPoint( toScreen(p) );
  if( mNeedOverlay ) pointOverlay( p );
}

void KigPainter::drawLine( const Coordinate& p1, const Coordinate& p2 )
{
  drawLine( LineData( p1, p2 ) );
}

void KigPainter::drawText( const Rect p, const QString s, int textFlags, int len )
{
  QRect t = toScreen(p);
  int tf = textFlags;
  mP.drawText( t, tf, s, len );
  if( mNeedOverlay ) textOverlay( t, s, tf, len );
}

void KigPainter::textOverlay( const QRect& r, const QString s, int textFlags, int len )
{
  //  kdDebug() << Rect::fromQRect( mP.boundingRect( r, textFlags, s, len ) ) << endl;
  mOverlay.push_back( mP.boundingRect( r, textFlags, s, len ) );
}

const Rect KigPainter::boundingRect( const Rect& r, const QString s,
                                     int f, int l ) const
{
  return fromScreen( mP.boundingRect( toScreen( r ), f, s, l ) );
};

void KigPainter::setColor( const QColor& c )
{
  color = c;
  mP.setPen( QPen( color, width, style ) );
}

void KigPainter::setStyle( const PenStyle c )
{
  style = c;
  mP.setPen( QPen( color, width, style ) );
}

void KigPainter::setWidth( const uint c )
{
  width = c;
  mP.setPen( QPen( color, width, style ) );
}

void KigPainter::setPen( const QPen& p )
{
  color = p.color();
  width = p.width();
  style = p.style();
  mP.setPen(p);
}

void KigPainter::setBrush( const QBrush& b )
{
  brushStyle = b.style();
  brushColor = b.color();
  mP.setBrush( b );
}

void KigPainter::setBrushStyle( const BrushStyle c )
{
  brushStyle = c;
  mP.setBrush( QBrush( brushColor, brushStyle ) );
}

void KigPainter::setBrushColor( const QColor& c )
{
  brushColor = c;
  mP.setBrush( QBrush( brushColor, brushStyle ) );
}

void KigPainter::drawPolygon( const std::vector<Coordinate>& pts, bool winding, int index, int npoints )
{
  Rect sr;
  // i know this isn't really fast, but i blame it all on Qt with its
  // stupid container classes... what's wrong with the STL ?
  QPointArray t( pts.size() );
  int c = 0;
  for( std::vector<Coordinate>::const_iterator i = pts.begin(); i != pts.end(); ++i )
  {
    sr.setContains( *i );
    QPoint tt = toScreen (*i);
    t.putPoints( c++, 1, tt.x(), tt.y() );
  };
  mP.drawPolygon( t, winding, index, npoints );
  mOverlay.push_back( toScreen( sr ) );
}

Rect KigPainter::window()
{
  return msi.shownRect();
}

void KigPainter::circleOverlayRecurse( const Coordinate& centre,
				       double radiussq,
				       const Rect& cr )
{
  Rect currentRect = cr.normalized();

  if( !currentRect.intersects( window() ) ) return;

  // this code is an adaptation of Marc Bartsch's code, from KGeo
  Coordinate tl = currentRect.topLeft();
  Coordinate br = currentRect.bottomRight();
  Coordinate tr = currentRect.topRight();
  Coordinate bl = currentRect.bottomLeft();
  Coordinate c = currentRect.center();

  // mp: we compute the minimum and maximum distance from the center
  // of the circle and this rect
  double distxmin = 0, distxmax = 0, distymin = 0, distymax = 0;
  if ( centre.x >= tr.x ) distxmin = centre.x - tr.x;
  if ( centre.x <= bl.x ) distxmin = bl.x - centre.x;
  if ( centre.y >= tr.y ) distymin = centre.y - tr.y;
  if ( centre.y <= bl.y ) distymin = bl.y - centre.y;
  distxmax = fabs(centre.x - c.x) + currentRect.width()/2;
  distymax = fabs(centre.y - c.y) + currentRect.height()/2;
  // this should take into account the thickness of the line...
  distxmin -= pixelWidth();
  if (distxmin < 0) distxmin = 0;
  distxmax += pixelWidth();
  distymin -= pixelWidth();
  if (distymin < 0) distymin = 0;
  distymax += pixelWidth();
  double distminsq = distxmin*distxmin + distymin*distymin;
  double distmaxsq = distxmax*distxmax + distymax*distymax;

  // if the circle doesn't touch this rect, we return
  // too far from the centre
  if (distminsq > radiussq) return;

  // too near to the centre
  if (distmaxsq < radiussq) return;

  // the rect contains some of the circle
  // -> if it's small enough, we keep it
  if( currentRect.width() < overlayRectSize() ) {
    mOverlay.push_back( toScreen( currentRect) );
  } else {
    // this func works recursive: we subdivide the current rect, and if
    // it is of a good size, we keep it, otherwise we handle it again
    double width = currentRect.width() / 2;
    double height = currentRect.height() / 2;
    Rect r1 ( c, -width, -height);
    r1.normalize();
    circleOverlayRecurse(centre, radiussq, r1);
    Rect r2 ( c, width, -height);
    r2.normalize();
    circleOverlayRecurse(centre, radiussq, r2);
    Rect r3 ( c, -width, height);
    r3.normalize();
    circleOverlayRecurse(centre, radiussq, r3);
    Rect r4 ( c, width, height);
    r4.normalize();
    circleOverlayRecurse(centre, radiussq, r4);
  };
};

void KigPainter::circleOverlay( const Coordinate& centre, double radius )
{
  double t = radius + pixelWidth();
  Coordinate r( t, t );
  Coordinate bottomLeft = centre - r;
  Coordinate topRight = centre + r;
  Rect rect( bottomLeft, topRight );
  circleOverlayRecurse ( centre , radius*radius, rect );
}

void KigPainter::segmentOverlay( const Coordinate& p1, const Coordinate& p2 )
{
  // this code is based upon what Marc Bartsch wrote for KGeo

  // some stuff we may need:
  Coordinate p3 = p2 - p1;
  Rect border = window();
//  double length = p3.length();
  // mp: using the l-infinity distance is more natural here
  double length = fabs(p3.x);
  if ( fabs( p3.y ) > length ) length = fabs( p3.y );
  if ( length < pixelWidth() )
  {
    // hopefully prevent SIGZERO's
    mOverlay.push_back( toScreen( Rect( p1, p2 ) ) );
    return;
  };
  p3 *= overlayRectSize();
  p3 /= length;

  int counter = 0;

  Rect r(p1, p2);
  r.normalize();

  for (;;) {
    Rect tR( Coordinate( 0, 0 ), overlayRectSize(), overlayRectSize() );
    Coordinate tP = p1+p3*counter;
    tR.setCenter(tP);
    if (!tR.intersects(r))
    {
      //kdDebug()<< "stopped after "<< counter << " passes." << endl;
      break;
    }
    if (tR.intersects(border)) mOverlay.push_back( toScreen( tR ) );
    if (++counter > 100)
    {
      kdError()<< k_funcinfo << "counter got too big :( " << endl;
      break;
    }
  }
}

double KigPainter::overlayRectSize()
{
  return 20 * pixelWidth();
}

void KigPainter::pointOverlay( const Coordinate& p1 )
{
  Rect r( p1, 3*pixelWidth(), 3*pixelWidth());
  r.setCenter( p1 );
  mOverlay.push_back( toScreen( r) );
}

double KigPainter::pixelWidth()
{
  return msi.pixelWidth();
}

void KigPainter::setWholeWinOverlay()
{
  mOverlay.clear();
  mOverlay.push_back( mP.viewport() );
  // don't accept any more overlay's...
  mNeedOverlay = false;
}

QPoint KigPainter::toScreen( const Coordinate p ) const
{
  return msi.toScreen( p );
}

void KigPainter::drawGrid( const CoordinateSystem& c )
{
  c.drawGrid( *this );
  setWholeWinOverlay();
}

void KigPainter::drawObject( const Object* o, bool ss )
{
  o->drawWrap( *this, ss );
}

void KigPainter::drawObjects( const Objects& os )
{
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    drawObject( *i );
  };
}

void KigPainter::drawFilledRect( const QRect& r )
{
  QPen pen( Qt::black, 1, Qt::DotLine );
  setPen( pen );
  setBrush( QBrush( Qt::cyan, Dense6Pattern ) );
  drawRect( r.normalize() );
}

void KigPainter::drawTextStd( const QPoint& p, const QString& s )
{
  if ( ! s ) return;
  // tf = text formatting flags
  int tf = AlignLeft | AlignTop | DontClip | WordBreak;
  // we need the rect where we're going to paint text
  setPen(QPen(Qt::blue, 1, SolidLine));
  setBrush(Qt::NoBrush);
  drawText( Rect(
              msi.fromScreen(p), window().bottomRight()
              ).normalized(), s, tf );

}

QRect KigPainter::toScreen( const Rect r ) const
{
  return msi.toScreen( r );
}

void KigPainter::drawSimpleText( const Coordinate& c, const QString s )
{
  int tf = AlignLeft | AlignTop | DontClip | WordBreak;
  drawText( c, s, tf);
}

void KigPainter::drawText( const Coordinate p, const QString s,
                           int textFlags, int len )
{
  drawText( Rect( p, mP.window().right(), mP.window().top() ),
            s, textFlags, len );
}
const Rect KigPainter::simpleBoundingRect( const Coordinate& c, const QString s )
{
  int tf = AlignLeft | AlignTop | DontClip | WordBreak;
  return boundingRect( c, s, tf );
}

const Rect KigPainter::boundingRect( const Coordinate& c, const QString s,
                                     int f, int l ) const
{
  return boundingRect( Rect( c, mP.window().right(), mP.window().top() ),
                       s, f, l );
}

Coordinate KigPainter::fromScreen( const QPoint& p ) const
{
  return msi.fromScreen( p );
}

Rect KigPainter::fromScreen( const QRect& r ) const
{
  return msi.fromScreen( r );
}

void KigPainter::drawRay( const Coordinate& a, const Coordinate& b )
{
  Coordinate tb = b;
  calcRayBorderPoints( a, tb, window() );
  drawSegment( a, tb );
}

void KigPainter::drawAngle( const Rect& surroundingRect, int startAngle, int angle )
{
  mP.drawArc( toScreen( surroundingRect ), startAngle, angle );
  Rect r = surroundingRect;
  r *= 1.2;
  r.setCenter( surroundingRect.center() );
//  if ( mNeedOverlay ) mOverlay.push_back( toScreen( r ) );
  setWholeWinOverlay();
}

void KigPainter::drawFatPoint( const Coordinate& p )
{
  drawFatPoint( p, 5 * pixelWidth() );
}

inline Coordinate conicGetCoord( double theta, double ecostheta0,
                                 double esintheta0, double pdimen,
                                 const Coordinate& focus1 )
{
  // TODO: eliminate some cos() and sin() calls with a table...
  double costheta = cos( theta );
  double sintheta = sin( theta );
  double ecosthetamtheta = costheta*ecostheta0 + sintheta*esintheta0;
  double rho = pdimen / (1.0 - ecosthetamtheta);
  return focus1 + rho * Coordinate( costheta, sintheta );
};

typedef std::pair<double,Coordinate> coordparampair;

struct workitem
{
  workitem( coordparampair f, coordparampair s, Rect *o) :
    first(f), second(s), overlay(o) {};
  coordparampair first;
  coordparampair second;
  Rect		 *overlay;
};
//typedef std::pair<coordparampair, coordparampair> workitem;

void KigPainter::drawConic( const ConicPolarEquationData& data )
{
  // @author Maurizio Paolini wrote an original version of this, ( the
  // math in conicGetCoord() is still his ), I ( Dominique ) adapted
  // it afterwards to use a distribution mechanism as explained
  // below. )

  // this function works more or less like Locus::calcForWidget():
  // we calc some initial points, draw them, and then keep checking if
  // the point with the param in between two other points.  If the
  // point with that param is not too close to one of them, and if it
  // is in the current window(), then we draw it, and put it on the
  // stack for further processing...

  Coordinate focus1 = data.focus1;
  double pdimen = data.pdimen;
  double ecostheta0 = data.ecostheta0;
  double esintheta0 = data.esintheta0;

  // we manage our own overlay
  bool tNeedOverlay = mNeedOverlay;
  mNeedOverlay = false;

  // this stack contains pairs of Coordinates that we still need to
  // process:
  std::stack<workitem> workstack;
  // mp: this stack contains all the generated overlays:
  // the strategy for generating the overlay structure is the same
  // recursive-like used to draw the segments: a new rectangle is
  // generated whenever the length of a segment becomes lower than
  // overlayRectSize(), or if the segment would be draw anyway
  // to avoid strange things happening we impose that the distance
  // in parameter space be less than a threshold before generating
  // any overlay.
  //
  // The third parameter in workitem is a pointer into a stack of
  // all generated rectangles (in real coordinate space); if 0
  // there is no rectangles associated to that segment yet.
  //
  // Using the final mOverlay stack would be much more efficient, but
  // 1. needs transformations into window space
  // 2. would be more difficult to drop rectangles not intersecting
  //    the window.
  std::stack<Rect> overlaystack;

  // mp: the original version in which an initial set of 20 intervals
  // were pushed onto the stack is replaced by a single interval and
  // by forcing subdivision till h < hmax (with more or less the same
  // final result).
  // First push the [0,2*pi] interval into the stack:
  workstack.push( workitem(
                    coordparampair( 0, conicGetCoord( 0, ecostheta0, esintheta0,
                                                      pdimen, focus1 ) ),
                    coordparampair( 2*M_PI, conicGetCoord( 2*M_PI, ecostheta0, esintheta0,
                                                           pdimen, focus1 ) ),
                    0 ) );

  // maxlength is the square of the maximum size that we allow
  // between two points..
  double maxlength = 1.5 * pixelWidth();
  maxlength *= maxlength;
  // error squared is required to be less that sigma (half pixel)
  double sigma = maxlength/4;
  // distance between two parameter values cannot be too small
  double hmin = 1e-4;
  // distance between two parameter values cannot be too large
  double hmax = M_PI/20;
  double hmaxoverlay = M_PI/4;

  int count = 1;               // the number of segments we've already
                               // visited...
  static const int maxnumberofpoints = 1000;

  const Rect& sr = window();

  // we don't use recursion, but a stack based approach for efficiency
  // concerns...
  while ( ! workstack.empty() && count < maxnumberofpoints )
  {
    workitem curitem = workstack.top();
    workstack.pop();
    bool curitemok = true;
    while ( curitemok && count++ < maxnumberofpoints )
    {
      // we take the middle parameter of the two previous points...
      Coordinate p0 = curitem.first.second;
      Coordinate p1 = curitem.second.second;
      double t2 = ( curitem.first.first + curitem.second.first ) / 2;
      double h = fabs( curitem.second.first - curitem.first.first ) /2;
      Rect *overlaypt = curitem.overlay;
      Coordinate p2 = conicGetCoord( t2, ecostheta0, esintheta0,
                                     pdimen, focus1 );
      bool dooverlay = ! overlaypt && h < hmaxoverlay
 && fabs( p0.x - p1.x ) <= overlayRectSize()
 && fabs( p0.y - p1.y ) <= overlayRectSize();
      bool addn = sr.contains( p2 );
      // estimated error between the curve and the segments
      double errsq = (0.5*p0 + 0.5*p1 - p2).squareLength();
      errsq /= 4;
      curitemok = false;
      bool dodraw = h < hmax && ( errsq < sigma || h < hmin );
      if ( tNeedOverlay && ( dooverlay || dodraw ) )
      {
        Rect newoverlay( p0, p1 );
        overlaystack.push( newoverlay );
        overlaypt = &overlaystack.top();
      }
      if ( overlaypt ) overlaypt->setContains( p2 );
      if ( dodraw )
      {
        // draw the two segments
        QPoint tp0 = toScreen(p0);
        QPoint tp1 = toScreen(p1);
        QPoint tp2 = toScreen(p2);
        mP.drawLine( tp0, tp2 );
        mP.drawLine( tp2, tp1 );
      }
      else
      {
        // push into stack in order to process both subintervals
        if (addn || sr.contains( p0 ) || h >= hmax)
          workstack.push( workitem( curitem.first, coordparampair( t2, p2 ),
                                    overlaypt ) );
        if (addn || sr.contains( p1 ) || h >= hmax)
        {
          curitem = workitem( coordparampair( t2, p2 ), curitem.second ,
                              overlaypt );
          curitemok = true;
        }
      }
    }
  }

  assert ( tNeedOverlay || overlaystack.empty() );
  if ( tNeedOverlay )
  {
    Rect border = window();
    while ( ! overlaystack.empty() )
    {
      Rect overlay = overlaystack.top();
      overlaystack.pop();
      if (overlay.intersects( border ))
        mOverlay.push_back( toScreen( overlay ) );
    }
  }
  mNeedOverlay = tNeedOverlay;
}

void KigPainter::drawCubicRecurse (
                   double& xleft, double& yleft, bool& validleft,
                   int& numrootsleft,
                   double &xright, double &yright, bool &validright,
                   int &numrootsright,
                   const CubicCartesianEquationData &data, int &root,
                   double &ymin, double &ymax, double &tol,
                   bool& tNeedOverlay, Rect& overlay)
{
  double ltol = tol;
  if ( ! ( validleft && validright && numrootsleft == numrootsright ) )
    // in questo caso la tolleranza e' molto minore
    ltol /= 100;
  if ( xright - xleft < ltol )
  {
    if ( validleft && validright && numrootsleft == numrootsright )
    {
      /* draw the segment */
      Coordinate pleft = Coordinate( xleft, yleft );
      Coordinate pright = Coordinate( xright, yright );
      QPoint tpleft = toScreen(pleft);
      QPoint tpright = toScreen(pright);
      mP.drawLine( tpleft, tpright );
    }
  } else {
    double xmiddle = (xright + xleft)/2;
    bool validmiddle;
    int numrootsmiddle;
    double ymiddle = calcCubicYvalue ( xmiddle, ymin, ymax, root, data,
                   validmiddle, numrootsmiddle );
    Coordinate pmiddle = Coordinate( xmiddle, ymiddle );
    if ( validmiddle && tNeedOverlay ) overlay.setContains( pmiddle );
    drawCubicRecurse ( xleft, yleft, validleft, numrootsleft,
                   xmiddle, ymiddle, validmiddle, numrootsmiddle,
                   data, root, ymin, ymax, tol, tNeedOverlay, overlay );
    drawCubicRecurse ( xmiddle, ymiddle, validmiddle, numrootsmiddle,
                   xright, yright, validright, numrootsright,
                   data, root, ymin, ymax, tol, tNeedOverlay, overlay );
  }
}

void KigPainter::drawCubic( const CubicCartesianEquationData& data )
{
  // we manage our own overlay
  bool tNeedOverlay = mNeedOverlay;
  mNeedOverlay = false;
  Rect border = window();
  Rect overlay;

  double ymin = border.bottom();
  double ymax = border.top();
  bool validleft, validright;
  int numrootsleft, numrootsright;

  double xleft = border.left();
  double xright = border.right();
  double tol = (xright - xleft)/100;
  for ( int root = 1; root <= 3; root++ )
  {
    double yleft = calcCubicYvalue ( xleft, ymin, ymax, root, data,
                   validleft, numrootsleft );
    double yright = calcCubicYvalue ( xright, ymin, ymax, root, data,
                   validright, numrootsright );
    Coordinate p = Coordinate( xleft, yleft );
    if ( validleft && tNeedOverlay ) overlay.setContains( p );
    p = Coordinate( xright, yright );
    if ( validright && tNeedOverlay ) overlay.setContains( p );
    drawCubicRecurse ( xleft, yleft, validleft, numrootsleft,
                   xright, yright, validright, numrootsright,
                   data, root, ymin, ymax, tol,
                   tNeedOverlay, overlay);
  }
  if ( tNeedOverlay ) mOverlay.push_back( toScreen( overlay ) );
  mNeedOverlay = tNeedOverlay;
}

void KigPainter::drawLine( const LineData& d )
{
  LineData l = calcBorderPoints( d, window() );
  drawSegment( l );
}

void KigPainter::drawSegment( const LineData& d )
{
  drawSegment( d.a, d.b );
}
