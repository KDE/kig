// selectionmode.cc
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

#include "selectionmode.h"

SelectionModeBase::SelectionModeBase( KigDocument* d )
  : KigMode( d )
{
}

SelectionModeBase::~SelectionModeBase()
{
}

void SelectionModeBase::leftClicked( QMouseEvent* e, KigWidget* v )
{
  mplc = e->pos();
  oco = mDoc->whatAmIOn( v->fromScreen( mplc ), v->screenInfo() );

  if( oco.empty() )
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

void SelectionModeBase::leftMouseMoved( QMouseEvent* e, KigWidget* w )
{
  // clicked on an object, move it ?
  if( ( plc - e->pos() ).manhattanLength() > 3 )
  {
    dragObject( moco, mplc, *w );
  };
}

void SelectionModeBase::leftReleased( QMouseEvent* e, KigWidget* v )
{
  if( (mplc - e->pos()).manhattanLength() > 4 ) return;

  Objects cos; // objects whose selection we change..
  if( !mselection.contains( oco.front() ) )
  {
    // clicked on an object that wasn't selected....
    // we only use oco.front(), since that's what the user
    // expects.  E.g. if he clicks on a point which is on a line,
    // then oco will contain first the point, then the line.
    // Obviously, we only want the point...
    if (!(e->state() & (ControlButton | ShiftButton)))
    {
      cos = mselection;
        clearSelection();
    };
    selectObject( oco.front() );
    cos.upush( oco.front() );
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

  KigPainter p( v->screenInfo(), &v->stillPix );
  p.drawObjects( cos );
  v->updateCurPix( p.overlay() );
  v->updateWidget();
}

void SelectionModeBase::midClicked( QMouseEvent* e, KigWidget* v )
{
  mplc = e->pos();
  moco = mDoc->whatAmIOn( v->fromScreen( e->pos() ), v->screenInfo() );
  // get rid of text still showing...
  v->updateCurPix();
  v->updateWidget();
}

void SelectionModeBase::midReleased( QMouseEvent* e, KigWidget* v )
{
  if( (e->pos() - plc).manhattanLength() > 4 ) return;

  // construct a new point..
  Point* pt = NormalPoint::sensiblePoint( v->fromScreen( plc ), *mDoc, *v );
  pt->calcForWidget( *v );

  if ( wantObject( pt, *mDoc, *v ) )
  {
    mDoc->addObject( pt );

    selectObject( pt, *v );

    // refresh the screen...
    v->redrawScreen();
    v->updateScrollBars();
  }
  else
    delete pt;
}

void SelectionModeBase::rightClicked( QMouseEvent* e, KigWidget* w )
{
  mplc = e->pos();
  moco = mDoc->whatAmIOn( w->fromScreen( mplc ), w->screenInfo() );
  // get rid of text still showing...
  w->updateCurPix();
  w->updateWidget();

  rightClicked( moco, *w );
}

void SelectionModeBase::mouseMoved( QMouseEvent* e, KigWidget* w )
{
  Objects os = mDoc->whatAmIOn( w->fromScreen( e->pos() ), w->screenInfo() );
  v->updateCurPix();

  if ( ! os.empty() && wantObject( *(os.front() ), *w ))
  {
    // the cursor is over an object, show object type next to cursor
    // and set statusbar text

    v->setCursor( KCursor::handCursor() );

    QString typeName = tmp.front()->vTBaseTypeName();
    mDoc->emitStatusBarText( i18n( "Select this %1" ).arg( typeName ) );

    KigPainter p( v->screenInfo(), &v->curPix );
    QPoint point = e->pos();
    point.setX(point.x()+15);

    p.drawTextStd( point, typeName );
    v->updateWidget( p.overlay() );
  }
  else
  {
    w->setCursor( KCursor::arrowCursor() );
    mDoc->emitStatusBarText( 0 );
    w->updateWidget();
  }
}

void SelectionModeBase::enableActions()
{
  KigMode::enableActions();
}

void SelectionModeBase::dragRect( const QPoint& p, KigWidget& w )
{
}

bool SelectionModeBase::wantObject( const Object&, KigWidget& )
{
  return true;
}

void SelectionModeBase::rightClicked( const Objects& os, KigWidget& w )
{
}

void SelectionModeBase::dragObject( const Objects& objectsClickedOn, const QPoint& pointClickedOn, KigWidget& w )
{
}

void SelectionModeBase::clearSelection( KigWidget& w )
{
  using namespace std;
  for_each( mselection.begin(), mselection.end(),
            bind2nd( mem_fun( &Object::setSelected ), false ) );
  mselection.clear();
  selectionChanged( w );
}

void SelectionModeBase::selectObject( Object* o, KigWidget& w )
{
  o->setSelected( true );
  mselection |= o;
  selectionChanged( w );
}

void SelectionModeBase::finish()
{
  kapp->eventLoop()->exitLoop();
}

const Objects& SelectionModeBase::selection() const
{
  return mselection;
}
