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
#include "../objects/object_drawer.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/object_constructor.h"
#include "../misc/coordinate_system.h"
#include "../misc/kigpainter.h"
#include "../misc/calcpaths.h"

#include <kcursor.h>
#include <kaction.h>

#include <algorithm>

static void redefinePoint( ObjectTypeCalcer* mpt, const Coordinate& c, KigDocument& doc, const KigWidget& w )
{
  ObjectFactory::instance()->redefinePoint( mpt, c, doc, w );
  mpt->calc( doc );
}

ConstructMode::ConstructMode( KigDocument& d, const ObjectConstructor* ctor )
  : BaseMode( d ), mctor( ctor )
{
  mpt = ObjectFactory::instance()->fixedPointCalcer( Coordinate( 0, 0 ) );
  mpt->calc( d );
}

ConstructMode::~ConstructMode()
{
}

void ConstructMode::leftClickedObject(
  ObjectHolder* o, const QPoint& p, KigWidget& w, bool )
{
  bool alreadyselected = std::find( mparents.begin(), mparents.end(), o ) != mparents.end();
  std::vector<ObjectCalcer*> nargs = getCalcers( mparents );
  if ( o && !alreadyselected )
  {
    nargs.push_back( o->calcer() );
    if ( mctor->wantArgs( nargs, mdoc, w ) )
    {
      selectObject( o, w );
      return;
    }
  }

  nargs = getCalcers( mparents );
  nargs.push_back( mpt.get() );
  if ( mctor->wantArgs( nargs, mdoc, w ) )
  {
    // add mpt to the document..
    ObjectHolder* n = new ObjectHolder( mpt.get() );
    mdoc.addObject( n );
    selectObject( n, w );
    // get a new mpt for our further use..
    mpt = ObjectFactory::instance()->sensiblePointCalcer( w.fromScreen( p ), mdoc, w );
    mpt->calc( mdoc );
  }
}

void ConstructMode::midClicked( const QPoint& p, KigWidget& w )
{
  std::vector<ObjectCalcer*> args = getCalcers( mparents );
  args.push_back( mpt.get() );
  if ( mctor->wantArgs( args, mdoc, w ) )
  {
    ObjectHolder* n = new ObjectHolder( mpt.get() );
    mdoc.addObject( n );

    selectObject( n, w );

    mpt = ObjectFactory::instance()->sensiblePointCalcer( w.fromScreen( p ), mdoc, w );
    mpt->calc( mdoc );
  }
}

void ConstructMode::rightClicked( const std::vector<ObjectHolder*>&, const QPoint&, KigWidget& )
{
  // TODO ?
}

void ConstructMode::mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p,
                                KigWidget& w, bool shiftpressed )
{
  w.updateCurPix();
  KigPainter pter( w.screenInfo(), &w.curPix, mdoc );

  // set the text next to the arrow cursor like in modes/normal.cc
  QPoint textloc = p;
  textloc.setX( textloc.x() + 15 );

  Coordinate ncoord = w.fromScreen( p );
  if ( shiftpressed )
    ncoord = mdoc.coordinateSystem().snapToGrid( ncoord, w );

  redefinePoint( mpt.get(), ncoord, mdoc, w );

  bool alreadyselected = false;
  std::vector<ObjectCalcer*> args = getCalcers( mparents );
  if ( ! os.empty() )
  {
    alreadyselected = std::find( mparents.begin(), mparents.end(), os.front() ) != mparents.end();
    args.push_back( os.front()->calcer() );
  }
  if ( !os.empty() && ! alreadyselected && mctor->wantArgs( args, mdoc, w ) )
  {
    mctor->handlePrelim( pter, args, mdoc, w );

    QString o = mctor->useText( *os.front()->calcer(), args, mdoc, w );
    mdoc.emitStatusBarText( o );
    pter.drawTextStd( textloc, o );

    w.setCursor( KCursor::handCursor() );
  }
  else
  {
    std::vector<ObjectCalcer*> args = getCalcers( mparents );
    args.push_back( mpt.get() );
    if ( mctor->wantArgs( args, mdoc, w ) )
    {
      ObjectDrawer d;
      d.draw( *mpt->imp(), pter, true );
      mctor->handlePrelim( pter, args, mdoc, w );

      QString o = mctor->useText( *mpt, args, mdoc, w );
      mdoc.emitStatusBarText( o );
      pter.drawTextStd( textloc, o );

      w.setCursor( KCursor::handCursor() );
    }
    else
    {
      w.setCursor( KCursor::arrowCursor() );
    }
  }
  w.updateWidget( pter.overlay() );
}

void ConstructMode::selectObject( ObjectHolder* o, KigWidget& w )
{
  mparents.push_back( o );
  std::vector<ObjectCalcer*> args = getCalcers( mparents );

  if ( mctor->wantArgs( args, mdoc, w ) == ArgsParser::Complete )
  {
    mctor->handleArgs( args, mdoc, w );
    // finish off..
    finish();
  };

  w.redrawScreen( mparents );
}

PointConstructMode::PointConstructMode( KigDocument& d )
  : BaseMode( d )
{
  // we add the data objects to the document cause
  // ObjectFactory::redefinePoint does that too, and this way, we can
  // depend on them already being known by the doc when we add the
  // mpt..
  mpt = ObjectFactory::instance()->fixedPointCalcer( Coordinate() );
  mpt->calc( d );
}

PointConstructMode::~PointConstructMode()
{
}

void PointConstructMode::leftClickedObject(
  ObjectHolder*, const QPoint&, KigWidget& w, bool )
{
  mdoc.addObject( new ObjectHolder( mpt.get() ) );
  w.redrawScreen( std::vector<ObjectHolder*>() );
  mdoc.doneMode( this );
}

void PointConstructMode::midClicked( const QPoint& p, KigWidget& w )
{
  leftClickedObject( 0, p, w, true );
}

void PointConstructMode::rightClicked( const std::vector<ObjectHolder*>&, const QPoint&,
                                       KigWidget& )
{
  // TODO ?
}

void PointConstructMode::mouseMoved(
  const std::vector<ObjectHolder*>&,
  const QPoint& p,
  KigWidget& w,
  bool shiftpressed )
{
  w.updateCurPix();
  KigPainter pter( w.screenInfo(), &w.curPix, mdoc );

  Coordinate ncoord = w.fromScreen( p );
  if ( shiftpressed )
    ncoord = mdoc.coordinateSystem().snapToGrid( ncoord, w );

  redefinePoint( mpt.get(), ncoord, mdoc, w );

  ObjectDrawer d;
  d.draw( *mpt->imp(), pter, true );
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
  finish();
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

void ConstructMode::selectObjects( const std::vector<ObjectHolder*>& os, KigWidget& w )
{
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    std::vector<ObjectCalcer*> args = getCalcers( mparents );
    assert( mctor->wantArgs( args, mdoc, w ) != ArgsParser::Complete );
    selectObject( *i, w );
  };
}

void ConstructMode::finish()
{
  for ( Objects::iterator i = mparents.begin(); i != mparents.end(); ++i )
  {
    assert( (*i)->inherits( Object::ID_RealObject ) );
    static_cast<RealObject*>( *i )->setSelected( false );
  }
  mdoc.doneMode( this );
}
