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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "dragrectmode.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/kigpainter.h"

#include <tqevent.h>
#include <tqglobal.h>
#include <kaction.h>

DragRectMode::DragRectMode( const TQPoint& start, KigPart& d, KigWidget& w )
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

void DragRectMode::moved( const TQPoint& p, KigWidget& w )
{
  // update the rect...
  w.updateCurPix();
  std::vector<TQRect> overlay;
  if ( mstartselected )
  {
    KigPainter pt( w.screenInfo(), &w.curPix, mdoc.document() );
    pt.drawFilledRect( TQRect( p,  mstart ) );
    overlay = pt.overlay();
  };
  w.updateWidget( overlay );
}

void DragRectMode::released( const TQPoint& p, KigWidget& w, bool nc )
{
  if ( mstartselected )
  {
    mrect =  w.fromScreen( TQRect( mstart, p ) );
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

void DragRectMode::moved( TQMouseEvent* e, KigWidget& w )
{
  moved( e->pos(), w );
}

void DragRectMode::released( TQMouseEvent* e, KigWidget& w )
{
  released( e->pos(), w, ! ( e->state() & ( ControlButton | ShiftButton ) ) );
}

DragRectMode::~DragRectMode()
{
}

void DragRectMode::mouseMoved( TQMouseEvent* e, KigWidget* w )
{
  moved( e, *w );
}

void DragRectMode::leftMouseMoved( TQMouseEvent* e, KigWidget* w )
{
  moved( e, *w );
}

void DragRectMode::midMouseMoved( TQMouseEvent* e, KigWidget* w )
{
  moved( e, *w );
}

void DragRectMode::rightMouseMoved( TQMouseEvent* e, KigWidget* w )
{
  moved( e, *w );
}

void DragRectMode::leftReleased( TQMouseEvent* e, KigWidget* w )
{
  released( e, *w );
}

void DragRectMode::midReleased( TQMouseEvent* e, KigWidget* w )
{
  released( e, *w );
}

void DragRectMode::rightReleased( TQMouseEvent* e, KigWidget* w )
{
  released( e, *w );
}

Rect DragRectMode::rect() const
{
  return mrect;
}

void DragRectMode::clicked( const TQMouseEvent* e, KigWidget& w )
{
  clicked( e->pos(), w );
}

void DragRectMode::leftClicked( TQMouseEvent* e, KigWidget* w )
{
  clicked( e, *w );
}

void DragRectMode::midClicked( TQMouseEvent* e, KigWidget* w )
{
  clicked( e, *w );
}

void DragRectMode::rightClicked( TQMouseEvent* e, KigWidget* w )
{
  clicked( e, *w );
}

void DragRectMode::clicked( const TQPoint& p, KigWidget& )
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

