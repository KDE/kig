#include "kig_view.h"
#include "kig_view.moc"

#include "kig_part.h"
#include "../objects/object.h"
#include "../objects/point.h"
#include "../misc/coordinate_system.h"

#include <qdialog.h>

#include <kpopupmenu.h>
#include <kdebug.h>
#include <kcursor.h>
#include <kapplication.h>

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
  if (inIsKiosk)
    {
      kiosKontext = new KPopupMenu(this, "kiosKontext");
      kiosKontext->insertTitle("Kig Fullscreen Mode");
      kiosKontext->insertItem(i18n("Quit Fullscreen Mode"), this, SIGNAL(endKiosk()), Key_Escape);
    }
  document->addView(this);
  connect( document, SIGNAL( repaintOneObject(const Object*)), this, SLOT(updateObject(const Object*)) );
  connect( document, SIGNAL( allChanged() ), this, SLOT( updateAll() ) );

  setFocusPolicy(QWidget::ClickFocus);
  setBackgroundMode( Qt::NoBackground );
  setMouseTracking(true);

  curPix.resize(size());
  stillPix.resize( size() );

  recenterScreen();
  redrawStillPix();
  updateWidget(true);
};

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
      drawObjects(document->getMovingObjects(), curPix);
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
	  isMovingObjects = true;
	  Objects stillObjs;
	  document->startMovingSos(fromScreen(plc),stillObjs);
	  redrawStillPix();
	  drawObjects(stillObjs, stillPix);
	  updateCurPix();
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
  drawGrid();
  if (isMovingObjects) return;
  drawObjects(document->getObjects(), stillPix);
  bitBlt(&curPix, QPoint(0, 0), &stillPix, QRect(QPoint(0,0),size()));
  oldOverlay.push_back ( QRect( QPoint(0,0), size() ) );
};

void KigView::drawGrid()
{
  KigPainter p( this, &stillPix );

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

void KigView::drawObject(const Object* o, QPixmap& pix)
{
  KigPainter p( this, &pix );
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

void KigView::drawObjects(const Objects& os, QPixmap& p )
{
  KigPainter pt( this, &p );
  Objects::iterator it(os);
  Object* i;
  for (; (i = it.current()); ++it)
    i->drawWrap( pt, true );
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
