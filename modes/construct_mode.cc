// construct_mode.cc
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#include "construct_mode.h"

#include "../objects/object_factory.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/object_constructor.h"
#include "../misc/kigpainter.h"

#include <kcursor.h>

ConstructMode::ConstructMode( KigDocument& d, const ObjectConstructor* ctor )
  : BaseMode( d ), mctor( ctor ),
    mpt( ObjectFactory::instance()->fixedPoint( Coordinate( 0, 0 ) ) )
{
}

ConstructMode::~ConstructMode()
{
}

void ConstructMode::leftClickedObject(
  Object* o, const QPoint& p, KigWidget& w, bool )
{
  ObjectFactory::instance()->redefinePoint( mpt, w.fromScreen( p ),
                                            mdoc, w );
  mpt->calc( w );
  if ( o && mctor->wantArgs( mparents.with( o ), mdoc, w ) )
  {
    selectObject( o, p, w );
  }
  else if ( mctor->wantArgs( mparents.with( mpt ), mdoc, w ) )
  {
    // add mpt to the document..
    Object* pt = mpt;
    mdoc.addObject( pt );
    // get a new mpt for our further use..
    mpt = ObjectFactory::instance()->sensiblePoint( w.fromScreen( p ),
                                                    mdoc, w );

    selectObject( pt, p, w );
  }
  else
  {
  };
}

void ConstructMode::midClicked( const QPoint& p, KigWidget& w )
{
  ObjectFactory::instance()->redefinePoint( mpt, w.fromScreen( p ),
                                            mdoc, w );
  if ( mctor->wantArgs( mparents.with( mpt ), mdoc, w ) )
  {
    mdoc.addObject( mpt );

    selectObject( mpt, p, w );

    mpt = ObjectFactory::instance()->fixedPoint( w.fromScreen( p ) );
  }
}

void ConstructMode::rightClicked( const Objects&, const QPoint&, KigWidget& )
{
  // TODO ?
}

void ConstructMode::mouseMoved( const Objects& os,
                                const QPoint& p,
                                KigWidget& w )
{
  w.updateCurPix();
  KigPainter pter( w.screenInfo(), &w.curPix );

  ObjectFactory::instance()->redefinePoint( mpt, w.fromScreen( p ),
                                            mdoc, w );
  mpt->calc( w );
  if ( !os.empty() && mctor->wantArgs( mparents.with( os.front() ), mdoc, w ) )
  {
    mctor->handlePrelim( pter, mparents.with( os.front() ), mdoc, w );
    w.setCursor( KCursor::handCursor() );
  }
  else if ( mctor->wantArgs( mparents.with( mpt ), mdoc, w ) )
  {
    mpt->draw( pter, true );
    mctor->handlePrelim( pter, mparents.with( mpt ), mdoc, w );
    w.setCursor( KCursor::handCursor() );
  }
  else
  {
    w.setCursor( KCursor::arrowCursor() );
  };
  w.updateWidget( pter.overlay() );
}

void ConstructMode::selectObject( Object* o, const QPoint&, KigWidget& w )
{
  mparents.push_back( o );
  o->setSelected( true );

  if ( mctor->wantArgs( mparents, mdoc, w ) == ArgsChecker::Complete )
  {
    mctor->handleArgs( mparents, mdoc, w );
    // finish off..
    for ( Objects::iterator i = mparents.begin(); i != mparents.end(); ++i )
      (*i)->setSelected( false );
    mdoc.doneMode( this );
  };

  w.redrawScreen();
}

PointConstructMode::PointConstructMode( KigDocument& d )
  : BaseMode( d ),
    mpt( ObjectFactory::instance()->fixedPoint( Coordinate() ) )
{
}

PointConstructMode::~PointConstructMode()
{
  delete mpt;
}

void PointConstructMode::leftClickedObject(
  Object*, const QPoint&, KigWidget& w, bool )
{
  mdoc.addObject( mpt );
  mpt = 0;
  w.redrawScreen();
  mdoc.doneMode( this );
}

void PointConstructMode::midClicked( const QPoint& p, KigWidget& w )
{
  leftClickedObject( 0, p, w, true );
}

void PointConstructMode::rightClicked( const Objects&, const QPoint&,
                                       KigWidget& )
{
  // TODO ?
}

void PointConstructMode::mouseMoved(
  const Objects&,
  const QPoint& p,
  KigWidget& w )
{
  w.updateCurPix();
  KigPainter pter( w.screenInfo(), &w.curPix );
  ObjectFactory::instance()->redefinePoint( mpt, w.fromScreen( p ),
                                            mdoc, w );
  mpt->calc( w );
  mpt->draw( pter, true );
  w.setCursor( KCursor::blankCursor() );

  w.updateWidget( pter.overlay() );
}
