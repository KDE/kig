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

#include "kig_view.h"
#include "kig_view.moc"

#include "kig_part.h"
#include "../objects/object.h"
#include "../misc/coordinate_system.h"

#include "../modes/mode.h"

#include <qdialog.h>
#include <qwhatsthis.h>
#include <qlayout.h>
#include <qscrollbar.h>

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

KigWidget::KigWidget( KigDocument* doc,
                      KigView* view,
                      QWidget* parent,
                      const char* name,
                      bool fullscreen )
  : QWidget(parent, name, fullscreen ? WStyle_Customize | WStyle_NoBorder : 0 ),
    mdocument( doc ),
    mview( view ),
    stillPix(size()),
    curPix(size()),
    msi( Rect(), rect() ),
    misfullscreen( fullscreen )
{
  doc->addWidget(this);

  connect( mdocument, SIGNAL( recenterScreen() ), this, SLOT( recenterScreen() ) );

  setFocusPolicy(QWidget::ClickFocus);
  setBackgroundMode( Qt::NoBackground );
  setMouseTracking(true);

  curPix.resize( size() );
  stillPix.resize( size() );
};

void KigView::setupActions()
{
  KIconLoader* l = KGlobal::iconLoader();

  aZoomIn = KStdAction::zoomIn( mrealwidget, SLOT( zoomIn() ), mdoc->actionCollection() );
  aZoomIn->setToolTip( i18n( "Zoom in on the document" ) );
  aZoomIn->setWhatsThis( i18n( "Zoom in on the document" ) );

  aZoomOut = KStdAction::zoomOut( mrealwidget, SLOT( zoomOut() ),
                                  mdoc->actionCollection() );
  aZoomOut->setToolTip( i18n( "Zoom out of the document" ) );
  aZoomOut->setWhatsThis( i18n( "Zoom out of the document" ) );

  aCenterScreen = KStdAction::fitToPage( mrealwidget, SLOT( recenterScreen() ),
                                         mdoc->actionCollection() );
  aCenterScreen->setToolTip( i18n( "Recenter the screen on the document" ) );
  aCenterScreen->setWhatsThis( i18n( "Recenter the screen on the document" ) );

  QPixmap tmp = l->loadIcon( "window_fullscreen", KIcon::User );
  aFullScreen = new KAction(
    i18n( "Full Screen" ), tmp, 0, mrealwidget, SLOT( slotFullScreen() ),
    mdoc->actionCollection(), "view_fullscreen" );
  aFullScreen->setToolTip( i18n( "View this document full-screen." ) );
  aFullScreen->setWhatsThis( i18n( "View this document full-screen." ) );
}

KigWidget::~KigWidget()
{
  mdocument->delWidget( this );
};

void KigWidget::paintEvent(QPaintEvent*)
{
  updateEntireWidget();
}

void KigWidget::mousePressEvent (QMouseEvent* e)
{
  if( e->button() & Qt::LeftButton )
    return mdocument->mode()->leftClicked( e, this );
  if ( e->button() & Qt::MidButton )
    return mdocument->mode()->midClicked( e, this );
  if ( e->button() & Qt::RightButton )
    return mdocument->mode()->rightClicked( e, this );
};

void KigWidget::mouseMoveEvent (QMouseEvent* e)
{
  if( e->state() & Qt::LeftButton )
    return mdocument->mode()->leftMouseMoved( e, this );
  if ( e->state() & Qt::MidButton )
    return mdocument->mode()->midMouseMoved( e, this );
  if ( e->state() & Qt::RightButton )
    return mdocument->mode()->rightMouseMoved( e, this );
  return mdocument->mode()->mouseMoved( e, this );
};

void KigWidget::mouseReleaseEvent (QMouseEvent* e)
{
  if( e->state() & Qt::LeftButton )
    return mdocument->mode()->leftReleased( e, this );
  if ( e->state() & Qt::MidButton )
    return mdocument->mode()->midReleased( e, this );
  if ( e->state() & Qt::RightButton )
    return mdocument->mode()->rightReleased( e, this );
};

void KigWidget::updateWidget( const std::vector<QRect>& overlay )
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

void KigWidget::updateEntireWidget()
{
  std::vector<QRect> overlay;
  overlay.push_back( QRect( QPoint( 0, 0 ), size() ) );
  updateWidget( overlay );
}

void KigWidget::resizeEvent( QResizeEvent* e )
{
  QSize osize = e->oldSize();
  QSize nsize = e->size();
  Rect orect = msi.shownRect();

  curPix.resize( nsize );
  stillPix.resize( nsize );
  msi.setViewRect( rect() );

  Rect nrect( 0., 0.,
              orect.width() * nsize.width() / osize.width(),
              orect.height() * nsize.height() / osize.height() );
  nrect = matchScreenShape( nrect );
  nrect.setTopLeft( orect.topLeft() );
  msi.setShownRect( nrect );

  // horrible hack...  We need to somehow differentiate between the
  // resizeEvents we get on startup, and the ones generated by the
  // user.  The first require recentering the screen, the latter
  // don't..
  if ( nsize.width() / osize.width() > 4 ) recenterScreen();

  updateScrollBars();

  redrawScreen();
}

void KigWidget::updateCurPix( const std::vector<QRect>& ol )
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

void KigWidget::recenterScreen()
{
  msi.setShownRect( matchScreenShape( mdocument->suggestedRect() ) );
  updateScrollBars();
  redrawScreen();
}

Rect KigWidget::matchScreenShape( const Rect& r ) const
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

void KigWidget::zoomIn()
{
  Rect r = msi.shownRect();
  Coordinate c = r.center();
  r /= 2;
  r.setCenter( c );
  msi.setShownRect( r );
  redrawScreen();
  updateScrollBars();
}

void KigWidget::zoomOut()
{
  Rect r = msi.shownRect();
  Coordinate c = r.center();
  r *= 2;
  r.setCenter( c );
  msi.setShownRect( r );
  redrawScreen();
  updateScrollBars();
}

void KigWidget::clearStillPix()
{
  stillPix.fill(Qt::white);
  oldOverlay.clear();
  oldOverlay.push_back ( QRect( QPoint(0,0), size() ) );
}

void KigWidget::redrawScreen( bool dos )
{
  // update the screen...
  clearStillPix();
  KigPainter p( msi, &stillPix, *mdocument );
  p.drawGrid( mdocument->coordinateSystem() );
  p.drawObjects( mdocument->objects() );
  updateCurPix( p.overlay() );
  if ( dos ) updateEntireWidget();
}

const ScreenInfo& KigWidget::screenInfo() const
{
  return msi;
}

const Rect KigWidget::showingRect() const
{
  return msi.shownRect();
}

const Coordinate KigWidget::fromScreen( const QPoint& p )
{
  return msi.fromScreen( p );
}

double KigWidget::pixelWidth() const
{
  return msi.pixelWidth();
}

const Rect KigWidget::fromScreen( const QRect& r )
{
  return msi.fromScreen( r );
}


void KigWidget::updateScrollBars()
{
  if ( mview ) mview->updateScrollBars();
}

KigView::KigView( KigDocument* doc,
                  QWidget* parent,
                  const char* name )
  : QWidget( parent, name ),
    aZoomIn( 0 ), aZoomOut( 0 ), aCenterScreen( 0 ),
    mlayout( 0 ), mrightscroll( 0 ), mbottomscroll( 0 ),
    mupdatingscrollbars( false ),
    mrealwidget( 0 ), mdoc( doc )
{
  mlayout = new QGridLayout( this, 2, 2 );
  mrightscroll = new QScrollBar( Vertical, this, "Right Scrollbar" );
  // TODO: make this configurable...
  mrightscroll->setTracking( true );
  connect( mrightscroll, SIGNAL( valueChanged( int ) ),
           this, SLOT( slotRightScrollValueChanged( int ) ) );
  connect( mrightscroll, SIGNAL( sliderReleased() ),
           this, SLOT( updateScrollBars() ) );
  mbottomscroll = new QScrollBar( Horizontal, this, "Bottom Scrollbar" );
  connect( mbottomscroll, SIGNAL( valueChanged( int ) ),
           this, SLOT( slotBottomScrollValueChanged( int ) ) );
  connect( mbottomscroll, SIGNAL( sliderReleased() ),
           this, SLOT( updateScrollBars() ) );
  mrealwidget = new KigWidget( doc, this, this, "Kig Widget" );
  mlayout->addWidget( mbottomscroll, 1, 0 );
  mlayout->addWidget( mrealwidget, 0, 0 );
  mlayout->addWidget( mrightscroll, 0, 1 );

  setupActions();

  resize( sizeHint() );
  mrealwidget->recenterScreen();
}

void KigView::updateScrollBars()
{
  // we update the scrollbars to reflect the new "total size" of the
  // document...  The total size is calced in entireDocumentRect().
  // ( it is calced as a rect that contains all the points in the
  // document, and then enlarged a bit, and scaled to match the screen
  // width/height ratio...
  // What we do here is tell the scroll bars what they should show as
  // their total size..

  // see the doc of this variable in the header for this...
  mupdatingscrollbars = true;

  Rect er = mrealwidget->entireDocumentRect();
  Rect sr = mrealwidget->screenInfo().shownRect();

  // we define the total rect to be the smallest rect that contains
  // both er and sr...
  er |= sr;

  // we need ints, not doubles, so since "pixelwidth == widgetcoord /
  // internalcoord", we use "widgetcoord/pixelwidth", which would then
  // equal "internalcoord", which has to be an int ( by definition.. )
  // i know, i'm a freak to think about these sorts of things... ;)
  double pw = mrealwidget->screenInfo().pixelWidth();

  // what the scrollbars reflect is the bottom resp. the left side of
  // the shown rect.  This is why the maximum value is not er.top()
  // (which would be the maximum value of the top of the shownRect),
  // but er.top() - sr.height(), which is the maximum value the bottom of
  // the shownRect can reach...

  int rightmin = static_cast<int>( er.bottom() / pw );
  int rightmax = static_cast<int>( ( er.top() - sr.height() ) / pw );

  mrightscroll->setMinValue( rightmin );
  mrightscroll->setMaxValue( rightmax );
  mrightscroll->setLineStep( (int)( sr.height() / pw / 10 ) );
  mrightscroll->setPageStep( (int)( sr.height() / pw / 1.2 ) );

  // note that since Qt has a coordinate system with the lowest y
  // values at the top, and we have it the other way around ( i know i
  // shouldn't have done this.. :( ), we invert the value that the
  // scrollbar shows.  This is inverted again in
  // slotRightScrollValueChanged()...
  mrightscroll->setValue( (int) ( rightmin + ( rightmax - ( sr.bottom() / pw ) ) ) );

  mbottomscroll->setMinValue( (int)( er.left() / pw ) );
  mbottomscroll->setMaxValue( (int)( ( er.right() - sr.width() ) / pw ) );
  mbottomscroll->setLineStep( (int)( sr.width() / pw / 10 ) );
  mbottomscroll->setPageStep( (int)( sr.width() / pw / 1.2 ) );
  mbottomscroll->setValue( (int)( sr.left() / pw ) );

  mupdatingscrollbars = false;
}

Rect KigWidget::entireDocumentRect() const
{
  return matchScreenShape( mdocument->suggestedRect() );
}

void KigView::slotRightScrollValueChanged( int v )
{
  if ( ! mupdatingscrollbars )
  {
    // we invert the inversion that was done in updateScrollBars() (
    // check the documentation there..; )
    v = mrightscroll->minValue() + ( mrightscroll->maxValue() - v );
    double pw = mrealwidget->screenInfo().pixelWidth();
    double nb = double( v ) * pw;
    mrealwidget->scrollSetBottom( nb );
  };
}

void KigView::slotBottomScrollValueChanged( int v )
{
  if ( ! mupdatingscrollbars )
  {
    double pw = mrealwidget->screenInfo().pixelWidth();
    double nl = double( v ) * pw;
    mrealwidget->scrollSetLeft( nl );
  };
}

void KigWidget::scrollSetBottom( double rhs )
{
  Rect sr = msi.shownRect();
  Coordinate bl = sr.bottomLeft();
  bl.y = rhs;
  sr.setBottomLeft( bl );
  msi.setShownRect( sr );
  redrawScreen();
}

void KigWidget::scrollSetLeft( double rhs )
{
  Rect sr = msi.shownRect();
  Coordinate bl = sr.bottomLeft();
  bl.x = rhs;
  sr.setBottomLeft( bl );
  msi.setShownRect( sr );
  redrawScreen();
}

const ScreenInfo& KigView::screenInfo() const
{
  return mrealwidget->screenInfo();
}

KigView::~KigView()
{
}

KigWidget* KigView::realWidget()
{
  return mrealwidget;
}

const KigDocument& KigWidget::document() const
{
  return *mdocument;
}

QSize KigWidget::sizeHint() const
{
  return QSize( 600, 400 );
}

void KigWidget::wheelEvent( QWheelEvent* e )
{
  int delta = e->delta();
  if ( mview ) mview->scrollVertical( delta );
}

void KigView::scrollVertical( int delta )
{
  if ( delta >= 0 )
    for ( int i = 0; i < delta; i += 120 )
      mrightscroll->subtractLine();
  else
    for ( int i = 0; i >= delta; i -= 120 )
      mrightscroll->addLine();
}

void KigWidget::slotFullScreen()
{
  QDesktopWidget* desktop = kapp->desktop();
  int screen = desktop->screenNumber( this );
  QRect r = desktop->screenGeometry( screen );

  QDialog* kiosk = new QDialog( 0, 0, false, WStyle_Customize | WStyle_NoBorder );
  kiosk->setGeometry( r );
  KigWidget* w = new KigWidget( mdocument, 0, kiosk, 0, true );
  w->setGeometry( QRect( QPoint( 0, 0 ), r.size() ) );
  w->redrawScreen();
  kiosk->raise();
  kiosk->show();
}

bool KigWidget::isFullScreen() const
{
  return misfullscreen;
}
