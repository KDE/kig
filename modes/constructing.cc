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

StdConstructionMode::StdConstructionMode( Object* obc, NormalMode* b,
                                          KigDocument* d )
  : KigMode( d ),
    mobc( obc ),
    mprev( b )
{
}

StdConstructionMode::~StdConstructionMode()
{
}

void StdConstructionMode::leftClicked( QMouseEvent* e, KigView* v )
{
  plc = e->pos();
  v->updateCurPix();
  oco = mDoc->whatAmIOn( v->fromScreen( e->pos() ), 3*v->pixelWidth() );
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
    // so we try if it wants a point..
    addPointRequest( v->fromScreen( e->pos() ), v );
  };
}

void StdConstructionMode::mouseMoved( QMouseEvent* e, KigView* v )
{
  // objects under cursor
  Objects ouc = mDoc->whatAmIOn( v->fromScreen( e->pos() ), 3*v->pixelWidth() );
  v->updateCurPix();
  KigPainter p( v->showingRect(), &v->curPix );
  mobc->drawPrelim( p, v->fromScreen( e->pos() ) );
  if ( ! ouc.empty() && mobc->wantArg( ouc.front() ) )
  {
    // TODO...
    // add a Object::drawPrelim( KigPainter&, Object* o ) where
    // it draws itself as if o had been selectArg'd...
    QString o = mobc->wantArg( ouc.front() );

    mDoc->emitStatusBarText( o );
    p.drawSimpleText( v->fromScreen( e->pos() ), o );
    v->setCursor (KCursor::handCursor());
  }
  else if( mobc->wantPoint() )
  {
    Coordinate c = v->fromScreen( e->pos() );
    QString s = mobc->wantPoint();
    mDoc->emitStatusBarText( s );
    double i = v->pixelWidth();
    Coordinate d( i, i );
    p.drawSimpleText( c + d*2, s );
    p.drawPoint( c, false );
    v->setCursor( KCursor::blankCursor() );
  }
  else
  {
    QString o = i18n( "Constructing a %1" ).arg( mobc->vTBaseTypeName() );
    mDoc->emitStatusBarText( o );
    p.drawSimpleText( v->fromScreen( e->pos() ), o );
    v->setCursor(KCursor::arrowCursor());
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
  addPointRequest( v->fromScreen( e->pos() ), v );
}

void StdConstructionMode::selectArg( Object* o, KigView* v )
{
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
  KigMode::enableActions();

  mDoc->aCancelConstruction->setEnabled( true );
}

void StdConstructionMode::cancelConstruction()
{
  delete mobc;
  NormalMode* p = mprev;
  KigDocument* d = mDoc;
  delete this;
  d->setMode( p );
}


/*
void ConstructingMode::midReleased( QMouseEvent* e, KigView* v )
{
  if( ( e->pos() - plc ).manhattanLength() > 3 ) return;
  Point* p;
  if( !oco.empty() && Object::toCurve( oco.front() ) )
    p = new ConstrainedPoint( Object::toCurve( oco.front() ), v->fromScreen( plc ) );
  else
    p = new FixedPoint( v->fromScreen( plc ) );
  mDoc->addObject( p );
  // we select the new point as an argument to the object we're
  // constructing...
  oco.clear();
  oco.append( p );
  this->leftReleased( e, v );
}

void ConstructingMode::rightClicked( QMouseEvent*, KigView* )
{

}

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

void ConstructingMode::mouseMoved( QMouseEvent*, KigView* )
{
};
*/
void StdConstructionMode::updateScreen( KigView* v )
{
  v->clearStillPix();
  KigPainter p( v->showingRect(), &v->stillPix );
  p.drawGrid( mDoc->coordinateSystem() );
  p.drawObjects( mDoc->objects() );
  v->updateCurPix( p.overlay() );
  v->updateWidget();
}

void StdConstructionMode::addPointRequest( const Coordinate& c, KigView* v )
{
  if ( mobc->wantPoint() )
  {
    Point* p = new NormalPoint( NormalPoint::NPImpForCoord( c, mDoc, 3*v->pixelWidth() ) );
    mDoc->addObject( p );
    selectArg( p, v );
  };
}
