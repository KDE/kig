// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "dragrectmode.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/kigpainter.h"

#include <QMouseEvent>
#include <qglobal.h>
#include <QAction>

DragRectMode::DragRectMode( const QPoint& start, KigPart& d, KigWidget& w )
  : KigMode( d ), mstart( start ), mnc( true ), mstartselected( true ),
    mcancelled( false )
{
  moved( start, w );
}

DragRectMode::DragRectMode( KigPart& d, KigWidget& w )
  : KigMode( d ), mnc( true ), mstartselected( false ),
    mcancelled( false )
{
  w.updateCurPix();
  w.updateWidget();
}

void DragRectMode::moved( const QPoint& p, KigWidget& w )
{
  // update the rect...
  w.updateCurPix();
  std::vector<QRect> overlay;
  if ( mstartselected )
  {
    KigPainter pt( w.screenInfo(), &w.curPix, mdoc.document() );
    pt.drawFilledRect( QRect( p,  mstart ) );
    overlay = pt.overlay();
  };
  w.updateWidget( overlay );
}

void DragRectMode::released( const QPoint& p, KigWidget& w, bool nc )
{
  if ( mstartselected )
  {
    mrect =  w.fromScreen( QRect( mstart, p ) );
    mret = mdoc.document().whatIsInHere( mrect, w );
    mnc = nc;

    mdoc.doneMode( this );
  };
}

void DragRectMode::enableActions()
{
  KigMode::enableActions();

  mdoc.aCancelConstruction->setEnabled( true );
}

std::vector<ObjectHolder*> DragRectMode::ret() const
{
  return mret;
}

bool DragRectMode::needClear() const
{
  return mnc;
}

void DragRectMode::moved( QMouseEvent* e, KigWidget& w )
{
  moved( e->pos(), w );
}

void DragRectMode::released( QMouseEvent* e, KigWidget& w )
{
  released( e->pos(), w, ! ( e->modifiers() & ( Qt::ControlModifier | Qt::ShiftModifier ) ) );
}

DragRectMode::~DragRectMode()
{
}

void DragRectMode::mouseMoved( QMouseEvent* e, KigWidget* w )
{
  moved( e, *w );
}

void DragRectMode::leftMouseMoved( QMouseEvent* e, KigWidget* w )
{
  moved( e, *w );
}

void DragRectMode::midMouseMoved( QMouseEvent* e, KigWidget* w )
{
  moved( e, *w );
}

void DragRectMode::rightMouseMoved( QMouseEvent* e, KigWidget* w )
{
  moved( e, *w );
}

void DragRectMode::leftReleased( QMouseEvent* e, KigWidget* w )
{
  released( e, *w );
}

void DragRectMode::midReleased( QMouseEvent* e, KigWidget* w )
{
  released( e, *w );
}

void DragRectMode::rightReleased( QMouseEvent* e, KigWidget* w )
{
  released( e, *w );
}

Rect DragRectMode::rect() const
{
  return mrect;
}

void DragRectMode::clicked( const QMouseEvent* e, KigWidget& w )
{
  clicked( e->pos(), w );
}

void DragRectMode::leftClicked( QMouseEvent* e, KigWidget* w )
{
  clicked( e, *w );
}

void DragRectMode::midClicked( QMouseEvent* e, KigWidget* w )
{
  clicked( e, *w );
}

void DragRectMode::rightClicked( QMouseEvent* e, KigWidget* w )
{
  clicked( e, *w );
}

void DragRectMode::clicked( const QPoint& p, KigWidget& )
{
  if ( !mstartselected )
  {
    mstartselected = true;
    mstart = p;
  };
}

bool DragRectMode::cancelled() const
{
  return mcancelled;
}

void DragRectMode::cancelConstruction()
{
  mcancelled = true;
  mdoc.doneMode( this );
}

