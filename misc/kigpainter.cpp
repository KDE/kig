#include "kigpainter.h"

#include "../kig/kig_view.h"
#include "common.h"

#include <qpen.h>

inline double max( double a, double b )
{
  return ( a > b ? a : b );
};

KigPainter::KigPainter( KigView* view, QPaintDevice* device )
  : mP ( device ),
    mView( view )
{
}

KigPainter::~KigPainter()
{
}

void KigPainter::drawRect( const Rect& r )
{
  Rect rt = r.normalized();
  QRect qr = mView->toScreen(rt);
  qr.normalize();
  mP.drawRect(qr);
  mView->appendOverlay( qr );
}

void KigPainter::drawRect( const QRect& r )
{
  mP.drawRect(r);
  mView->appendOverlay( r );
}

void KigPainter::drawCircle( const Coordinate& center, const double radius )
{
  Coordinate bottomLeft = center - Coordinate(radius, radius);
  Coordinate topRight = center + Coordinate(radius, radius);
  Rect r( bottomLeft, topRight );
  QRect qr = mView->toScreen( r );
  mP.drawEllipse ( qr );
  circleOverlay( center, radius );
}

void KigPainter::drawSegment( const Coordinate& from, const Coordinate& to )
{
  QPoint tF = mView->toScreen(from), tT = mView->toScreen(to);
  mP.drawLine( tF, tT );
  segmentOverlay( from, to );
}

void KigPainter::drawPoint( const Coordinate& p, bool s )
{
  if( s )
    {
      mP.drawPoint( mView->toScreen(p) );
      pointOverlay( p );
    }
  else 
    drawCircle( p, 2*mView->pixelWidth() );
}

void KigPainter::drawLine( const Coordinate& p1, const Coordinate& p2 )
{
  Coordinate ba = p1, bb = p2;
  calcBorderPoints( ba, bb, window() );
  drawSegment( ba, bb );
}

void KigPainter::drawText( const Rect p, const QString s, int textFlags, int len )
{
  QRect t = mView->toScreen(p);
  int tf = textFlags;
  mP.drawText( t, tf, s, len );
  textOverlay( t, s, tf, len );
}

void KigPainter::textOverlay( const QRect& r, const QString s, int textFlags, int len )
{
  //  kdDebug() << Rect::fromQRect( mP.boundingRect( r, textFlags, s, len ) ) << endl;
  mView->appendOverlay( mP.boundingRect( r, textFlags, s, len ) );
}

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
  // i know this isn't really fast, but i blame it all on Qt with its
  // stupid container classes... what's wrong with the STL ?
  QPointArray t( pts.size() );
  int c = 0;
  for( std::vector<Coordinate>::const_iterator i = pts.begin(); i != pts.end(); ++i )
    {
      QPoint tt = mView->toScreen (*i);
      t.putPoints( c++, 1, tt.x(), tt.y() );
    };
  mP.drawPolygon( t, winding, index, npoints );
  // just repaint the entire window...
  mView->appendOverlay( QRect( QPoint( 0, 0 ), mView->size() ) );
}

Rect KigPainter::window()
{
  return mView->showingRect().normalized();
}

void KigPainter::circleOverlayRecurse( const Coordinate& centre, double radius,
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

  // 1.415 should actually be 1.414...
  double fault = currentRect.width()*1.415 + mView->pixelWidth();
  double radiusBig = radius + fault ;
  double radiusSmall = radius - fault ;

  // if the circle doesn't touch this rect, we return
  // too far from the centre
  if (((tl - centre).length() > radiusBig) &&
      ((tr - centre).length() > radiusBig) &&
      ((bl - centre).length() > radiusBig) &&
      ((br - centre).length() > radiusBig))
    return;

  // too near to the centre
  if (((tl - centre).length() < radiusSmall) &&
      ((tr - centre).length() < radiusSmall) &&
      ((bl - centre).length() < radiusSmall) &&
      ((br - centre).length() < radiusSmall))
    return;

  // the rect contains some of the circle
  // -> if it's small enough, we keep it
  if( currentRect.width() < overlayRectSize() ) {
    mView->appendOverlay( mView->toScreen( currentRect) );
  } else {
    // this func works recursive: we subdivide the current rect, and if
    // it is of a good size, we keep it, otherwise we handle it again
    double width = currentRect.width() / 2;
    double height = currentRect.height() / 2;
    Rect r1 ( c, -width, -height);
    r1.normalize();
    circleOverlayRecurse(centre, radius, r1);
    Rect r2 ( c, width, -height);
    r2.normalize();
    circleOverlayRecurse(centre, radius, r2);
    Rect r3 ( c, -width, height);
    r3.normalize();
    circleOverlayRecurse(centre, radius, r3);
    Rect r4 ( c, width, height);
    r4.normalize();
    circleOverlayRecurse(centre, radius, r4);
  };
};

void KigPainter::circleOverlay( const Coordinate& centre, double radius )
{
  double t = radius + mView->pixelWidth();
  Coordinate r( t, t );
  Coordinate bottomLeft = centre - r;
  Coordinate topRight = centre + r;
  Rect rect( bottomLeft, topRight );
  circleOverlayRecurse ( centre , radius, rect );
}

void KigPainter::segmentOverlay( const Coordinate& p1, const Coordinate& p2 )
{
  // this code is based upon what Marc Bartsch wrote for KGeo

  // some stuff we may need:
  Coordinate p3 = p2 - p1;
  double length = p3.length();
  Rect border = window();
  // if length is smaller than one, we risk getting a divide by zero
  length = max(length, 1);
  p3 *= overlayRectSize();
  p3 /= length;

  int counter = 0;

  Rect r(p1, p2);
  r.normalize();

  for (;;) {
    // sanity check...
    Rect tR( Coordinate( 0, 0 ), overlayRectSize(), overlayRectSize() );
    Coordinate tP = p1+p3*counter;
    tR.setCenter(tP);
    if (!tR.intersects(r))
      {
	//kdDebug()<< "stopped after "<< counter << " passes." << endl;
	break;
      }
    if (tR.intersects(border)) mView->appendOverlay( mView->toScreen( tR ) );
    if (++counter > 100) 
      {
	kdError()<< k_funcinfo << "counter got too big :( " << endl;
	break;
      }
    }
}

double KigPainter::overlayRectSize()
{
  return 20 * mView->pixelWidth();
}

void KigPainter::pointOverlay( const Coordinate& p1 )
{
  Rect r( p1, 3*mView->pixelWidth(), 3*mView->pixelWidth());
  r.setCenter( p1 );
  mView->appendOverlay( mView->toScreen( r) );
}
