/**
   This file is part of Kig, a KDE program for Interactive Geometry...
   Copyright (C) 2002-2003  Dominique Devriese <devriese@kde.org>

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

#include "kigpainter.h"

#include "../kig/kig_view.h"
#include "../kig/kig_document.h"
#include "../misc/goniometry.h"
#include "../objects/object_holder.h"
#include "../objects/curve_imp.h"
#include "../objects/point_imp.h"
#include "object_hierarchy.h"
#include "common.h"
#include "conic-common.h"
#include "cubic-common.h"
#include "coordinate_system.h"

#include <qpen.h>

#include <cmath>
#include <stack>
#include <functional>
#include <algorithm>

KigPainter::KigPainter( const ScreenInfo& si, QPaintDevice* device,
                        const KigDocument& doc, bool no )
  : mP ( device ),
    color( Qt::blue ),
    style( Qt::SolidLine ),
    pointstyle( 0 ),
    width( -1 ),
    brushStyle( Qt::NoBrush ),
    brushColor( Qt::blue ),
    mdoc( doc ),
    msi( si ),
    mNeedOverlay( no ),
    overlayenlarge( 0 )
{
  mP.setBackgroundColor( Qt::white );
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

void KigPainter::drawFatPoint( const Coordinate& p )
{
  int twidth = width == -1 ? 5 : width;
  mP.setPen( QPen( color, 1, style ) );
  switch ( pointstyle )
  {
    case 0:
    {
      double radius = twidth * pixelWidth();
      setBrushStyle( Qt::SolidPattern );
      Coordinate rad( radius, radius );
      rad /= 2;
      Coordinate tl = p - rad;
      Coordinate br = p + rad;
      Rect r( tl, br );
      QRect qr = toScreen( r );
      mP.drawEllipse( qr );
      if( mNeedOverlay ) mOverlay.push_back( qr );
      break;
    }
    case 1:
    {
      double radius = twidth * pixelWidth();
      setBrushStyle( Qt::NoBrush );
      Coordinate rad( radius, radius );
      rad /= 2;
      Coordinate tl = p - rad;
      Coordinate br = p + rad;
      Rect r( tl, br );
      QRect qr = toScreen( r );
      mP.drawEllipse( qr );
      if( mNeedOverlay ) mOverlay.push_back( qr );
      break;
    }
    case 2:
    {
      double radius = twidth * pixelWidth();
      Coordinate rad( radius, radius );
      rad /= 2;
      Coordinate tl = p - rad;
      Coordinate br = p + rad;
      Rect r( tl, br );
      QRect qr = toScreen( r );
      mP.drawRect( qr );
      mP.fillRect( qr, QBrush( color, Qt::SolidPattern ) );
      if( mNeedOverlay ) mOverlay.push_back( qr );
      break;
    }
    case 3:
    {
      double radius = twidth * pixelWidth();
      Coordinate rad( radius, radius );
      rad /= 2;
      Coordinate tl = p - rad;
      Coordinate br = p + rad;
      Rect r( tl, br );
      QRect qr = toScreen( r );
      mP.drawRect( qr );
      if( mNeedOverlay ) mOverlay.push_back( qr );
      break;
    }
    case 4:
    {
      double radius = twidth * pixelWidth();
      Coordinate rad( radius, radius );
      rad /= 2;
      Coordinate tl = p - rad;
      Coordinate br = p + rad;
      Rect r( tl, br );
      QRect qr = toScreen( r );
      mP.setPen( QPen( color, 2 ) );
      mP.drawLine( qr.topLeft(), qr.bottomRight() );
      mP.drawLine( qr.topRight(), qr.bottomLeft() );
      if( mNeedOverlay ) mOverlay.push_back( qr );
      break;
    }
  }
  mP.setPen( QPen( color, twidth, style ) );
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
  t.moveBy( 2, 2 );
  t.setWidth( t.width() - 4 );
  t.setHeight( t.height() - 4 );
  mP.drawText( t, tf, s, len );
  if( mNeedOverlay ) textOverlay( t, s, tf, len );
}

void KigPainter::textOverlay( const QRect& r, const QString s, int textFlags, int len )
{
  //  kdDebug() << Rect::fromQRect( mP.boundingRect( r, textFlags, s, len ) ) << endl;
  QRect newr( mP.boundingRect( r, textFlags, s, len ) );
  newr.setWidth( newr.width() + 4 );
  newr.setHeight( newr.height() + 4 );
  mOverlay.push_back( newr );
}

const Rect KigPainter::boundingRect( const Rect& r, const QString s,
                                     int f, int l ) const
{
  QRect qr = mP.boundingRect( toScreen( r ), f, s, l );
  qr.setWidth( qr.width() + 4 );
  qr.setHeight( qr.height() + 4 );
  return fromScreen( qr );
}

void KigPainter::setColor( const QColor& c )
{
  color = c;
  mP.setPen( QPen( color, width == -1 ? 1 : width, style ) );
}

void KigPainter::setStyle( const PenStyle c )
{
  style = c;
  mP.setPen( QPen( color, width == -1 ? 1 : width, style ) );
}

void KigPainter::setWidth( const int c )
{
  width = c;
  if (c > 0) overlayenlarge = c - 1;
  mP.setPen( QPen( color, width == -1 ? 1 : width, style ) );
}

void KigPainter::setPointStyle( const int p )
{
  pointstyle = p;
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

bool KigPainter::getNightVision( ) const
{
  return mdoc.getNightVision();
}

QColor KigPainter::getColor() const
{
  return color;
}

/*
static void setContains( QRect& r, const QPoint& p )
{
  if ( r.left() > p.x() ) r.setLeft( p.x() );
  if ( r.right() < p.x() ) r.setRight( p.x() );
  // this is correct, i think.  In qt the bottom has the highest y
  // coord...
  if ( r.bottom() > p.y() ) r.setBottom( p.y() );
  if ( r.top() < p.y() ) r.setTop( p.y() );
}
*/

void KigPainter::drawPolygon( const std::vector<QPoint>& pts,
                              bool winding, int index, int npoints )
{
  QPen oldpen = mP.pen();
  QBrush oldbrush = mP.brush();
  setBrush( QBrush( color, Dense4Pattern ) );
  setPen( Qt::NoPen );
  // i know this isn't really fast, but i blame it all on Qt with its
  // stupid container classes... what's wrong with the STL ?
  QPointArray t( pts.size() );
  int c = 0;
  for( std::vector<QPoint>::const_iterator i = pts.begin(); i != pts.end(); ++i )
  {
    t.putPoints( c++, 1, i->x(), i->y() );
  };
  mP.drawPolygon( t, winding, index, npoints );
  setPen( oldpen );
  setBrush( oldbrush );
  if( mNeedOverlay ) mOverlay.push_back( t.boundingRect() );
}

void KigPainter::drawArea( const std::vector<Coordinate>& pts, bool border )
{
  QPen oldpen = mP.pen();
  QBrush oldbrush = mP.brush();
  setBrush( QBrush( color, SolidPattern ) );
  if ( border )
    setPen( QPen( color, width == -1 ? 1 : width ) );
  else
    setPen( Qt::NoPen );
  QPointArray t( pts.size() );
  int c = 0;
  for( std::vector<Coordinate>::const_iterator i = pts.begin(); i != pts.end(); ++i )
  {
    QPoint p = toScreen( *i );
    t.putPoints( c++, 1, p.x(), p.y() );
  }
  mP.drawPolygon( t );
  setPen( oldpen );
  setBrush( oldbrush );
  if( mNeedOverlay ) mOverlay.push_back( t.boundingRect() );
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
    mOverlay.push_back( toScreenEnlarge( currentRect) );
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
}

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
    if (tR.intersects(border)) mOverlay.push_back( toScreenEnlarge( tR ) );
    if (++counter > 100)
    {
      kdDebug()<< k_funcinfo << "counter got too big :( " << endl;
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

void KigPainter::drawGrid( const CoordinateSystem& c, bool showGrid, bool showAxes )
{
  c.drawGrid( *this, showGrid, showAxes );
  setWholeWinOverlay();
}

void KigPainter::drawObject( const ObjectHolder* o, bool ss )
{
  o->draw( *this, ss );
}

void KigPainter::drawObjects( const std::vector<ObjectHolder*>& os, bool sel )
{
  drawObjects( os.begin(), os.end(), sel );
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
  if ( s.isNull() ) return;
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

QRect KigPainter::toScreenEnlarge( const Rect r ) const
{
  if ( overlayenlarge == 0 ) return msi.toScreen( r );

  QRect qr = msi.toScreen( r );
  qr.moveBy ( -overlayenlarge, -overlayenlarge );
  int w = qr.width();
  int h = qr.height();
  qr.setWidth (w + 2*overlayenlarge);
  qr.setHeight (h + 2*overlayenlarge);
  return qr;
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

typedef std::pair<double,Coordinate> coordparampair;

struct workitem
{
  workitem( coordparampair f, coordparampair s, Rect *o) :
    first(f), second(s), overlay(o) {}
  coordparampair first;
  coordparampair second;
  Rect		 *overlay;
};

void KigPainter::drawLine( const LineData& d )
{
  if ( d.a != d.b )
  {
    LineData l = calcBorderPoints( d, window() );
    drawSegment( l.a, l.b );
  }
}

void KigPainter::drawSegment( const LineData& d )
{
  drawSegment( d.a, d.b );
}

void KigPainter::drawRay( const LineData& d )
{
  drawRay( d.a, d.b );
}

void KigPainter::drawAngle( const Coordinate& cpoint, const double dstartangle,
                            const double dangle )
{
  // convert to 16th of degrees...
  const int startangle = static_cast<int>( Goniometry::convert( 16 * dstartangle, Goniometry::Rad, Goniometry::Deg ) );
  const int angle = static_cast<int>( Goniometry::convert( 16 * dangle, Goniometry::Rad, Goniometry::Deg ) );

  QPoint point = toScreen( cpoint );

//   int radius = mP.window().width() / 5;
  int radius = 50;
  QRect surroundingRect( 0, 0, radius*2, radius*2 );
  surroundingRect.moveCenter( point );

  mP.drawArc( surroundingRect, startangle, angle );

  // now for the arrow...
  QPoint end( static_cast<int>( point.x() + radius * cos( dstartangle + dangle ) ),
              static_cast<int>( point.y() - radius * sin( dstartangle + dangle ) ) );
  QPoint vect = (end - point);
  double vectlen = sqrt( float( vect.x() * vect.x() + vect.y() * vect.y() ) );
  QPoint orthvect( -vect.y(), vect.x() );
  vect = vect * 6 / vectlen;
  orthvect = orthvect * 6 / vectlen;

  QPointArray arrow( 3 );
  arrow.setPoint( 0, end );
  arrow.setPoint( 1, end + orthvect + vect );
  arrow.setPoint( 2, end + orthvect - vect );
//  std::vector<QPoint> arrow;
//  arrow.push_back( end );
//  arrow.push_back( end + orthvect + vect );
//  arrow.push_back( end + orthvect - vect );

  setBrushStyle( Qt::SolidPattern );
//  drawPolygon( arrow );
  mP.drawPolygon( arrow, false, 0, -1 );

//  if ( mNeedOverlay ) mOverlay.push_back( toScreen( r ) );
  setWholeWinOverlay();   //mp: ugly! why not compute a correct overlay?
                          //    mOverlay.push_back( arrow.boundingRect() ); 
}

void KigPainter::drawPolygon( const std::vector<Coordinate>& pts,
                              bool winding, int index, int npoints )
{
  using namespace std;
  vector<QPoint> points;
  for ( uint i = 0; i < pts.size(); ++i )
    points.push_back( toScreen( pts[i] ) );
  drawPolygon( points, winding, index, npoints );
}

void KigPainter::drawVector( const Coordinate& a, const Coordinate& b )
{
  // bugfix...
  if ( a == b ) return;
  // the segment
  drawSegment( a, b );
  // the arrows...
  Coordinate dir = b - a;
  Coordinate perp( dir.y, -dir.x );
  double length = perp.length();
  perp *= 10* pixelWidth();
  perp /= length;
  dir *= 10 * pixelWidth();
  dir /= length;
  Coordinate c = b - dir + perp;
  Coordinate d = b - dir - perp;
  // draw the arrow lines with a normal style
  mP.setPen( QPen( color, width == -1 ? 1 : width, Qt::SolidLine ) );
  drawSegment( b, c );
  drawSegment( b, d );
  // setting again the original style
  mP.setPen( QPen( color, width == -1 ? 1 : width, style ) );
}

/* *** this function is commented out ***
inline Coordinate locusGetCoord( double p, const CurveImp* curve, const ObjectHierarchy& h,
                                 bool& valid, const KigDocument& doc )
{
  Coordinate pt = curve->getPoint( p, valid, doc );
  if ( ! valid ) return Coordinate();
  PointImp pimp( pt );
  Args args;
  args.push_back( &pimp );
  std::vector<ObjectImp*> calced = h.calc( args, doc );
  assert( calced.size() == 1 );
  ObjectImp* o = calced.front();
  Coordinate ret;
  if ( o->inherits( ObjectImp::ID_PointImp ) )
  {
    valid = true;
    ret = static_cast<PointImp*>( o )->coordinate();
  }
  else
    valid = false;
  delete o;
  return ret;
};
*/

class CurveImpPointCalcer
{
  const CurveImp* curve;
public:
  CurveImpPointCalcer( const CurveImp* c )
    : curve( c )
    {
    }
  static const double endinterval;
  inline const Coordinate getPoint( double param, const KigDocument& d ) const {
    return curve->getPoint( param, d );
  }
};

const double CurveImpPointCalcer::endinterval = 1.;

void KigPainter::drawCurve( const CurveImp* curve )
{
  // we manage our own overlay
  bool tNeedOverlay = mNeedOverlay;
  mNeedOverlay = false;

  QPen pen = mP.pen();

  // this stack contains pairs of Coordinates ( parameter intervals )
  // that we still need to process:
  std::stack<workitem> workstack;
  // mp: this stack contains all the generated overlays:
  // the strategy for generating the overlay structure is the same
  // recursive-like used to draw the segments: a new rectangle is
  // generated whenever the length of a segment becomes lower than
  // overlayRectSize(), or if the segment would be drawn anyway
  // to avoid strange things from happening we impose that the distance
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
  // First push the [0,1] interval into the stack:

  Coordinate coo1 = curve->getPoint( 0., mdoc );
  Coordinate coo2 = curve->getPoint( 1., mdoc );
  workstack.push( workitem(
                    coordparampair( 0., coo1 ),
                    coordparampair( 1., coo2 ),
                    0 ) );

  // maxlength is the square of the maximum size that we allow
  // between two points..
  double maxlength = 1.5 * pixelWidth();
  maxlength *= maxlength;
  // error squared is required to be less that sigma (half pixel)
  double sigma = maxlength/4;
  // distance between two parameter values cannot be too small
  double hmin = 3e-5;
  // distance between two parameter values cannot be too large
  double hmax = 1./40;
  double hmaxoverlay = 1./8;

  int count = 1;               // the number of segments we've already
                               // visited...
  static const int maxnumberofpoints = 1000;

  const Rect& sr = window();

  // what this algorithm does is approximating the curve with a set of
  // segments.  we don't draw the individual segments, but use
  // QPainter::drawPolyline() so that the line styles work properly.
  // Possibly there are performance advantages as well ?  this array
  // is a buffer of the polyline approximation of the part of the
  // curve that we are currently processing.
  QPointArray curpolyline( 1000 );
  int curpolylinenextfree = 0;

  // we don't use recursion, but a stack based approach for efficiency
  // concerns...
  while ( ! workstack.empty() && count < maxnumberofpoints )
  {
    workitem curitem = workstack.top();
    workstack.pop();
    bool curitemok = true;
    while ( curitemok && count++ < maxnumberofpoints )
    {
      double t0 = curitem.first.first;
      double t1 = curitem.second.first;
      Coordinate p0 = curitem.first.second;
      bool valid0 = p0.valid();
      Coordinate p1 = curitem.second.second;
      bool valid1 = p1.valid();

      // we take the middle parameter of the two previous points...
      double t2 = ( t0 + t1 ) / 2;
      double h = fabs( t1 - t0 ) /2;

      // if exactly one of the two endpoints is invalid, then
      // we prefer to find an internal value of the parameter
      // separating valid points from invalid points.  We use
      // a bisection strategy (this is not implemented yet!)
//      if ( ( valid0 && ! valid1 ) || ( valid1 && ! valid0 ) )
//      {
//	while ( h >= hmin )
//	{
//	  .......................................
//	}
//      }

      Rect *overlaypt = curitem.overlay;
      Coordinate p2 = curve->getPoint( t2, mdoc );
      bool allvalid = p2.valid() && valid0 && valid1;
      bool dooverlay = ! overlaypt && h < hmaxoverlay && valid0 && valid1
                       && fabs( p0.x - p1.x ) <= overlayRectSize()
                       && fabs( p0.y - p1.y ) <= overlayRectSize();
      bool addn = sr.contains( p2 ) || h >= hmax;
      // estimated error between the curve and the segments
      double errsq = 1e21;
      if ( allvalid ) errsq = (0.5*p0 + 0.5*p1 - p2).squareLength();
      errsq /= 4;
      curitemok = false;
//      bool dodraw = allvalid && h < hmax && ( errsq < sigma || h < hmin );
      bool dodraw = allvalid && h < hmax && errsq < sigma;
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
        if ( curpolylinenextfree > 0 && curpolyline[curpolylinenextfree - 1] != tp1 )
        {
          // flush the current part of the curve
          mP.drawPolyline( curpolyline, 0, curpolylinenextfree );
          curpolylinenextfree = 0;
        }
        if ( curpolylinenextfree == 0 )
          curpolyline[curpolylinenextfree++] = tp1;
        curpolyline[curpolylinenextfree++] = tp2;
        curpolyline[curpolylinenextfree++] = tp0;
      }
      else if ( h >= hmin )   // we do not continue to subdivide indefinitely!
      {
        // push into stack in order to process both subintervals
        if ( addn || ( valid0 && sr.contains( p0 ) ) )
          workstack.push( workitem( curitem.first, coordparampair( t2, p2 ),
                                    overlaypt ) );
        if ( addn || ( valid1 && sr.contains( p1 ) ) )
        {
          curitem = workitem( coordparampair( t2, p2 ), curitem.second ,
                              overlaypt );
          curitemok = true;
        }
      }
    }
  }
  // flush the rest of the curve
  mP.drawPolyline( curpolyline, 0, curpolylinenextfree );
  curpolylinenextfree = 0;

  if ( ! workstack.empty () )
    kdDebug() << "Stack not empty in KigPainter::drawCurve!\n" << endl;
  assert ( tNeedOverlay || overlaystack.empty() );
  if ( tNeedOverlay )
  {
    Rect border = window();
    while ( ! overlaystack.empty() )
    {
      Rect overlay = overlaystack.top();
      overlaystack.pop();
      if (overlay.intersects( border ))
        mOverlay.push_back( toScreenEnlarge( overlay ) );
    }
  }
  mNeedOverlay = tNeedOverlay;
}

void KigPainter::drawTextFrame( const Rect& frame,
                                const QString& s, bool needframe )
{
  QPen oldpen = mP.pen();
  QBrush oldbrush = mP.brush();
  if ( needframe )
  {
    // inspired upon kgeo, thanks to Marc Bartsch, i've taken some of
    // his code too..
    setPen( QPen( Qt::black, 1 ) );
    setBrush( QBrush( QColor( 255, 255, 222 ) ) );
    drawRect( frame );
    setPen( QPen( QColor( 197, 194, 197 ), 1, Qt::SolidLine ) );

    QRect qr = toScreen( frame );

    mP.drawLine( qr.topLeft(), qr.topRight() );
    mP.drawLine( qr.topLeft(), qr.bottomLeft() );
  };
  setPen( oldpen );
  setBrush( oldbrush );
  drawText( frame, s, Qt::AlignVCenter | Qt::AlignLeft );
}

void KigPainter::drawArc( const Coordinate& center, const double radius,
                          const double dstartangle, const double dangle )
{
  // convert to 16th of degrees...
  const int startangle = static_cast<int>( Goniometry::convert( 16 * dstartangle, Goniometry::Rad, Goniometry::Deg ) );
  const int angle = static_cast<int>( Goniometry::convert( 16 * dangle, Goniometry::Rad, Goniometry::Deg ) );

  if ( angle <= 16 )
  {
    Coordinate a = center + radius * Coordinate( cos( dstartangle ), sin( dstartangle ));
    Coordinate b = center + radius * Coordinate( cos( dstartangle + dangle ), sin( dstartangle + dangle ));
    drawSegment ( a , b );
  } 
  else
  { 
    Rect krect( 0, 0, 2*radius, 2*radius );
    krect.setCenter( center );
    QRect rect = toScreen( krect );

    mP.drawArc( rect, startangle, angle );
    setWholeWinOverlay();
  }
}

