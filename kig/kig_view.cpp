#include "kig_view.h"
#include "kig_view.moc"

#include "kig_part.h"
#include "../objects/object.h"
#include "../objects/point.h"

#include <qpainter.h>
#include <qdialog.h>

#include <kpopupmenu.h>
#include <kdebug.h>
#include <kcursor.h>
#include <kapplication.h>

#include <iostream>

KigView::KigView (KigDocument* inDoc, QWidget* parent, const char* name, bool inIsKiosk)
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
    isKiosk(inIsKiosk)
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
  setMouseTracking(true);

  curPix.resize(size());
  stillPix.resize( size() );

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
  oco = document->whatAmIOn(plc);
  if (oco.isEmpty())
    {
      // clicked on an empty spot...
      if ((e->button() & RightButton) && isKiosk) kiosKontext->popup(e->pos());
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
      document->moveSosTo(e->pos());
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
      Objects where = document->whatAmIOn(pmt);
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
	  document->startMovingSos(plc,stillObjs);
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
      Objects where = document->whatAmIOn(pmt);
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
      document->selectObjects(QRect(pmt, plc).normalize());
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
	  p = new ConstrainedPoint(Object::toCurve(oco.getFirst()),plc);
	}
      else
	{
	  // add a normal Point...
	  p = new Point(plc);
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
  oldOverlay.append (new QRect (QPoint(0,0), size()));
};

void KigView::drawGrid()
{
  QPainter p( &stillPix );
  // this grid comes from KGeo
  p.save();
  p.setWindow(-size().width() / 2, -size().height() / 2, size().width(), size().height());
  //gridLines:
  const int unitInPixel = 40;
  const int maxX = (size().width() - 20)/(2*unitInPixel), maxY= (size().height() - 20)/(2*unitInPixel);
  p.setPen( QPen( lightGray, 0.5, DotLine ) );
  for ( int k = -maxX; k <= maxX; k++ )
    {
      p.drawLine( k * unitInPixel, -maxY * unitInPixel, k * unitInPixel, maxY * unitInPixel );
    }
  for ( int k = -maxY; k <= maxY; k++ )
    {
      p.drawLine( -maxX * unitInPixel, k * unitInPixel, maxX * unitInPixel, k * unitInPixel );
    }
  
  // axes:
  p.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
  p.drawLine( -maxX * unitInPixel, 0, maxX * unitInPixel, 0 );
  p.drawLine( 0, maxY * unitInPixel, 0, -maxY * unitInPixel );
  // numbers:
  for ( int k = -maxX; k <= maxX; k++ )
    {
      if ( k != 0 )
	{
	  p.drawText( (k-1) * unitInPixel, 2, 2 * unitInPixel, unitInPixel, AlignHCenter | AlignTop, QString().setNum( k ) );
	}
    }
  for ( int k = -maxY; k <= maxY; k++ )
    {
      if ( k != 0 )
	{
	  p.drawText( 2, ( k * unitInPixel ) - 13, 24, 24, AlignCenter, QString().setNum( -k ) );
	}
    }
  int right = maxX * unitInPixel;
  int top = -maxY * unitInPixel;
  p.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
  p.setBrush( Qt::gray );
  QPointArray a;
  a.setPoints ( 3, right + 6, - 3, right + 12 , 0, right + 6, 3 );
  p.drawPolygon( a, true );
  p.drawLine( right, 0, right + 5, 0 );
  a.setPoints ( 3, - 3, top - 6, 0, top - 13, 3, top - 6 );
  p.drawPolygon( a, true );
  p.drawLine( 0, top, 0, top - 6 );
  
  p.restore();
};

void KigView::updateWidget( bool needRedraw )
{
#undef SHOW_OVERLAY_RECTS
#ifdef SHOW_OVERLAY_RECTS
  QPainter debug (this);
  debug.setPen(Qt::yellow);
#endif // SHOW_OVERLAY_RECTS
  if (needRedraw)
    {
      // if necessary, we just redraw everything...
      bitBlt (this, 0, 0, &curPix);
    }
  else
    {
      // we undo our old changes...
      for (QRect* i = oldOverlay.first(); i; i = oldOverlay.next())
	bitBlt( this, i->topLeft(), &curPix, *i );
      // we add our new changes...
      for (QRect* i = overlay.first(); i; i = overlay.next())
	{
	  bitBlt( this, i->topLeft(), &curPix, *i );
#ifdef SHOW_OVERLAY_RECTS
	  debug.drawRect(*i);
#endif
	};
    };
  oldOverlay.setAutoDelete(true);
  oldOverlay.clear();
  oldOverlay.setAutoDelete(false);
  if (needRedraw)
    {
      oldOverlay.append (new QRect(QPoint(0,0), size()));
      overlay.setAutoDelete(true);
    }
  else
    {
      oldOverlay = overlay;
      overlay.setAutoDelete(false);
    };
  overlay.clear();
  return;
};

void KigView::drawObject(const Object* o, QPixmap& pix)
{
  QPainter p( &pix );
  o->drawWrap( p, true );
  o->getOverlayWrap(overlay, QRect(QPoint(0,0), size()));
};

void KigView::drawRect()
{
  QPainter p(&curPix);
  QPen pen(Qt::black, 1, Qt::DotLine);
  p.setPen(pen);
  p.setBrush(QBrush(Qt::cyan,Dense6Pattern));
  QRect* r = new QRect(plc, pmt);
  *r = r->normalize();
  p.drawRect(*r);
  overlay.append(r);
};

void KigView::drawPrelim()
{
  QPainter p(&curPix);
  document->getObc()->drawPrelim(p,mapFromGlobal(QCursor::pos()));
  document->getObc()->getPrelimOverlay(overlay,QRect(QPoint(0,0),size()), mapFromGlobal(QCursor::pos()));
};

void KigView::drawTbd()
{
  if (tbd.isNull()) return;
  QPainter p(&curPix);
  // tf = text formatting flags
  int tf = AlignLeft | AlignTop | DontClip | WordBreak;
  // we need the rect where we're going to paint text
  QRect* tmpRect = new QRect(p.boundingRect(pmt.x(), pmt.y(), 900, 50, tf, tbd));
  p.setPen(QPen(Qt::blue, 1, SolidLine));
  p.setBrush(Qt::NoBrush);
  p.drawText(pmt.x(), pmt.y(), 900 ,50, tf, tbd);
  overlay.append(tmpRect);
};

void KigView::resizeEvent(QResizeEvent*)
{
  curPix.resize(size());
  stillPix.resize( size() );
  redrawStillPix();
  updateWidget(true);
}

void KigView::drawObjects(const Objects& os, QPixmap& p )
{
  Objects::iterator it(os);
  Object* i;
  for (; (i = it.current()); ++it)
    drawObject(i,p);
}
void KigView::updateCurPix()
{
  // we make curPix look like stillPix again...
  for (QRect* i = oldOverlay.first(); i ; i = oldOverlay.next())
    bitBlt(&curPix, i->topLeft(), &stillPix, *i);
}
