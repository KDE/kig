// normal.cc
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

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
#include "normal.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../objects/normalpoint.h"
#include "../misc/kigpainter.h"
#include "popup.h"
#include "moving.h"
#include "macro.h"

#include <qevent.h>
#include <kcursor.h>
#include <klocale.h>
#include <kaction.h>
#include <kcommand.h>

NormalMode::NormalMode( KigDocument* d )
  : KigMode( d )
{
}

NormalMode::~NormalMode()
{
}

void NormalMode::leftClicked( QMouseEvent* e, KigView* v )
{
  plc = e->pos();
  oco = mDoc->whatAmIOn( v->fromScreen( e->pos() ), 2*v->pixelWidth() );
  if( oco.empty() )
  {
    // clicked on an empty spot --> we show the rectangle for
    // selecting stuff...
    // FIXME: make this into a separate KigMode...
    // make it into a KigMode which returns a number of selected
    // objects on "exit", so we can use it from other modes as
    // well...
    v->updateCurPix();
    v->updateWidget();
    mDoc->emitStatusBarText( 0 );
  }
  else
  {
    // the user clicked on some object.. --> this could either mean
    // that he/she wants to select the object or that he wants to
    // start moving it.  We assume nothing here, we wait till he
    // either moves some 4 pixels, or till he releases his mouse
    // button in leftReleased() or mouseMoved()...
  };
};

void NormalMode::leftMouseMoved( QMouseEvent* e, KigView* v )
{
  if( oco.empty() )
  {
    // dragging the selection rect...
    v->updateCurPix();
    KigPainter p( v->showingRect(), &v->curPix );
    p.drawFilledRect( QRect( e->pos(),  plc ) );
    v->updateWidget( p.overlay() );
  }
  else
  {
    // clicked on an object, move it ?
    if( ( plc - e->pos() ).manhattanLength() > 3 )
    {
      // yes, we move it...
      // now to determine what to move...
      if( ( oco & sos ).empty() )
      {
        // the user clicked on something that is currently not
        // selected... --> we select it, taking the Ctrl- and
        // Shift-buttons into account...
        if (!(e->state() & (ControlButton | ShiftButton)))
          clearSelection();
        selectObject(oco.front());
      }

      MovingMode* m = new MovingMode( sos, v->fromScreen( plc ),
                                      this, v, mDoc );
      mDoc->setMode( m );
    };
  };
};

void NormalMode::leftReleased( QMouseEvent* e, KigView* v )
{
  Objects cos; // objects whose selection changed..
  if( oco.empty() )
  {
    // the rect for selecting stuff...
    if (!(e->state() & (ControlButton | ShiftButton)))
    {
      cos = sos;
      clearSelection();
    };
    const Rect r =  v->fromScreen( QRect( plc, e->pos() ) );
    Objects os = mDoc->whatIsInHere( r );
    for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
      if ( !cos.contains( *i ) ) cos.push_back( *i );
    selectObjects( os );
  }
  else
  {
    if( (plc - e->pos()).manhattanLength() > 4 ) return;
    if( !sos.contains( oco.front() ) )
    {
      // clicked on objects that weren't selected....
      // we only use oco.front(), since that's what the user
      // expects.  E.g. if he clicks on a point which is on a line,
      // then oco will contain first the point, then the line.
      // Obviously, we only want the point...
      if (!(e->state() & (ControlButton | ShiftButton)))
      {
        cos = sos;
        clearSelection();
      };
      selectObject( oco.front() );
      cos.push_back( oco.front() );
    }
    else
    {
      // clicked on selected objects...
      // we only use oco.front(), since that's what the user
      // expects.  E.g. if he clicks on a point which is on a line,
      // then oco will contain first the point, then the line.
      // Obviously, we only want the point...
      unselectObject( oco.front() );
      cos.push_back( oco.front() );
    };
  };
  KigPainter p( v->showingRect(), &v->stillPix );
  p.drawObjects( cos );
  v->updateCurPix( p.overlay() );
  v->updateWidget();
}

void NormalMode::midClicked( QMouseEvent* e, KigView* v )
{
  plc = e->pos();
  oco = mDoc->whatAmIOn( v->fromScreen( e->pos() ), 2*v->pixelWidth() );
  // get rid of text still showing...
  v->updateCurPix();
  v->updateWidget();
}

void NormalMode::midMouseMoved( QMouseEvent*, KigView* )
{
};

void NormalMode::midReleased( QMouseEvent* e, KigView* v )
{
  // moved too far
  if( (e->pos() - plc).manhattanLength() > 4 ) return;
  // clicked on some other object -> ConstrainedPoint ?
  Point* pt;
  pt = NormalPoint::sensiblePoint( v->fromScreen( plc ),
                                   *mDoc, 3*v->pixelWidth() );
  mDoc->addObject( pt );

  // refresh the screen...
  v->clearStillPix();
  KigPainter p( v->showingRect(), &v->stillPix );
  p.drawGrid( mDoc->coordinateSystem() );
  p.drawObjects( mDoc->objects() );

  v->updateCurPix( p.overlay() );
  v->updateWidget();
}

void NormalMode::rightClicked( QMouseEvent* e, KigView* v )
{
  plc = e->pos();
  oco = mDoc->whatAmIOn( v->fromScreen( e->pos() ), 2*v->pixelWidth() );

  // get rid of text still showing...
  v->updateCurPix();
  // commit this to the widget...
  v->updateWidget();
  // set a normal cursor...
  v->setCursor( KCursor::arrowCursor() );

  if( !oco.empty() )
  {
    if( !sos.contains( oco.front() ) )
    {
      clearSelection();
      selectObject( oco.front() );
    };
    // show a popup menu...
    KigObjectsPopup* m = popup( sos );
    if( m ) m->exec( v->mapToGlobal( plc ) );
  }
  else
  {
    KigDocumentPopup* m = popup( mDoc );
    if( m ) m->exec( v->mapToGlobal( plc ) );
  };
}

void NormalMode::rightMouseMoved( QMouseEvent*, KigView* )
{
  // this is handled by the popup menus ( see rightClicked() )
};

void NormalMode::rightReleased( QMouseEvent*, KigView* )
{
  // this is handled by the popup menus ( see rightClicked() )
}

void NormalMode::mouseMoved( QMouseEvent* e, KigView* v )
{
  // the user is just moving around his cursor, without having clicked
  // anywhere...
  QPoint p = e->pos();
  const Objects tmp = mDoc->whatAmIOn( v->fromScreen( p ),
                                       2 * v->pixelWidth()
    );
  v->updateCurPix();
  if( tmp.empty() )
  {
    v->setCursor( KCursor::arrowCursor() );
    mDoc->emitStatusBarText( 0 );
    v->updateWidget();
  }
  else
  {
    v->setCursor( KCursor::handCursor() );
    QString typeName = tmp.front()->vTBaseTypeName();
    QString shownText = i18n( "Select this %1" ).arg( typeName );
    mDoc->emitStatusBarText( shownText );
    KigPainter p( v->showingRect(), &v->curPix );
    p.drawTextStd( e->pos(), typeName );
    v->updateWidget( p.overlay() );
  };
}

void NormalMode::enableActions()
{
  KigMode::enableActions();
  mDoc->aMNewSegment->setEnabled( true );
  mDoc->aMNewPoint->setEnabled( true );
  mDoc->aMNewCircle->setEnabled( true );
  mDoc->aMNewLine->setEnabled( true );
  mDoc->aMNewOther->setEnabled( true );
  mDoc->aDeleteObjects->setEnabled( true );
  mDoc->aShowHidden->setEnabled( true );
  mDoc->aNewMacro->setEnabled( true );
  mDoc->history()->updateActions();
}

void NormalMode::deleteObjects()
{
  // TODO...
}

void NormalMode::selectObject( Object* o )
{
  sos.push_back( o );
  o->setSelected( true );
}

void NormalMode::selectObjects( Objects& os )
{
  // grr.. just noticed that Objects is a QPtrList :(
  // hehe, don't you love this c++ stuff ;)
  std::for_each( os.begin(), os.end(),
                 std::bind1st(
                   std::mem_fun( &NormalMode::selectObject ),
                   this ) );
//   for( Objects::iterator i = os.begin(); i != os.end(); ++i )
//   {
//     if( ! sos.contains( *i ) ) sos.push_back( *i );
//     (*i)->setSelected( true );
//   };
}

void NormalMode::unselectObject( Object* o )
{
  o->setSelected( false );
  sos.remove( o );
}

void NormalMode::clearSelection()
{
  for ( Objects::iterator i = sos.begin(); i != sos.end(); ++i )
    (*i)->setSelected( false );
  sos.clear();
}

KigObjectsPopup* NormalMode::popup( const Objects& )
{
  return 0;
}

KigDocumentPopup* NormalMode::popup( KigDocument* )
{
  return 0;
}

void NormalMode::showHidden()
{
  const Objects& os = mDoc->objects();
  for (Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    (*i)->setShown(true);
  // TODO: update the (which ?) screen...
}

void NormalMode::newMacro()
{
//   DefineMacroMode* m = new DefineMacroMode( mDoc, this );

//   mDoc->setMode( m );
}

void NormalMode::objectsAdded()
{
  // i know this is ugly :(
  static_cast<KigView*>(mDoc->widget())->redrawScreen();
}

void NormalMode::objectsRemoved()
{
  static_cast<KigView*>(mDoc->widget())->redrawScreen();
}
