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
#include "../objects/normalpoint.h"
#include "../objects/label.h"
#include "normal.h"

#include <kcursor.h>
#include <klocale.h>
#include <kaction.h>
#include <klineeditdlg.h>

#include <functional> // for std::mem_fun and std::bind2nd

void PointConstructionMode::updateScreen( KigView* v )
{
  v->clearStillPix();
  KigPainter p( v->screenInfo(), &v->stillPix );
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
  updatePoint( v->fromScreen( e->pos() ), v->screenInfo() );
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

PointConstructionMode::PointConstructionMode( NormalMode* b, KigDocument* d )
  : KigMode( d ),
    mp( NormalPoint::fixedPoint( Coordinate( 0, 0 ) ) ),
    mprev( b )
{
}

void PointConstructionMode::mouseMoved( QMouseEvent* e, KigView* v )
{
  v->setCursor( KCursor::blankCursor() );
  updatePoint( v->fromScreen( e->pos() ), v->screenInfo() );
  v->updateCurPix();
  KigPainter p( v->screenInfo(), &v->curPix );
  p.drawObject( mp, true );
  v->updateWidget( p.overlay() );
}

void PointConstructionMode::updatePoint( const Coordinate& c, const ScreenInfo& si )
{
  mp->redefine( c, *mDoc, 3*si.pixelWidth() );
  mp->calc( si );
}

void PointConstructionMode::finish( KigView* v )
{
  mDoc->addObject( mp );
  mp = mp->copy();
  mp->calc( v->screenInfo() );

  updateScreen( v );

  mprev->clearSelection();
  NormalMode* s = mprev;
  KigDocument* d = mDoc;
  delete this;
  d->setMode( s );
}

StdConstructionMode::StdConstructionMode( StdConstructibleType* t,
                                          NormalMode* b, KigDocument* d )
  : PointConstructionMode( b, d ), mtype( t )
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
  updatePoint( v->fromScreen( e->pos() ), v->screenInfo() );
  if( !oco.empty() )
  {
    KigPainter p( v->screenInfo(), &v->curPix );
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
  if( ! oco.empty() &&
      mtype->wantArgs( osa.with( oco.front() ) ) != Object::NotGood )
    selectArg( oco.front(), v );
  else
  {
    // oco is empty or the obc doesn't like it...
    // so we try if it wants our point..
    if ( mtype->wantArgs( osa.with( mp ) ) != Object::NotGood )
    {
      updatePoint( v->fromScreen( plc ), v->screenInfo() );
      mDoc->addObject( mp );
      selectArg( mp, v );
    };
  };
}

void StdConstructionMode::mouseMoved( QMouseEvent* e, KigView* v )
{
  Coordinate c = v->fromScreen( e->pos() );
  // objects under cursor
  Objects ouc = mDoc->whatAmIOn( c, 3*v->pixelWidth() );
  updatePoint( c, v->screenInfo() );

  // set the text next to the arrow cursor like in modes/normal.cc
  QPoint point = e->pos();
  point.setX(point.x()+15);

  v->updateCurPix();
  KigPainter p( v->screenInfo(), &v->curPix );
  if ( ! ouc.empty() && mtype->wantArgs( osa.with( ouc.front() ) ) != Object::NotGood )
  {
    // the object wants the arg currently under the cursor...
    // draw mobc...
    mtype->drawPrelim( p, osa.with( ouc.front() ) );

    QString o = mtype->useText( osa, ouc.front() );

    mDoc->emitStatusBarText( o );
    p.drawTextStd( point , o );
    v->setCursor (KCursor::handCursor());
  }
  // see if mobc wants the point we're dragging around...
  else if ( mtype->wantArgs( osa.with( mp ) ) != Object::NotGood )
  {
    // it does...
    // so we first draw the point, then mobc, and then the text that
    // wantArg returns...
    p.drawObject( mp, true );
    mtype->drawPrelim( p, osa.with( mp ) );

    QString s = mtype->useText( osa, mp );
    mDoc->emitStatusBarText( s );

    // we draw the text a bit to the bottom so it doesn't end up on
    // top of the point...

    double i = v->pixelWidth();
    Coordinate d( i, i );
    p.drawTextStd( point, s );
    v->setCursor( KCursor::blankCursor() );
  }
  else
  {
    // mobc doesn't want either...
    // we just tell the user what he's constructing...
    QString o = i18n( "Constructing a %1" ).arg( i18n( mtype->baseTypeName() ) );
    mDoc->emitStatusBarText( o );
    p.drawTextStd( point , o );
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
  updatePoint( v->fromScreen( e->pos() ), v->screenInfo() );
  mDoc->addObject( mp );
  selectArg( mp, v );
}

void StdConstructionMode::selectArg( Object* o, KigView* v )
{
  if ( o == mp )
    // if we're selecting our mp, we need a new one...
    mp = mp->copy();
//   calcing is not necessary here, since
//  all of the data is copied along
//  mp->calc( v->screenInfo() );
  osa.push_back( o );
  int a = mtype->wantArgs( osa );
  assert( a != Object::NotGood );
  bool finished = ( a == Object::Complete );
  if( finished )
  {
    Object* o = mtype->build( osa, Type::ParamMap() );
    o->calc( v->screenInfo() );
    mDoc->addObject( o );
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

TextLabelConstructionMode::~TextLabelConstructionMode()
{
}

TextLabelConstructionMode::TextLabelConstructionMode( NormalMode* b,
                                                      KigDocument* d )
  : KigMode( d ), mprev( b )
{
}
void TextLabelConstructionMode::leftClicked( QMouseEvent* e, KigView* )
{
  mplc = e->pos();
}

void TextLabelConstructionMode::leftReleased( QMouseEvent* e, KigView* v )
{
  if ( ( mplc - e->pos() ).manhattanLength() > 4 ) return;
  Coordinate c = v->fromScreen( mplc );
  bool ok;
  QString text = KLineEditDlg::getText(
    i18n( "Choose Text" ), QString::fromUtf8(""), 0, &ok, v );
  if ( ! ok ) { killMode(); return; };
  mDoc->addObject( new TextLabel( text, c ) );
  v->redrawScreen();
  killMode();
  v->setCursor( KCursor::crossCursor() );
}

void TextLabelConstructionMode::killMode()
{
  NormalMode* p = mprev;
  KigDocument* d = mDoc;
  delete this;
  d->setMode( p );
}

void TextLabelConstructionMode::cancelConstruction()
{
  killMode();
}

void TextLabelConstructionMode::enableActions()
{
  KigMode::enableActions();

  mDoc->aCancelConstruction->setEnabled( true );
}

void TextLabelConstructionMode::mouseMoved( QMouseEvent*, KigView* v )
{
  v->setCursor( KCursor::crossCursor() );
}
