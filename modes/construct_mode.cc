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
#include "../misc/kigpainter.h"

#include <kcursor.h>
#include <kaction.h>

ConstructMode::ConstructMode( KigDocument& d, const ObjectConstructor* ctor )
  : BaseMode( d ), mctor( ctor ),
    mpt( ObjectFactory::instance()->fixedPoint( Coordinate( 0, 0 ) ) )
{
}

ConstructMode::~ConstructMode()
{
  delete_all( mpt.begin(), mpt.end() );
  mpt.clear();
}

void ConstructMode::leftClickedObject(
  Object* o, const QPoint& p, KigWidget& w, bool )
{
  ObjectFactory::instance()->redefinePoint( mpt[2], w.fromScreen( p ),
                                            mdoc, w );
  mpt.calc( mdoc );
  if ( o && !mparents.contains( o ) && mctor->wantArgs( mparents.with( o ), mdoc, w ) )
  {
    selectObject( o, p, w );
  }
  else if ( mctor->wantArgs( mparents.with( mpt[2] ), mdoc, w ) )
  {
    // add mpt to the document..
    mdoc.addObjects( mpt );
    selectObject( mpt[2], p, w );
    // get a new mpt for our further use..
    mpt = ObjectFactory::instance()->sensiblePoint( w.fromScreen( p ),
                                                    mdoc, w );
  }
  else
  {
  };
}

void ConstructMode::midClicked( const QPoint& p, KigWidget& w )
{
  ObjectFactory::instance()->redefinePoint( mpt, w.fromScreen( p ),
                                            mdoc, w );
  if ( mctor->wantArgs( mparents.with( mpt[2] ), mdoc, w ) )
  {
    mdoc.addObjects( mpt );

    selectObject( mpt[2], p, w );

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
  KigPainter pter( w.screenInfo(), &w.curPix, mdoc );

  // set the text next to the arrow cursor like in modes/normal.cc
  QPoint textloc = p;
  textloc.setX( textloc.x() + 15 );

  ObjectFactory::instance()->redefinePoint( mpt[2], w.fromScreen( p ),
                                            mdoc, w );
  mpt.calc( mdoc );
  if ( !os.empty() && !mparents.contains( os.front() ) &&
       mctor->wantArgs( mparents.with( os.front() ), mdoc, w ) )
  {
    mctor->handlePrelim( pter, mparents.with( os.front() ), mdoc, w );

    QString o = mctor->useText( *os.front(), mparents, mdoc, w );
    mdoc.emitStatusBarText( o );
    pter.drawTextStd( textloc, o );

    w.setCursor( KCursor::handCursor() );
  }
  else if ( mctor->wantArgs( mparents.with( mpt[2] ), mdoc, w ) )
  {
    mpt[2]->draw( pter, true );
    mctor->handlePrelim( pter, mparents.with( mpt[2] ), mdoc, w );

    QString o = mctor->useText( *mpt[2], mparents, mdoc, w );
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

void ConstructMode::selectObject( Object* o, const QPoint&, KigWidget& w )
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
    mpt( ObjectFactory::instance()->fixedPoint( Coordinate() ) )
{
}

PointConstructMode::~PointConstructMode()
{
  delete_all( mpt.begin(), mpt.end() );
  mpt.clear();
}

void PointConstructMode::leftClickedObject(
  Object*, const QPoint&, KigWidget& w, bool )
{
  mdoc.addObjects( mpt );
  mpt.clear();
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
  KigPainter pter( w.screenInfo(), &w.curPix, mdoc );
  ObjectFactory::instance()->redefinePoint( mpt[2], w.fromScreen( p ),
                                            mdoc, w );
  mpt.calc( mdoc );
  mpt[2]->draw( pter, true );
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
