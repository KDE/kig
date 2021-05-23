// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "base_mode.h"

#include "popup/popup.h"
#include "popup/objectchooserpopup.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../kig/kig_document.h"

#include <QEvent>
#include <KCursor>
#include <QMouseEvent>

BaseMode::BaseMode( KigPart& d )
  : KigMode( d )
{
}

BaseMode::~BaseMode()
{
}

void BaseMode::leftClicked( QMouseEvent* e, KigWidget* v )
{
  // touch screens don't send a mouseMoved event before a click event,
  // apparently, so we simulate it.
  mouseMoved( e, v );

  // get rid of text still showing...
  v->updateCurPix();
  v->updateWidget();

  mplc = e->pos();
  moco = mdoc.document().whatAmIOn( v->fromScreen( mplc ), *v );

  if( moco.empty() )
  {
    // clicked on an empty spot --> we show the rectangle for
    // selecting stuff...
    dragRect( mplc, *v );
  }
  else
  {
    // the user clicked on some object.. --> this could either mean
    // that he/she wants to select the object or that he wants to
    // start moving it.  We assume nothing here, we wait till he
    // either moves some 4 pixels, or till he releases his mouse
    // button in leftReleased() or mouseMoved()...
  };
}

void BaseMode::leftMouseMoved( QMouseEvent* e, KigWidget* w )
{
  if( !moco.empty() && ( mplc - e->pos() ).manhattanLength() > 3 )
    dragObject( moco, mplc, *w,
                ( e->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier ) ) != 0
      );
}

void BaseMode::leftReleased( QMouseEvent* e, KigWidget* v )
{
  if( (mplc - e->pos()).manhattanLength() > 4 ) return;

  ObjectHolder* o = 0;
  bool keyCtrl = ( e->modifiers() & Qt::ControlModifier ) != 0;
  bool keyShift = ( e->modifiers() & Qt::ShiftModifier ) != 0;
  if ( ! moco.empty() )
  {
    if ( keyShift )
    {
      int id = ObjectChooserPopup::getObjectFromList( e->pos(), v, moco );
      if ( id >= 0 )
        o = moco[id];
    }
    else
      o = moco.front();
  }
  leftClickedObject( o, e->pos(), *v, keyCtrl );
}

void BaseMode::midClicked( QMouseEvent* e, KigWidget* v )
{
  // get rid of text still showing...
  v->updateCurPix();
  v->updateWidget();

  mplc = e->pos();
  moco = mdoc.document().whatAmIOn( v->fromScreen( e->pos() ), *v );
}

void BaseMode::midReleased( QMouseEvent* e, KigWidget* v )
{
  if( (e->pos() - mplc).manhattanLength() > 4 ) return;

  midClicked( mplc, *v );
}

void BaseMode::rightClicked( QMouseEvent* e, KigWidget* w )
{
  // get rid of text still showing...
  w->updateCurPix();
  w->updateWidget();
  // set a normal cursor...
  w->setCursor( Qt::ArrowCursor );

  mplc = e->pos();
  moco = mdoc.document().whatAmIOn( w->fromScreen( mplc ), *w );

  rightClicked( moco, mplc, *w );
}

void BaseMode::mouseMoved( QMouseEvent* e, KigWidget* w )
{
  std::vector<ObjectHolder*> os = mdoc.document().whatAmIOn( w->fromScreen( e->pos() ), *w );
  mouseMoved( os, e->pos(), *w, e->modifiers() & Qt::ShiftModifier );
}

void BaseMode::dragRect( const QPoint&, KigWidget& )
{
}

void BaseMode::leftClickedObject( ObjectHolder*, const QPoint&,
                                  KigWidget&, bool )
{
}

void BaseMode::dragObject( const std::vector<ObjectHolder*>&, const QPoint&,
                           KigWidget&, bool )
{
}

void BaseMode::enableActions()
{
  KigMode::enableActions();
}

std::vector<ObjectHolder*> BaseMode::oco()
{
  return moco;
}

QPoint BaseMode::pointLocation()
{
  return mplc;
}
