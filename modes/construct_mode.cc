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

#include "../objects/object.h"
#include "../objects/object_factory.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/object_constructor.h"
#include "../misc/coordinate_system.h"
#include "../misc/kigpainter.h"
#include "../misc/calcpaths.h"

#include <kcursor.h>
#include <kaction.h>

static void redefinePoint( Object* mpt, const Coordinate& c, KigDocument& doc, const KigWidget& w )
{
  ObjectFactory::instance()->redefinePoint( mpt, c, doc, w );
  mpt->calc( doc );
};

ConstructMode::ConstructMode( KigDocument& d, const ObjectConstructor* ctor )
  : BaseMode( d ), mctor( ctor ),
    mpt( 0 )
{
  mpt = ObjectFactory::instance()->fixedPoint( Coordinate( 0, 0 ) );
  mpt->calc( d );
}

ConstructMode::~ConstructMode()
{
  delete mpt;
}

void ConstructMode::leftClickedObject(
  Object* o, const QPoint& p, KigWidget& w, bool )
{
  if ( o && !mparents.contains( o ) && mctor->wantArgs( mparents.with( o ), mdoc, w ) )
  {
    selectObject( o, w );
  }
  else if ( mctor->wantArgs( mparents.with( mpt ), mdoc, w ) )
  {
    // add mpt to the document..
    mdoc.addObject( mpt );
    selectObject( mpt, w );
    // get a new mpt for our further use..
    mpt = ObjectFactory::instance()->sensiblePoint( w.fromScreen( p ),
                                                             mdoc, w );
    mpt->calc( mdoc );
  }
  else
  {
  };
}

void ConstructMode::midClicked( const QPoint& p, KigWidget& w )
{
  if ( mctor->wantArgs( mparents.with( mpt ), mdoc, w ) )
  {
    mdoc.addObject( mpt );

    selectObject( mpt, w );

    mpt = ObjectFactory::instance()->sensiblePoint( w.fromScreen( p ), mdoc, w );
    mpt->calc( mdoc );
  }
}

void ConstructMode::rightClicked( const Objects&, const QPoint&, KigWidget& )
{
  // TODO ?
}

void ConstructMode::mouseMoved( const Objects& os,
                                const QPoint& p,
                                KigWidget& w,
                                bool shiftpressed )
{
  w.updateCurPix();
  KigPainter pter( w.screenInfo(), &w.curPix, mdoc );

  // set the text next to the arrow cursor like in modes/normal.cc
  QPoint textloc = p;
  textloc.setX( textloc.x() + 15 );

  Coordinate ncoord = w.fromScreen( p );
  if ( shiftpressed )
    ncoord = mdoc.coordinateSystem().snapToGrid( ncoord, w );

  redefinePoint( mpt, ncoord, mdoc, w );

  if ( !os.empty() && !mparents.contains( os.front() ) &&
       mctor->wantArgs( mparents.with( os.front() ), mdoc, w ) )
  {
    mctor->handlePrelim( pter, mparents.with( os.front() ), mdoc, w );

    QString o = mctor->useText( *os.front(), mparents, mdoc, w );
    mdoc.emitStatusBarText( o );
    pter.drawTextStd( textloc, o );

    w.setCursor( KCursor::handCursor() );
  }
  else if ( mctor->wantArgs( mparents.with( mpt ), mdoc, w ) )
  {
    mpt->draw( pter, true );
    mctor->handlePrelim( pter, mparents.with( mpt ), mdoc, w );

    QString o = mctor->useText( *mpt, mparents, mdoc, w );
    mdoc.emitStatusBarText( o );
    pter.drawTextStd( textloc, o );

    w.setCursor( KCursor::handCursor() );
  }
  else
  {
    w.setCursor( KCursor::arrowCursor() );
  };
  w.updateWidget( pter.overlay() );
}

void ConstructMode::selectObject( Object* o, KigWidget& w )
{
  mparents.push_back( o );
  o->setSelected( true );

  if ( mctor->wantArgs( mparents, mdoc, w ) == ArgsChecker::Complete )
  {
    mctor->handleArgs( mparents, mdoc, w );
    // finish off..
    for ( Objects::iterator i = mparents.begin(); i != mparents.end(); ++i )
    {
      assert( (*i)->inherits( Object::ID_RealObject ) );
      static_cast<RealObject*>( *i )->setSelected( false );
    }
    mdoc.doneMode( this );
  };

  w.redrawScreen();
}

PointConstructMode::PointConstructMode( KigDocument& d )
  : BaseMode( d ),
    mpt( 0 )
{
  // we add the data objects to the document cause
  // ObjectFactory::redefinePoint does that too, and this way, we can
  // depend on them already being known by the doc when we add the
  // mpt..
  mpt = ObjectFactory::instance()->fixedPoint( Coordinate() );
  mpt->calc( d );
}

PointConstructMode::~PointConstructMode()
{
  // delete mpt and its obsolete parents..
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
  KigWidget& w,
  bool shiftpressed )
{
  w.updateCurPix();
  KigPainter pter( w.screenInfo(), &w.curPix, mdoc );

  Coordinate ncoord = w.fromScreen( p );
  if ( shiftpressed )
    ncoord = mdoc.coordinateSystem().snapToGrid( ncoord, w );

  redefinePoint( mpt, ncoord, mdoc, w );

  mpt->draw( pter, true );
  w.setCursor( KCursor::blankCursor() );

  w.updateWidget( pter.overlay() );
}

void ConstructMode::enableActions()
{
  BaseMode::enableActions();

  mdoc.aCancelConstruction->setEnabled( true );
}

void ConstructMode::cancelConstruction()
{
  mdoc.doneMode( this );
}

void PointConstructMode::enableActions()
{
  BaseMode::enableActions();

  mdoc.aCancelConstruction->setEnabled( true );
}

void PointConstructMode::cancelConstruction()
{
  mdoc.doneMode( this );
}

void ConstructMode::selectObjects( const Objects& os, KigWidget& w )
{
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( mctor->wantArgs( mparents, mdoc, w ) != ArgsChecker::Complete );
    selectObject( *i, w );
  };
}
