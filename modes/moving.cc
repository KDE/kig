// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "moving.h"

#include "normal.h"

#include "../objects/object_imp.h"
#include "../objects/object_factory.h"
#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../kig/kig_commands.h"
#include "../misc/kigpainter.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate_system.h"

#include <QMouseEvent>

#include <functional>
#include <algorithm>
#include <map>
#include <iterator>

void MovingModeBase::initScreen( const std::vector<ObjectCalcer*>& in )
{
  mcalcable = in;
  std::set<ObjectCalcer*> calcableset( mcalcable.begin(), mcalcable.end() );

  // don't try to move objects that have been deleted from the
  // document or internal objects that the user is not aware of..
  std::vector<ObjectHolder*> docobjs = mdoc.document().objects();
  for ( std::vector<ObjectHolder*>::iterator i = docobjs.begin();
        i != docobjs.end(); ++i )
    if ( calcableset.find( ( *i )->calcer() ) != calcableset.end() )
      mdrawable.push_back( *i );

  std::set<ObjectHolder*> docobjsset( docobjs.begin(), docobjs.end() );
  std::set<ObjectHolder*> drawableset( mdrawable.begin(), mdrawable.end() );
  std::set<ObjectHolder*> notmovingobjs;
  std::set_difference( docobjsset.begin(), docobjsset.end(), drawableset.begin(), drawableset.end(),
                       std::inserter( notmovingobjs, notmovingobjs.begin() ) );

  mview.clearStillPix();
  KigPainter p( mview.screenInfo(), &mview.stillPix, mdoc.document() );
  p.drawGrid( mdoc.document().coordinateSystem(), mdoc.document().grid(),
              mdoc.document().axes() );
  p.drawObjects( notmovingobjs.begin(), notmovingobjs.end(), false );
  mview.updateCurPix();

  KigPainter p2( mview.screenInfo(), &mview.curPix, mdoc.document() );
  p2.drawObjects( drawableset.begin(), drawableset.end(), true );
}

void MovingModeBase::leftReleased( QMouseEvent*, KigWidget* v )
{
  // clean up after ourselves:
  for ( std::vector<ObjectCalcer*>::iterator i = mcalcable.begin();
        i != mcalcable.end(); ++i )
    ( *i )->calc( mdoc.document() );
  stopMove();
  mdoc.setModified( true );

  // refresh the screen:
  v->redrawScreen( std::vector<ObjectHolder*>() );
  v->updateScrollBars();

  mdoc.doneMode( this );
}

void MovingModeBase::mouseMoved( QMouseEvent* e, KigWidget* v )
{
  v->updateCurPix();
  Coordinate c = v->fromScreen( e->pos() );

  bool snaptogrid = e->modifiers() & Qt::ShiftModifier;
  moveTo( c, snaptogrid );
  for ( std::vector<ObjectCalcer*>::iterator i = mcalcable.begin();
        i != mcalcable.end(); ++i )
    ( *i )->calc( mdoc.document() );
  KigPainter p( v->screenInfo(), &v->curPix, mdoc.document() );
  // TODO: only draw the explicitly moving objects as selected, the
  // other ones as deselected.. Needs some support from the
  // subclasses..
  p.drawObjects( mdrawable, true );
  v->updateWidget( p.overlay() );
  v->updateScrollBars();
}

class MovingMode::Private
{
public:
  // explicitly moving objects: these are the objects that the user
  // requested to move...
  std::vector<ObjectCalcer*> emo;
  // point where we started moving..
  Coordinate pwwsm;
  MonitorDataObjects* mon;
  // we keep a map from the emo objects to their reference location.
  // This is the location that they claim to be at before moving
  // starts, and we use it as a reference point to determine where
  // they should move next..
  std::map<const ObjectCalcer*, Coordinate> refmap;
};

MovingMode::MovingMode( const std::vector<ObjectHolder*>& os, const Coordinate& c,
                        KigWidget& v, KigPart& doc )
  : MovingModeBase( doc, v ), d( new Private )
{
  d->pwwsm = c;
  std::vector<ObjectCalcer*> emo;
  std::set<ObjectCalcer*> objs;
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( (*i)->canMove() )
    {
      emo.push_back( ( *i )->calcer() );
      d->refmap[( *i )->calcer()] = (*i)->moveReferencePoint();
      objs.insert( ( *i )->calcer() );
      std::vector<ObjectCalcer*> parents = ( *i )->calcer()->movableParents();
      objs.insert( parents.begin(), parents.end() );
    };

  emo = calcPath( emo );
  for ( std::vector<ObjectCalcer*>::const_iterator i = emo.begin(); i != emo.end(); ++i )
    if ( !isChild( *i, d->emo ) )
      d->emo.push_back( *i );

  d->mon = new MonitorDataObjects( std::vector<ObjectCalcer*>( objs.begin(),objs.end() ) );

  std::set<ObjectCalcer*> tmp = objs;
  for ( std::set<ObjectCalcer*>::const_iterator i = tmp.begin(); i != tmp.end(); ++i )
  {
    std::set<ObjectCalcer*> children = getAllChildren(*i);
    objs.insert( children.begin(), children.end() );
  }

  initScreen( calcPath( std::vector<ObjectCalcer*>( objs.begin(), objs.end() ) ) );
}

void MovingMode::stopMove()
{
  QString text = d->emo.size() == 1 ?
                 d->emo[0]->imp()->type()->moveAStatement() :
                 i18np( "Move %1 Object", "Move %1 Objects", d->emo.size() );
  KigCommand* mc = new KigCommand( mdoc, text );
  d->mon->finish( mc );
  mdoc.history()->push( mc );
}

void MovingMode::moveTo( const Coordinate& o, bool snaptogrid )
{
  for( std::vector<ObjectCalcer*>::iterator i = d->emo.begin(); i != d->emo.end(); ++i )
  {
    assert( d->refmap.find( *i ) != d->refmap.end() );
    Coordinate nc = d->refmap[*i] + ( o - d->pwwsm );
    if ( snaptogrid ) nc = mdoc.document().coordinateSystem().snapToGrid( nc, mview );
    (*i)->move( nc, mdoc.document() );
  };
}

PointRedefineMode::PointRedefineMode( ObjectHolder* p, KigPart& d, KigWidget& v )
  : MovingModeBase( d, v ), mp( p ), mmon( 0 )
{
  assert( dynamic_cast<ObjectTypeCalcer*>( p->calcer() ) );
  moldtype = static_cast<ObjectTypeCalcer*>( p->calcer() )->type();
  std::vector<ObjectCalcer*> oldparents = p->calcer()->parents();
  std::copy( oldparents.begin(), oldparents.end(), std::back_inserter( moldparents ) );

  std::vector<ObjectCalcer*> parents = getAllParents( mp->calcer() );
  mmon = new MonitorDataObjects( parents );
  std::vector<ObjectCalcer*> moving = parents;
  std::set<ObjectCalcer*> children = getAllChildren( mp->calcer() );
  std::copy( children.begin(), children.end(), std::back_inserter( moving ) );
  initScreen( moving );
}

void PointRedefineMode::moveTo( const Coordinate& o, bool snaptogrid )
{
  Coordinate realo =
    snaptogrid ? mdoc.document().coordinateSystem().snapToGrid( o, mview ) : o;
  ObjectFactory::instance()->redefinePoint(
    static_cast<ObjectTypeCalcer*>( mp->calcer() ), realo, mdoc.document(), mview );
}

PointRedefineMode::~PointRedefineMode()
{
}

MovingModeBase::MovingModeBase( KigPart& doc, KigWidget& v )
  : KigMode( doc ), mview( v )
{
}

MovingModeBase::~MovingModeBase()
{
}

void MovingModeBase::leftMouseMoved( QMouseEvent* e, KigWidget* v )
{
  mouseMoved( e, v );
}

MovingMode::~MovingMode()
{
  delete d->mon;
  delete d;
}

void PointRedefineMode::stopMove()
{
  assert( dynamic_cast<ObjectTypeCalcer*>( mp->calcer() ) );
  ObjectTypeCalcer* mpcalc = static_cast<ObjectTypeCalcer*>(  mp->calcer() );

  std::vector<ObjectCalcer*> newparents = mpcalc->parents();
  std::vector<ObjectCalcer::shared_ptr> newparentsref(
    newparents.begin(), newparents.end() );
  const ObjectType* newtype = mpcalc->type();

  std::vector<ObjectCalcer*> oldparents;
  for( std::vector<ObjectCalcer::shared_ptr>::iterator i = moldparents.begin();
       i != moldparents.end(); ++i )
    oldparents.push_back( i->get() );
  mpcalc->setType( moldtype );
  mpcalc->setParents( oldparents );
  mp->calc( mdoc.document() );

  KigCommand* command = new KigCommand( mdoc, i18n( "Redefine Point" ) );
  command->addTask(
    new ChangeParentsAndTypeTask( mpcalc, newparents, newtype ) );
  mmon->finish( command );
  mdoc.history()->push( command );
}
