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

#include "kig_view.h"
#include "kig_view.moc"

#include "kig_part.h"
#include "kig_popup.h"
#include "../objects/object.h"
#include "../objects/point.h"
#include "../misc/coordinate_system.h"

#include <qdialog.h>
#include <qwhatsthis.h>

#include <kpopupmenu.h>
#include <kdebug.h>
#include <kcursor.h>
#include <kapplication.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kiconloader.h>

kdbgstream& operator<< ( kdbgstream& s, const QPoint& t )
{
  s << "x: " << t.x() << " y: " << t.y();
  return s;
};

KigView::KigView( KigDocument* inDoc, QWidget* parent, const char* name, bool inIsKiosk )
  : QWidget(parent, name),
    document(inDoc),
    plc(0,0),
    pmt(0,0),
    isMovingObjects(false),
    isDraggingRect(false),
    stillPix(size()),
    curPix(size()),
    kiosk(0),
    kiosKontext(0),
    isKiosk(inIsKiosk),
    mViewRect( )
{
  setInstance( document->instance() );
  setXMLFile("kigpartui.rc");

  if (inIsKiosk)
    {
      kiosKontext = new KPopupMenu(this, "kiosKontext");
      kiosKontext->insertTitle("Kig Fullscreen Mode");
      kiosKontext->insertItem(i18n("Quit Fullscreen Mode"), this, SIGNAL(endKiosk()), Key_Escape);
    }
  document->addView(this);
  connect( document, SIGNAL( repaintOneObject(const Object*)), this, SLOT(updateObject(const Object*)) );
  connect( document, SIGNAL( allChanged() ), this, SLOT( updateAll() ) );
  connect( document, SIGNAL( recenterScreen() ), this, SLOT( recenterScreen() ) );
  connect( document, SIGNAL( repaintObjects( const Objects& ) ), this, SLOT( updateObjects( const Objects& ) ) );

  setFocusPolicy(QWidget::ClickFocus);
  setBackgroundMode( Qt::NoBackground );
  setMouseTracking(true);

  curPix.resize(size());
  stillPix.resize( size() );

  recenterScreen();
  redrawStillPix();
  updateWidget(true);

  setupActions();
};

void KigView::setupActions()
{
  KAction* a;
  KIconLoader l;

  a = KStdAction::zoomIn( this, SLOT( zoomIn() ), actionCollection() );
  a->setWhatsThis( i18n( "Zoom in on the document" ) );

  a = KStdAction::zoomOut( this, SLOT( zoomOut() ), actionCollection() );
  a->setWhatsThis( i18n( "Zoom out of the document" ) );

  a = new KAction( i18n( "Center screen" ), 0, this, SLOT( recenterScreen() ),
		   actionCollection(), "view_recenter" );
  a->setWhatsThis( i18n( "Recenter the screen on the document" ) );

  QPixmap tmp = l.loadIcon( "window_fullscreen", KIcon::User );
  a = new KAction( i18n( "Full screen" ), tmp, 0, this,
		   SLOT( startKioskMode() ), actionCollection(),
		   "view_fullscreen" );
  a->setWhatsThis( i18n( "View this document full-screen." ) );
}

KigView::~KigView()
{
  document->delView(this);
  delete kiosKontext;
};

void KigView::paintEvent(QPaintEvent*)
{
  updateWidget(true);
}

void KigView::mousePressEvent (QMouseEvent* e)
{
  if( isMovingObjects ) { return; };
  pmt = plc = e->pos();
  oco = document->whatAmIOn(fromScreen(plc), 2*pixelWidth());
  if (oco.isEmpty())
    {
      // clicked on an empty spot...
      if ((e->button() & RightButton) && isKiosk) kiosKontext->popup(pmt);
      if ((e->button() & LeftButton) && document->canSelectRect()) isDraggingRect = true;
      updateCurPix();
      displayText(0);
      updateWidget();
    }
  else
    {
      updateCurPix();
      // clicked on one or more objects...
      if (e->button() & LeftButton) displayText(oco.first()->vTBaseTypeName());
      else displayText(0);
      updateWidget();
    };
};

void KigView::mouseMoveEvent (QMouseEvent* e)
{
  pmt = e->pos();

  // moving the sos if that's necessary...
  if ( isMovingObjects )
    {
      updateCurPix();
      document->moveSosTo(fromScreen(pmt));
      KigPainter p( this, &curPix );
      drawObjects(document->getMovingObjects(), p);
      updateWidget(false);
      return;
    };

  // the selection rect for selecting all objects inside it...
  if (isDraggingRect)
    {
      updateCurPix();
      drawRect();
      updateWidget();
      return;
    };

  if (document->getObc())
    {
      updateCurPix();
      // draw a preliminary image of what the object will look like...
      drawPrelim();
      // -> if he's over a selectable object, we tell him what it
      // would be used for, or else, we just tell him what he's
      // constructing...
      Objects where = document->whatAmIOn(fromScreen(pmt), 2*pixelWidth());
      if (where.isEmpty())
	{
	  displayText(i18n("Constructing a %1").arg(document->getObc()->vTBaseTypeName()));
	  setCursor(KCursor::arrowCursor());
	}
      else
	{
	  displayText(document->getObc()->wantArg(where.first()));
      	  setCursor (KCursor::handCursor());
	};
      updateWidget();
      return;
    };

  if (!oco.isEmpty())
    {
      // the user clicked on something...
      if ((plc-pmt).manhattanLength() > 3 && document->canMoveObjects())
	{
	  // the user wants to move these objects...
	  if ( (oco & document->getSos()).isEmpty())
	    {
	      // he clicked on something that isn't currently
	      // selected...
	      // we first select it (taking the Ctrl- and Shift-
	      // buttons into account)
	      if (!(e->state() & (ControlButton | ShiftButton)))
		document->clearSelection();
	      document->selectObject(oco.first());
	      // we don't call drawObject, since this is done by
	      // document->selectObject... 
	    };
	  // we start moving the sos...
	  startMovingSos( plc );
	  // we immediately show our changes by calling ourselves
	  // again... 
	  this->mouseMoveEvent(e);
	  return;
	};
      updateCurPix();
      displayText(oco.first()->vTBaseTypeName());
      updateWidget(false);
    };

  if (!(e->state() & (LeftButton | MidButton | RightButton)))
    {
      // the user didn't click, and is just moving around his
      // cursor...
      Objects where = document->whatAmIOn(fromScreen(pmt), 2*pixelWidth());
      updateCurPix();
      // no obc
      // --> we just tell the user what he's moving over...
      if (where.isEmpty())
	{
	  setCursor(KCursor::arrowCursor());
	  displayText(0);
	}
      else
	{
	  setCursor (KCursor::handCursor());
	  displayText(where.getFirst()->vTBaseTypeName());
	};
      updateWidget(false);
      return;
    };
};

void KigView::mouseReleaseEvent (QMouseEvent* e)
{
  Coordinate __d = fromScreen(e->pos());
  kdDebug() << k_funcinfo << __d << endl;
  if ( isMovingObjects )
    {
      document->stopMovingSos();
      isMovingObjects = false;
      redrawStillPix();
      updateWidget();
    }
  else if ( isDraggingRect )
    {
      if (! (e->state() & (ControlButton | ShiftButton)))
	{
	  document->clearSelection();
	};
      document->selectObjects(Rect(fromScreen(pmt), fromScreen(plc)).normalized());
      // we don't call drawObjects, this is done for us by
      // document->selectObjects()...
      isDraggingRect = false;
      updateWidget();
    }
  else if (!oco.isEmpty() && (e->button() & LeftButton) && (pmt-plc).manhattanLength() < 4 && document->canSelectObject(oco.first()))
    {
      if (oco.getFirst()->getSelected())
	{
	  if (e->state() & (ShiftButton | ControlButton))
	    {
	      if (document->canUnselect()) document->unselect(oco.getFirst());
	      // we don't call drawObject, since document->unselect
	      // does this for us...
	      updateWidget();
	    }
	  else
	    {
	      if (document->canUnselect())
		{
		  document->clearSelection();
		  document->selectObject(oco.getFirst());
		};
	      // we don't call drawObject, since the document
	      // does this for us...
	      updateWidget();
	    };
	}
      else
	{
	  if (!(e->state() & (ShiftButton | ControlButton)) && document->canUnselect())
	    document->clearSelection();
	  document->selectObject(oco.first());
	  // we don't call drawObject, since the document
	  // does this for us...
	  updateWidget();
	};
    }
  else if( !oco.isEmpty() && ( e->button() & RightButton ) && (pmt-plc).manhattanLength() < 4 )
    {
      displayText(0);
      updateWidget();
      Objects o = oco & document->getSos();
      KigObjectsPopup* m;
      if( o.isEmpty() )
	{
	  Objects a;
	  a.append( oco.first() );
	  m = document->getPopup( a, this );
	}
      else
	{
	  m = document->getPopup( document->getSos(), this );
	};
      if( m ) m->exec( mapToGlobal( plc ) );
      delete m;
    }
  else if (document->canAddObjects() && (e->button() == MidButton) && (pmt-plc).manhattanLength() < 4)
    {
      Point* p;
      if (!oco.isEmpty() && Object::toCurve(oco.first()))
	{
	  // add a ConstrainedPoint...
	  p = new ConstrainedPoint(Object::toCurve(oco.getFirst()),fromScreen(plc));
	}
      else
	{
	  // add a normal Point...
	  p = new Point(fromScreen(plc));
	};
      document->addObject(p);
      // we don't call drawObject, since the document
      // does this for us...
      if (document->getObc())
	{
	  if (document->canSelectObject(p))
	    document->selectObject(p);
	};
      updateWidget();
    };
  oco.clear();
};

void KigView::startKioskMode()
{
  assert (!kiosk);
  kiosk = new QDialog (0,0, false, WStyle_Customize | WStyle_NoBorder);
  kiosk->setGeometry( 0, 0, KApplication::desktop()->width(), KApplication::desktop()->height());
  kioskView = new KigView(document, kiosk, "kioskView", true);
  kioskView->setFixedSize( QSize(kapp->desktop()->width(), kapp->desktop()->height()));
  connect (kioskView, SIGNAL(endKiosk()), this, SLOT(endKioskMode()));
  kiosk->raise();
  kiosk->show();
}

void KigView::endKioskMode()
{
  kiosk->hide();
  delete kioskView;
  kioskView = 0;
  delete kiosk;
  kiosk = 0;
}

void KigView::redrawStillPix()
{
  stillPix.fill(Qt::white);
  KigPainter p( this, &stillPix );
  drawGrid( p );
  if (isMovingObjects) return;
  drawObjects( document->getObjects(), p );
  bitBlt(&curPix, QPoint(0, 0), &stillPix, QRect(QPoint(0,0),size()));
  oldOverlay.push_back ( QRect( QPoint(0,0), size() ) );
};

void KigView::drawGrid( KigPainter& p )
{
  document->getCoordinateSystem()->drawGrid( p );
};

void KigView::updateWidget( bool needRedraw )
{
#undef SHOW_OVERLAY_RECTS
#ifdef SHOW_OVERLAY_RECTS
  QPainter debug (this, this);
  debug.setPen(Qt::yellow);
#endif // SHOW_OVERLAY_RECTS
  if (needRedraw)
    {
      // if necessary, we just redraw everything...
      bitBlt( this, QPoint( 0, 0 ), &curPix, QRect( QPoint( 0, 0 ), size() ) );
    }
  else
    {
      // we undo our old changes...
      for ( std::vector<QRect>::iterator i = oldOverlay.begin(); i != oldOverlay.end(); ++i )
	bitBlt( this, i->topLeft(), &curPix, *i );
      // we add our new changes...
      for ( std::vector<QRect>::iterator i = overlay.begin(); i != overlay.end(); ++i )
	{
	  bitBlt( this, i->topLeft(), &curPix, *i );
#ifdef SHOW_OVERLAY_RECTS
	  debug.drawRect(*i);
#endif
	};
    };
  oldOverlay.clear();
  if (needRedraw)
    {
      oldOverlay.push_back ( QRect( QPoint(0,0), size() ) );
    }
  else
    {
      oldOverlay = overlay;
    };
  overlay.clear();
  return;
};

void KigView::drawObject(const Object* o, KigPainter& p)
{
  o->drawWrap( p, true );
};

void KigView::drawRect()
{
  KigPainter p( this, &curPix );
  QPen pen(Qt::black, 1, Qt::DotLine);
  p.setPen(pen);
  p.setBrush(QBrush(Qt::cyan,Dense6Pattern));
  QRect r( plc, pmt );
  p.drawRect(r.normalize());
};

void KigView::drawPrelim()
{
  KigPainter p( this, &curPix );
  document->getObc()->drawPrelim
    ( p,
      fromScreen( mapFromGlobal( QCursor::pos() ) )
      );
};

void KigView::drawTbd()
{
  if (tbd.isNull()) return;
  KigPainter p(this,&curPix);
  // tf = text formatting flags
  int tf = AlignLeft | AlignTop | DontClip | WordBreak;
  // we need the rect where we're going to paint text
  p.setPen(QPen(Qt::blue, 1, SolidLine));
  p.setBrush(Qt::NoBrush);
  p.drawText(Rect( fromScreen(pmt), showingRect().bottomRight() ).normalized(), tbd, tf);
};

void KigView::resizeEvent(QResizeEvent*)
{
  recenterScreen();
  curPix.resize(size());
  stillPix.resize( size() );
  redrawStillPix();
  updateWidget(true);
}

void KigView::drawObjects(const Objects& os, KigPainter& p )
{
  Objects::iterator it(os);
  Object* i;
  for (; (i = it.current()); ++it)
    i->drawWrap( p, true );
}

void KigView::updateCurPix()
{
  // we make curPix look like stillPix again...
  for ( std::vector<QRect>::iterator i = oldOverlay.begin(); i != oldOverlay.end(); ++i )
    bitBlt( &curPix, i->topLeft(), &stillPix, *i );
}

Rect KigView::showingRect()
{
  return mViewRect;
}


QPoint KigView::toScreen( const Coordinate p )
{
  Coordinate t = p - showingRect().bottomLeft();
  t *= size().width();
  t /= showingRect().width();
  return QPoint( t.x, height() - t.y );
}

Coordinate KigView::fromScreen( const QPoint& p )
{
  Coordinate t( p.x(), height() - p.y() );
  t *= showingRect().width();
  t /= size().width();
  return t + showingRect().bottomLeft();
}

void KigView::recenterScreen()
{
  mViewRect = matchScreenShape(document->suggestedRect());
  kdDebug() << k_funcinfo << endl
// 	    << "(0,0): " << toScreen(Coordinate(0,0)) << endl
// 	    << "(-3,-2): " << toScreen( Coordinate(-3,-2)) << endl
// 	    << "(3,2): " << toScreen( Coordinate(3,2)) << endl;
	    << "showingRect:" << showingRect() << endl
	    << "fromScreen(...): " << fromScreen(QRect(QPoint(0,0), size()))
	    << endl;
  updateAll();
}

double KigView::pixelWidth()
{
  Coordinate a = fromScreen( QPoint( 0, 0 ) );
  Coordinate b = fromScreen( QPoint( 0, 1000 ) );
  return fabs( b.y - a.y ) / 1000;
}

Rect KigView::matchScreenShape( const Rect& r )
{
  kdDebug() << k_funcinfo << "input: " << r << endl;
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
  kdDebug() << k_funcinfo << "output: " << s << endl;
  return s.normalized();
}

void KigView::drawScreen( QPaintDevice* d )
{
  KigPainter p( this, d );
  drawGrid( p );
  drawObjects( document->getObjects(), p );
}
void KigView::startMovingSos( const QPoint& p )
{
  isMovingObjects = true;
  Objects stillObjs;
  document->startMovingSos(fromScreen(p),stillObjs);
  redrawStillPix();
  KigPainter pt( this, &stillPix );
  drawObjects(stillObjs, pt);
  updateCurPix();
}

void KigView::zoomIn()
{
  Coordinate c = mViewRect.center();
  mViewRect /= 2;
  mViewRect.setCenter( c );
  updateAll();
}

void KigView::zoomOut()
{
  Coordinate c = mViewRect.center();
  mViewRect *= 2;
  mViewRect.setCenter( c );
  updateAll();
}
