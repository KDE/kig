#include "kig_view.h"

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
  : QWidget(parent, name, WPaintClever | WResizeNoErase | WRepaintNoErase),
    stillPix(size()),
    curPix(size()),
    document(inDoc),
    plc(0,0),
    pmt(0,0),
    mode (NMode),
    nmode (nmMoving),
    cmode (cmMoving),
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
   connect(document, SIGNAL(selectionChanged(const Object*)), this, SLOT(redrawOneObject(const Object*)));
  connect( document, SIGNAL( startedMoving() ), this, SLOT( redrawStillPix() ) );
  connect( document, SIGNAL( cosMoved() ), this, SLOT( paintOnPartOfWidget()));
  connect( document, SIGNAL( stoppedMoving() ), this, SLOT( redrawStillPix() ) );
//   connect( document, SIGNAL( objectAdded() ), this, SLOT( redrawStillPix() ) );
  connect( document, SIGNAL( allChanged() ), this, SLOT( redrawStillPix() ) );
  connect( document, SIGNAL( modeChanged() ), this, SLOT( setMode() ) );
  setFocusPolicy(QWidget::ClickFocus);
  setMouseTracking(true);
  redrawStillPix();

  objectOverlayList.setAutoDelete(true);
};

KigView::~KigView()
{
  document->delView(this);
  delete kiosKontext;
};

void KigView::setReadOnly (bool ro)
{
  readOnly = ro;
};

void KigView::paintEvent(QPaintEvent*)
{
  paintOnWidget(true);
}

void KigView::mousePressEvent (QMouseEvent* e)
{
  pmt = plc = e->pos();
  oco = document->whatAmIOn(plc);
  switch (mode)
    {
    case SMode:
      // we use nmode for SMode too... (lazyness...)
      if (oco.empty()) nmode = nmClickedNowhere;
      else nmode = nmClickedForSelecting;
      break;
    case NMode:
      if (oco.empty())
	{
	  if (e->button() & MidButton) nmode = nmClickedForNewPoint;
	  else if (e->button() & LeftButton) nmode = nmClickedNowhere;
	  else if (e->button() & RightButton && isKiosk) { kiosKontext->popup(e->pos()); }
	  else { nmode = nmNothing; displayText(0); paintOnWidget(false); }
	}
      else 
	{
	  if (e->button() & RightButton) { nmode = nmNothing; displayText(0); paintOnWidget(false); }
	  else nmode = nmClickedForSelecting;
	};
      break;
    case CMode:
      if (!oco.empty() && (e->button() == LeftButton))
	cmode = cmClicked;
      else if (e->button() & MidButton)
	cmode = cmClickedForNewPoint;
      else cmode = cmNothing;
      break;
    default:
      break;
    };
//   kdDebug() << "new mode: " << (mode == CMode?"cmode":"nmode") << endl;
//   kdDebug() << "new cmode: " << cmode << endl;
//   kdDebug() << "new nmode: " << nmode << endl;
};

void KigView::mouseMoveEvent (QMouseEvent* e)
{
  pmt = e->pos();
  QString s;
  switch(mode)
    {
    case SMode:
      switch (nmode)
	{
	case nmClickedNowhere:
	  paintOnWidget(false);
	  break;
	case nmMoving:
	  oco = document->whatAmIOn(e->pos());
	  if (!oco.empty()) { setCursor (KCursor::handCursor()); displayText(oco.front()->vTBaseTypeName()); }
	  else { setCursor(KCursor::arrowCursor()); displayText(0); }
	  paintOnWidget(false);
	  break;
	case nmClickedForSelecting:
	  if ((plc-pmt).manhattanLength() > 3)
	    {
	      nmode = nmNothing;
	    };
	  break;
	default:
	  break;
	};
      break;
    case NMode:
      switch (nmode)
	{
	case nmClickedNowhere:
	  paintOnWidget(false);
	  break;
	case nmMoving:
	  oco = document->whatAmIOn(e->pos());
	  if (!oco.empty()) { setCursor (KCursor::handCursor()); displayText(oco.front()->vTBaseTypeName()); }
	  else { setCursor(KCursor::arrowCursor()); displayText(0); }
	  paintOnWidget(false);
	  break;
	case nmClickedForSelecting:
	  if ((plc-pmt).manhattanLength() > 3)
	    {
	      if ((oco & document->getSos()).empty()) 
		{
		  if (!(e->state() & ShiftButton)) document->clearSelection();
		  document->selectObject(oco.front());
		}
	      else
		{
		  document->startMovingSos(plc);
		  setMode();
		};
	    };
	  break;
	case nmClickedForNewPoint:
	  if ((plc-pmt).manhattanLength() > 3)
	    {
	      nmode = nmNothing;
	      displayText(0);
	      paintOnWidget(false);
	    };
	  break;
	default:
	  break;
	};
      break;
    case CMode:
      switch (cmode)
	{
	case cmMoving:
	  oco = document->whatAmIOn(e->pos());
	  if (!oco.empty() && (s = document->getObc()->wantArg(oco.front())))
	    {
	      setCursor (KCursor::handCursor());
	      displayText(s);
	      paintOnWidget(false);
	      break;
	    }
	  setCursor (KCursor::arrowCursor());
	  displayText(i18n("Constructing a %1").arg(document->getObc()->vTBaseTypeName()));
	  paintOnWidget(false);
	  break;
	case cmClicked:
	  if ((plc-pmt).manhattanLength() > 3)
	    cmode = cmNothing;
	  break;
	case cmClickedForNewPoint:
	  if ((e->pos() - plc).manhattanLength() > 3) cmode = cmNothing;
	  break;
	default:
	  break;
	};
      break;
    case MMode:
      document->moveSosTo(e->pos());
      paintOnWidget(false);
      break;
    default:
      break;
    };
};

void KigView::mouseReleaseEvent (QMouseEvent* e)
{
//   kdDebug() << "KigView::mouseReleaseEvent" << endl;
  Point* p;
  switch(mode)
    {
    case SMode:
      switch(nmode)
	{
	case nmClickedNowhere:
	  document->macroSelect(QRect(plc,pmt));
	  break;
	case nmClickedForSelecting:
	  document->macroSelect(oco.front());
	  break;
	default:
	  break;
	};
      break;
    case NMode:
      switch (nmode)
	{
	case nmClickedNowhere:
	  if (!(e->state() & ShiftButton)) document->clearSelection();
	  document->selectObjects (QRect(plc, pmt));
	  break;
	case nmClickedForNewPoint:
	  p = new Point(plc);
// 	  kdDebug() << "new point: " << p << endl;
	  document->addObject(p);
	  break;
	case nmClickedForSelecting:
          if ( oco.front()->getSelected() )
          {
              if ( e->state() & ShiftButton )
              {
                  document->unselect( oco.front() );
                  break;
              }
              else document->clearSelection();
          };
          if (!(e->state() & ShiftButton)) document->clearSelection();
          document->selectObject(oco.front());
          break;
	default:
	  break;
	};
      break;
    case CMode:
      switch(cmode)
	{
	case cmClicked:
	  if (document->getObc()->wantArg(oco.front()))
	    document->obcSelectArg(oco.front());
	  else {document->delObc(); document->selectObject(oco.front());}
	  break;
	case cmClickedForNewPoint:
	  p = new Point (e->pos());
	  if (!document->getObc()->wantArg(p)) {
	    delete p;
	    break;
	  }
	  document->addObject(p);
	  document->obcSelectArg(p);
	  break;
	default:
	  break;
	};
      break;
    case MMode:
      document->stopMovingSos();
      break;
    default:
      break;
    };
  setMode();
  nmode = nmMoving;
  cmode = cmMoving;
  displayText(0);
  paintOnWidget(false);
};

void KigView::setMode()
{
  if (document->getObc()) mode = CMode;
  else if (!document->getCos().empty()) mode = MMode;
  else if (document->isConstructingMacro()) mode = SMode;
  else mode = NMode;
}

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
  stillPix.resize( size() );
  stillPix.fill(Qt::white); 
  QPainter p(&stillPix );
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
  const Objects* obj = &( document->getStos() );
  if ( obj->empty() && document->getCos().empty()) obj = &( document->getObjects() );
  obj->draw( p,true );
  bitBlt(&curPix, 0,0,&stillPix);
  objectOverlayList.clear();
  objectOverlayList.append(new QRect(0,0,size().width(), size().height()));
  paintOnWidget( true );
};

void KigView::paintOnWidget( bool needRedraw )
{
  curPix.resize( size() );
  //maybe i should add an option to configure for this ?
// #define SHOW_OVERLAY_RECTS
#ifdef SHOW_OVERLAY_RECTS
  QPainter debug (this);
  debug.setPen(Qt::yellow);
#endif
  for (QRect* i =objectOverlayList.first(); i ; i = objectOverlayList.next())
    {
      bitBlt(&curPix, i->topLeft(), &stillPix, *i);
    };
  // since objectOverlayList has autoDelete turned on, this also
  // deletes all included QRects
  QPtrList<QRect> nList;

  const Objects& objs = document->getCos();
  QPainter p( &curPix );
  objs.draw( p,true );
  nList = objs.getOverlay(QRect(0,0,width(), height()));

  if ((mode==NMode || mode == SMode )&& nmode == nmClickedNowhere)
    {
      QPen pen(Qt::black, 1, Qt::DotLine);
      p.setPen(pen);
      p.setBrush(QBrush(Qt::cyan,Dense6Pattern));
      QRect r(plc, pmt);
      p.drawRect(r);
      // TODO: get this right
//       nList.append(new QRect(plc,r.width()+1, 1));
//       nList.append(new QRect(plc,1, r.height()+1));
//       nList.append(new QRect(pmt,1, r.height()+1));
//       nList.append(new QRect(plc,1, r.height()+1));
    };

  if (mode==CMode && cmode == cmMoving)
    {
      document->getObc()->drawPrelim(p,mapFromGlobal(QCursor::pos()));
      document->getObc()->getPrelimOverlay(nList,QRect(0,0,width(), height()), mapFromGlobal(QCursor::pos()));
    };
  if (tbd)
    {
      // tf = text formatting flags
      int tf = AlignLeft | AlignTop | DontClip | WordBreak;
      // we need the rect where we're going to paint text
      QRect* tmpRect = new QRect(p.boundingRect(pmt.x(), pmt.y(), 900, 50, tf, tbd));
      p.setPen(QPen(Qt::blue, 1, SolidLine));
      p.setBrush(Qt::NoBrush);
      p.drawText(pmt.x(), pmt.y(), 900 ,50, tf, tbd);
      nList.append(tmpRect);
//       p.drawText(pmt, tbd);
    };
  if (needRedraw) { bitBlt (this, 0, 0, &curPix); return; };
  for (QRect* i = nList.first(); i; i = nList.next())
    {
      bitBlt( this, i->topLeft(), &curPix, *i );
#ifdef SHOW_OVERLAY_RECTS
      debug.drawRect(*i);
#endif
    };
  for (QRect* i = objectOverlayList.first(); i; i = objectOverlayList.next())
    bitBlt( this, i->topLeft(), &curPix, *i );
  objectOverlayList.clear();
  objectOverlayList = nList;
}

void KigView::redrawOneObject(const Object* o)
{
  QPainter p;
  p.begin(&stillPix);
  o->drawWrap(p, true);
  p.end();
  p.begin(&curPix);
  o->drawWrap(p,true);
  p.end();
  p.begin(this);
  o->drawWrap(p,true);
  p.end();
}

void KigView::resizeEvent(QResizeEvent*)
{
  redrawStillPix();
}
