// construction_modes.cc
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

#include "constructing.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../misc/kigpainter.h"
#include "../objects/object.h"
#include "../objects/point.h"
#include "normal.h"

#include <kcursor.h>
#include <kaction.h>

#include <functional> // for std::mem_fun and std::bind2nd

/*
void ConstructingMode::rightReleased( QMouseEvent* e, KigView* v )
{
  if( (plc - e->pos()).manhattanLength() > 3 ) return;
  if( !oco.empty() )
    {
      // show a popup menu...
      // first clear the text..
      v->updateCurPix();
      mDoc->emitStatusBarText( 0 );
      v->updateWidget();
      Object* o = oco.front();
//       KigObjectsPopup* m = 0;
      Objects os;
      os.append( o );
//       if( osa.contains( o ) ) m = popup( os, v );
    };
}

*/

void PointConstructionMode::updateScreen( KigView* v )
{
  v->clearStillPix();
  KigPainter p( v->showingRect(), &v->stillPix );
  p.drawGrid( mDoc->coordinateSystem() );
  p.drawObjects( mDoc->objects() );
  v->updateCurPix( p.overlay() );
  v->updateWidget();
}

void PointConstructionMode::leftClicked( QMouseEvent* e, KigView* )
{
  plc = e->pos();
}

void PointConstructionMode::leftReleased( QMouseEvent* e, KigView* v )
{
  if( ( plc - e->pos() ).manhattanLength() > 4 ) return;
  updatePoint( v->fromScreen( e->pos() ), v->pixelWidth() );
  finish( v );
}

void PointConstructionMode::midClicked( QMouseEvent* e, KigView* v )
{
  leftClicked( e, v );
}

void PointConstructionMode::midReleased( QMouseEvent* e, KigView* v )
{
  leftReleased( e, v );
}

PointConstructionMode::PointConstructionMode( NormalMode* b,
                                              KigDocument* d )
  : KigMode( d ),
    mp( new NormalPoint( new FixedPointImp( Coordinate( 0, 0 ) ) ) ),
    mprev( b )
{
}

void PointConstructionMode::mouseMoved( QMouseEvent* e, KigView* v )
{
  updatePoint( v->fromScreen( e->pos() ), v->pixelWidth() );
  v->updateCurPix();
  KigPainter p( v->showingRect(), &v->curPix );
  p.drawObject( mp, true );
  v->updateWidget( p.overlay() );
}

void PointConstructionMode::updatePoint( const Coordinate& c, double fault )
{
  mp->setImp( NormalPoint::NPImpForCoord( c, mDoc, fault, mp->imp() ) );
}

void PointConstructionMode::finish( KigView* v )
{
  mDoc->addObject( mp );

  updateScreen( v );

  mprev->clearSelection();
  NormalMode* s = mprev;
  KigDocument* d = mDoc;
  delete this;
  d->setMode( s );
}

StdConstructionMode::StdConstructionMode( Object* obc, NormalMode* b,
                                          KigDocument* d )
  : PointConstructionMode( b, d ),
    mobc( obc )
{
}

StdConstructionMode::~StdConstructionMode()
{
}

void StdConstructionMode::leftClicked( QMouseEvent* e, KigView* v )
{
  plc = e->pos();
  oco = mDoc->whatAmIOn( v->fromScreen( e->pos() ), 3*v->pixelWidth() );
  v->updateCurPix();
  updatePoint( v->fromScreen( e->pos() ), v->pixelWidth() );
  if( !oco.empty() )
  {
    KigPainter p( v->showingRect(), &v->curPix );
    QString s = oco.front()->vTBaseTypeName();
    p.drawTextStd( e->pos(), s );
    v->updateWidget( p.overlay() );
    mDoc->emitStatusBarText( s );
  }
  else
  {
    mDoc->emitStatusBarText( 0 );
    v->updateWidget();
  };
}

void StdConstructionMode::leftReleased( QMouseEvent* e, KigView* v )
{
  if( ( e->pos() - plc ).manhattanLength() > 4 ) return;

  // if the user clicked on an obj, first see if we can use that...
  if( ! oco.empty() && mobc->wantArg( oco.front() ) )
    selectArg( oco.front(), v );
  else
  {
    // oco is empty or the obc doesn't like it...
    // so we try if it wants our point..
    if ( mobc->wantArg( mp ) )
    {
      mDoc->addObject( mp );
      selectArg( mp, v );
    };
  };
}

void StdConstructionMode::mouseMoved( QMouseEvent* e, KigView* v )
{
  // objects under cursor
  Coordinate c = v->fromScreen( e->pos() );
  Objects ouc = mDoc->whatAmIOn( c, 3*v->pixelWidth() );
  updatePoint( c, v->pixelWidth() );

  v->updateCurPix();
  KigPainter p( v->showingRect(), &v->curPix );
  if ( ! ouc.empty() && mobc->wantArg( ouc.front() ) )
  {
    // the object wants the arg currently under the cursor...
    // draw mobc...
    p.drawPrelim( mobc, ouc.front() );

    QString o = mobc->wantArg( ouc.front() );

    mDoc->emitStatusBarText( o );
    p.drawSimpleText( v->fromScreen( e->pos() ), o );
    v->setCursor (KCursor::handCursor());
  }
  // see if mobc wants the point we're dragging around...
  else if ( mobc->wantArg( mp ) )
  {
    // it does...
    // so we first draw the point, then mobc, and then the text that
    // wantArg returns...
    p.drawObject( mp, true );
    p.drawPrelim( mobc, mp );

    QString s = mobc->wantArg( mp );
    mDoc->emitStatusBarText( s );

    // we draw the text a bit to the bottom so it doesn't end up on
    // top of the point...

    double i = v->pixelWidth();
    Coordinate d( i, i );
    p.drawSimpleText( c + d*2, s );
    v->setCursor( KCursor::blankCursor() );
  }
  else
  {
    // mobc doesn't want either...
    // we just tell the user what he's constructing...
    QString o = i18n( "Constructing a %1" ).arg( mobc->vTBaseTypeName() );
    mDoc->emitStatusBarText( o );
    p.drawSimpleText( v->fromScreen( e->pos() ), o );
    v->setCursor( KCursor::arrowCursor() );
  };
  v->updateWidget( p.overlay() );
}

void StdConstructionMode::midClicked( QMouseEvent* e, KigView* )
{
  plc = e->pos();
}

void StdConstructionMode::midReleased( QMouseEvent* e, KigView* v )
{
  if ( (e->pos() - plc).manhattanLength() > 4 ) return;
  updatePoint( v->fromScreen( e->pos() ), v->pixelWidth() );
  mDoc->addObject( mp );
  selectArg( mp, v );
}

void StdConstructionMode::selectArg( Object* o, KigView* v )
{
  if ( o == mp )
    // if we're selecting our mp, we need a new one...
    mp = mp->copy();
  bool finished = mobc->selectArg( o );
  if( finished )
  {
    mobc->calc();
    mDoc->addObject( mobc );
  };

  updateScreen( v );

  if( finished )
  {
    // delete ourselves...
    mprev->clearSelection();
    NormalMode* s = mprev;
    KigDocument* d = mDoc;
    delete this;
    d->setMode( s );
  }
}

void StdConstructionMode::enableActions()
{
  PointConstructionMode::enableActions();
}

void StdConstructionMode::cancelConstruction()
{
  delete mobc;
  NormalMode* p = mprev;
  KigDocument* d = mDoc;
  delete this;
  d->setMode( p );
}

PointConstructionMode::~PointConstructionMode()
{
  delete mp;
}

void PointConstructionMode::enableActions()
{
  KigMode::enableActions();

  mDoc->aCancelConstruction->setEnabled( true );
}

void PointConstructionMode::cancelConstruction()
{
  NormalMode* p = mprev;
  KigDocument* d = mDoc;
  delete this;
  d->setMode( p );
}
