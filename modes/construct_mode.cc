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
#include "../objects/text_type.h"
#include "../objects/text_imp.h"
#include "../objects/bogus_imp.h"
#include "../objects/point_imp.h"
#include "../misc/argsparser.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/object_constructor.h"
#include "../misc/coordinate_system.h"
#include "../misc/kigpainter.h"
#include "../misc/calcpaths.h"

#include <kcursor.h>
#include <kaction.h>

#include <algorithm>
#include <functional>

static void redefinePoint( ObjectTypeCalcer* mpt, const Coordinate& c, KigDocument& doc, const KigWidget& w )
{
  ObjectFactory::instance()->redefinePoint( mpt, c, doc, w );
  mpt->calc( doc );
}

BaseConstructMode::BaseConstructMode( KigDocument& d )
  : BaseMode( d )
{
  mpt = ObjectFactory::instance()->fixedPointCalcer( Coordinate( 0, 0 ) );
  mpt->calc( d );
}

BaseConstructMode::~BaseConstructMode()
{
}

void BaseConstructMode::leftClickedObject(
  ObjectHolder* o, const QPoint& p, KigWidget& w, bool )
{
  bool alreadyselected = std::find( mparents.begin(), mparents.end(), o ) != mparents.end();
  std::vector<ObjectCalcer*> nargs = getCalcers( mparents );
  if ( o && !alreadyselected )
  {
    nargs.push_back( o->calcer() );
    if ( wantArgs( nargs, mdoc, w ) )
    {
      selectObject( o, w );
      return;
    }
  }

  nargs = getCalcers( mparents );
  nargs.push_back( mpt.get() );
  if ( wantArgs( nargs, mdoc, w ) )
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

void BaseConstructMode::midClicked( const QPoint& p, KigWidget& w )
{
  std::vector<ObjectCalcer*> args = getCalcers( mparents );
  args.push_back( mpt.get() );
  if ( wantArgs( args, mdoc, w ) )
  {
    ObjectHolder* n = new ObjectHolder( mpt.get() );
    mdoc.addObject( n );

    selectObject( n, w );

    mpt = ObjectFactory::instance()->sensiblePointCalcer( w.fromScreen( p ), mdoc, w );
    mpt->calc( mdoc );
  }
}

void BaseConstructMode::rightClicked( const std::vector<ObjectHolder*>&, const QPoint&, KigWidget& )
{
  // TODO ?
}

void BaseConstructMode::mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p,
                                KigWidget& w, bool shiftpressed )
{
  w.updateCurPix();
  KigPainter pter( w.screenInfo(), &w.curPix, mdoc );

  Coordinate ncoord = w.fromScreen( p );
  if ( shiftpressed )
    ncoord = mdoc.coordinateSystem().snapToGrid( ncoord, w );

  redefinePoint( mpt.get(), ncoord, mdoc, w );

  bool alreadyselected = false;
  std::vector<ObjectCalcer*> args = getCalcers( mparents );
  if ( ! os.empty() )
  {
    alreadyselected = std::find( mparents.begin(), mparents.end(), os.front() ) != mparents.end();
    if ( ! alreadyselected ) args.push_back( os.front()->calcer() );
  }
  if ( !os.empty() && ! alreadyselected && wantArgs( args, mdoc, w ) )
  {
    handlePrelim( args, p, pter, w );

    w.setCursor( KCursor::handCursor() );
  }
  else
  {
    std::vector<ObjectCalcer*> args = getCalcers( mparents );
    args.push_back( mpt.get() );
    if ( wantArgs( args, mdoc, w ) )
    {
      ObjectDrawer d;
      d.draw( *mpt->imp(), pter, true );

      handlePrelim( args, p, pter, w );

      w.setCursor( KCursor::handCursor() );
    }
    else
    {
      w.setCursor( KCursor::arrowCursor() );
    }
  }
  w.updateWidget( pter.overlay() );
}

void BaseConstructMode::selectObject( ObjectHolder* o, KigWidget& w )
{
  mparents.push_back( o );
  std::vector<ObjectCalcer*> args = getCalcers( mparents );

  if ( wantArgs( args, mdoc, w ) == ArgsParser::Complete )
  {
    handleArgs( args, w );
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

void BaseConstructMode::enableActions()
{
  BaseMode::enableActions();

  mdoc.aCancelConstruction->setEnabled( true );
}

void BaseConstructMode::cancelConstruction()
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

void BaseConstructMode::selectObjects( const std::vector<ObjectHolder*>& os, KigWidget& w )
{
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    std::vector<ObjectCalcer*> args = getCalcers( mparents );
    assert( wantArgs( args, mdoc, w ) != ArgsParser::Complete );
    selectObject( *i, w );
  };
}

void ConstructMode::handlePrelim( const std::vector<ObjectCalcer*>& args, const QPoint& p, KigPainter& pter, KigWidget& w )
{
  // set the text next to the arrow cursor like in modes/normal.cc
  QPoint textloc = p;
  textloc.setX( textloc.x() + 15 );

  mctor->handlePrelim( pter, args, mdoc, w );

  QString o = mctor->useText( *args.front(), args, mdoc, w );
  mdoc.emitStatusBarText( o );
  pter.drawTextStd( textloc, o );
}

int ConstructMode::wantArgs( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w )
{
  return mctor->wantArgs( os, d, w );
}

void BaseConstructMode::finish()
{
  mdoc.doneMode( this );
}

ConstructMode::ConstructMode( KigDocument& d, const ObjectConstructor* ctor )
  : BaseConstructMode( d ), mctor( ctor )
{
}

ConstructMode::~ConstructMode()
{
}

// does a test result have a frame by default ?
static const bool test_has_frame_dflt = true;

void TestConstructMode::handlePrelim( const std::vector<ObjectCalcer*>& os, const QPoint& p, KigPainter& pter, KigWidget& w )
{
  Args args;
  std::transform( os.begin(), os.end(), std::back_inserter( args ),
                  std::mem_fun( &ObjectCalcer::imp ) );

  QString usetext = i18n( mtype->argsParser().usetext( args.front(), args ) );
  ObjectImp* data = mtype->calc( args, mdoc );
  if ( ! data->valid() ) return;
  assert( data->inherits( TestResultImp::stype() ) );
  QString outputtext = static_cast<TestResultImp*>( data )->data();

  // usetext
  QPoint textloc = p;
  textloc.setX( textloc.x() + 15 );
  mdoc.emitStatusBarText( usetext );
  pter.drawTextStd( textloc, usetext );

  // test result
  TextImp ti( outputtext, w.fromScreen( p + QPoint( - 40, 30 ) ), test_has_frame_dflt );
  ti.draw( pter );

  delete data;
}

TestConstructMode::TestConstructMode( KigDocument& d, const ArgsParserObjectType* type )
  : BaseConstructMode( d ), mtype( type )
{
}

TestConstructMode::~TestConstructMode()
{
}

void ConstructMode::handleArgs( const std::vector<ObjectCalcer*>& args, KigWidget& w )
{
  mctor->handleArgs( args, mdoc, w );
  finish();
}

int TestConstructMode::wantArgs( const std::vector<ObjectCalcer*>& os, KigDocument&, KigWidget& )
{
  return mtype->argsParser().check( os );
}

void TestConstructMode::handleArgs( const std::vector<ObjectCalcer*>& args, KigWidget& )
{
  mresult = new ObjectTypeCalcer( mtype, args );
  mresult->calc( mdoc );
}

void TestConstructMode::leftClickedObject( ObjectHolder* o, const QPoint& p,
                                           KigWidget& w, bool ctrlOrShiftDown )
{
  if ( mresult ) {
    QPoint qloc = p + QPoint( -40, 0 );
    Coordinate loc = w.fromScreen( qloc );

    std::vector<ObjectCalcer*> parents;
    parents.push_back( new ObjectConstCalcer( new IntImp( test_has_frame_dflt ) ) );
    parents.push_back( new ObjectConstCalcer( new PointImp( loc ) ) );
    parents.push_back( new ObjectConstCalcer( new StringImp( QString::fromLatin1( "%1" ) ) ) );
    parents.push_back( mresult.get() );

    ObjectCalcer* ret = new ObjectTypeCalcer( TextType::instance(), parents );
    ret->calc( mdoc );
    mdoc.addObject( new ObjectHolder( ret ) );

    w.unsetCursor();

    finish();
  }
  else
    BaseConstructMode::leftClickedObject( o, p, w, ctrlOrShiftDown );
}

void TestConstructMode::midClicked( const QPoint& p, KigWidget& w )
{
  if ( mresult ) {
    // nothing to be done here, really
  }
  else
    BaseConstructMode::midClicked( p, w );
}

void TestConstructMode::rightClicked( const std::vector<ObjectHolder*>& oco, const QPoint& p, KigWidget& w )
{
  if ( mresult ) {
    // nothing to be done here, really
  }
  else
    BaseConstructMode::rightClicked( oco, p, w );
}

void TestConstructMode::mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w, bool shiftPressed )
{
  if ( mresult ) {
    w.setCursor( KCursor::blankCursor() );

    w.updateCurPix();
    KigPainter pter( w.screenInfo(), &w.curPix, mdoc );

    QPoint qloc = p + QPoint( -40, 0 );
    Coordinate loc = w.fromScreen( qloc );
    assert( dynamic_cast<const TestResultImp*>( mresult->imp() ) );
    TextImp ti( static_cast<const TestResultImp*>( mresult->imp() )->data(), loc, test_has_frame_dflt );
    ObjectDrawer d;
    d.draw( ti, pter, false );


    w.updateWidget( pter.overlay() );
  }
  else
    BaseConstructMode::mouseMoved( os, p, w, shiftPressed );
}
