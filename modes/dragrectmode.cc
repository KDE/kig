// dragrectmode.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "dragrectmode.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../misc/kigpainter.h"

#include <qevent.h>
#include <qglobal.h>

DragRectMode::DragRectMode( const QPoint& start, KigDocument& d, KigWidget& w )
  : KigMode( d ), mstart( start ), mnc( false )
{
  mdoc.emitStatusBarText( 0 );
  moved( start, w );
}

void DragRectMode::moved( const QPoint& p, KigWidget& w )
{
  // update the rect...
  w.updateCurPix();
  KigPainter pt( w.screenInfo(), &w.curPix, mdoc );
  pt.drawFilledRect( QRect( p,  mstart ) );
  w.updateWidget( pt.overlay() );
}

void DragRectMode::released( const QPoint& p, KigWidget& w, bool nc )
{
  const Rect r =  w.fromScreen( QRect( mstart, p ) );
  mret = mdoc.whatIsInHere( r, w );
  mnc = nc;

  mdoc.doneMode( this );
}

void DragRectMode::enableActions()
{
  KigMode::enableActions();
}

Objects DragRectMode::ret() const
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
  released( e->pos(), w, ! ( e->state() & ( ControlButton | ShiftButton ) ) );
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

