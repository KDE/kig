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

#include "kig_view.h"
#include "kig_view.moc"

#include "kig_part.h"
#include "../objects/object.h"
#include "../objects/point.h"
#include "../misc/coordinate_system.h"

#include "../modes/mode.h"

#include <qdialog.h>
#include <qwhatsthis.h>

#include <kdebug.h>
#include <kcursor.h>
#include <klocale.h>
#include <kapplication.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kiconloader.h>

#include <cmath>

kdbgstream& operator<< ( kdbgstream& s, const QPoint& t )
{
  s << "x: " << t.x() << " y: " << t.y();
  return s;
};

KigView::KigView( KigDocument* inDoc, QWidget* parent, const char* name )
  : QWidget(parent, name),
    document(inDoc),
    stillPix(size()),
    curPix(size()),
    msi( Rect(), rect() )
{
  document->addView(this);
  connect( document, SIGNAL( recenterScreen() ), this, SLOT( recenterScreen() ) );

  setFocusPolicy(QWidget::ClickFocus);
  setBackgroundMode( Qt::NoBackground );
  setMouseTracking(true);

  curPix.resize(size());
  stillPix.resize( size() );

  recenterScreen();

  clearStillPix();
  KigPainter p( msi, &stillPix );
  p.drawGrid( document->coordinateSystem() );
  p.drawObjects( document->objects() );
  updateCurPix( p.overlay() );

  setupActions();
};

void KigView::setupActions()
{
  KIconLoader l;

  aZoomIn = KStdAction::zoomIn( this, SLOT( zoomIn() ), document->actionCollection() );
  aZoomIn->setWhatsThis( i18n( "Zoom in on the document" ) );

  aZoomOut = KStdAction::zoomOut( this, SLOT( zoomOut() ),
                                  document->actionCollection() );
  aZoomOut->setWhatsThis( i18n( "Zoom out of the document" ) );

  aCenterScreen = KStdAction::fitToPage( this, SLOT( recenterScreen() ),
                                         document->actionCollection() );
  aCenterScreen->setWhatsThis( i18n( "Recenter the screen on the document" ) );
}

KigView::~KigView()
{
  document->delView(this);
};

void KigView::paintEvent(QPaintEvent*)
{
  updateEntireWidget();
}

void KigView::mousePressEvent (QMouseEvent* e)
{
  if( e->button() & Qt::LeftButton )
    return document->mode()->leftClicked( e, this );
  if ( e->button() & Qt::MidButton )
    return document->mode()->midClicked( e, this );
  if ( e->button() & Qt::RightButton )
    return document->mode()->rightClicked( e, this );
};

void KigView::mouseMoveEvent (QMouseEvent* e)
{
  if( e->state() & Qt::LeftButton )
    return document->mode()->leftMouseMoved( e, this );
  if ( e->state() & Qt::MidButton )
    return document->mode()->midMouseMoved( e, this );
  if ( e->state() & Qt::RightButton )
    return document->mode()->rightMouseMoved( e, this );
  return document->mode()->mouseMoved( e, this );
};

void KigView::mouseReleaseEvent (QMouseEvent* e)
{
  if( e->state() & Qt::LeftButton )
    return document->mode()->leftReleased( e, this );
  if ( e->state() & Qt::MidButton )
    return document->mode()->midReleased( e, this );
  if ( e->state() & Qt::RightButton )
    return document->mode()->rightReleased( e, this );
};

void KigView::updateWidget( const std::vector<QRect>& overlay )
{
#undef SHOW_OVERLAY_RECTS
#ifdef SHOW_OVERLAY_RECTS
  QPainter debug (this, this);
  debug.setPen(Qt::yellow);
#endif // SHOW_OVERLAY_RECTS
  // we undo our old changes...
  for ( std::vector<QRect>::const_iterator i = oldOverlay.begin(); i != oldOverlay.end(); ++i )
    bitBlt( this, i->topLeft(), &curPix, *i );
  // we add our new changes...
  for ( std::vector<QRect>::const_iterator i = overlay.begin(); i != overlay.end(); ++i )
  {
    bitBlt( this, i->topLeft(), &curPix, *i );
#ifdef SHOW_OVERLAY_RECTS
    debug.drawRect(*i);
#endif
  };
  oldOverlay = overlay;
};

void KigView::updateEntireWidget()
{
  std::vector<QRect> overlay;
  overlay.push_back( QRect( QPoint( 0, 0 ), size() ) );
  updateWidget( overlay );
}

void KigView::resizeEvent(QResizeEvent*)
{
  recenterScreen();
  curPix.resize(size());
  stillPix.resize( size() );
  msi.setViewRect( rect() );
  redrawScreen();
}

void KigView::updateCurPix( const std::vector<QRect>& ol )
{
  // we make curPix look like stillPix again...
  for ( std::vector<QRect>::const_iterator i = oldOverlay.begin(); i != oldOverlay.end(); ++i )
    bitBlt( &curPix, i->topLeft(), &stillPix, *i );
  for ( std::vector<QRect>::const_iterator i = ol.begin(); i != ol.end(); ++i )
    bitBlt( &curPix, i->topLeft(), &stillPix, *i );

  // we add ol to oldOverlay, so that part of the widget will be
  // updated too in updateWidget...
  std::copy( ol.begin(), ol.end(), std::back_inserter( oldOverlay ) );
}

void KigView::recenterScreen()
{
  msi.setShownRect( matchScreenShape(document->suggestedRect()) );
//   kdDebug() << k_funcinfo << endl
//  // 	    << "(0,0): " << toScreen(Coordinate(0,0)) << endl
// // 	    << "(-3,-2): " << toScreen( Coordinate(-3,-2)) << endl
// // 	    << "(3,2): " << toScreen( Coordinate(3,2)) << endl;
//  	    << "showingRect:" << showingRect() << endl
//  	    << "fromScreen(...): " << fromScreen(QRect(QPoint(0,0), size()))
//  	    << endl;
  redrawScreen();
}

Rect KigView::matchScreenShape( const Rect& r )
{
//   kdDebug() << k_funcinfo << "input: " << r << endl;
  Rect s = r;
  Coordinate c = r.center();
  double v = double(r.width())/r.height(); // document dimensions
  double w = double(size().width())/size().height(); // widget
						     // dimensions
  // we don't show less than r, if the dimensions don't match, we
  // extend r into some dimension...
  if( v > w )
  {
    // not high enough...
    s.setHeight( s.width() / w );
  }
  else
  {
    // not wide enough...
    s.setWidth( s.height() * w );
  };
  s.setCenter(c);
//   kdDebug() << k_funcinfo << "output: " << s << endl;
  return s.normalized();
}

void KigView::zoomIn()
{
  Rect r = msi.shownRect();
  Coordinate c = r.center();
  r /= 2;
  r.setCenter( c );
  msi.setShownRect( r );
  redrawScreen();
}

void KigView::zoomOut()
{
  Rect r = msi.shownRect();
  Coordinate c = r.center();
  r *= 2;
  r.setCenter( c );
  msi.setShownRect( r );
  redrawScreen();
}

void KigView::clearStillPix()
{
  stillPix.fill(Qt::white);
  oldOverlay.clear();
  oldOverlay.push_back ( QRect( QPoint(0,0), size() ) );
}

void KigView::redrawScreen()
{
  // update the screen...
  clearStillPix();
  KigPainter p( msi, &stillPix );
  p.drawGrid( document->coordinateSystem() );
  p.drawObjects( document->objects() );
  updateCurPix( p.overlay() );
  updateEntireWidget();
}

const ScreenInfo& KigView::screenInfo() const
{
  return msi;
}

const Rect KigView::showingRect()
{
  return msi.shownRect();
}

const Coordinate KigView::fromScreen( const QPoint& p )
{
  return msi.fromScreen( p );
}

double KigView::pixelWidth()
{
  return msi.pixelWidth();
}

const Rect KigView::fromScreen( const QRect& r )
{
  return msi.fromScreen( r );
}
