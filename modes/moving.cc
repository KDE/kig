// moving.cc
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

#include "moving.h"

#include "normal.h"

#include "../objects/object.h"
#include "../objects/object_imp.h"
#include "../objects/object_factory.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../kig/kig_commands.h"
#include "../misc/kigpainter.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate_system.h"

#include <functional>
#include <algorithm>
#include <map>

void MovingModeBase::initScreen( const Objects& tin )
{
  // don't try to move objects that have been deleted from the
  // document..
  Objects in;
  Objects docobjs = mdoc.allObjects();
  for ( uint i = 0; i < tin.size(); ++i )
    if ( docobjs.contains( tin[i] ) )
      in.push_back( tin[i] );

  // here we calc what objects will be moving, and we draw the others
  // on KigWidget::stillPix..  nmo are the Not Moving Objects
  Objects nmo;

  using namespace std;
  amo = in;
  // calc nmo: basically ( mdoc.objects() - amo )
  // we use some stl magic here, tmp and tmp2 are set to os and
  // mdoc.objects(), sorted, and then used in set_difference...
  Objects tmp( amo.begin(), amo.end() );
  sort( tmp.begin(), tmp.end() );
  Objects tmp2( mdoc.objects() );
  sort( tmp2.begin(), tmp2.end() );
  set_difference( tmp2.begin(), tmp2.end(),
                  tmp.begin(), tmp.end(),
                  back_inserter( nmo ) );

  mview.clearStillPix();
  KigPainter p( mview.screenInfo(), &mview.stillPix, mdoc );
  p.drawGrid( mdoc.coordinateSystem() );
  p.drawObjects( nmo );
  mview.updateCurPix();

  KigPainter p2( mview.screenInfo(), &mview.curPix, mdoc );
  p2.drawObjects( amo );
}

void MovingModeBase::leftReleased( QMouseEvent*, KigWidget* v )
{
  // clean up after ourselves:
  amo.calc( mdoc );
  stopMove();
  mdoc.setModified( true );

  // refresh the screen:
  v->redrawScreen();
  v->updateScrollBars();

  mdoc.doneMode( this );
}

void MovingModeBase::mouseMoved( QMouseEvent* e, KigWidget* v )
{
  v->updateCurPix();
  Coordinate c = v->fromScreen( e->pos() );


  bool snaptogrid = e->state() & Qt::ShiftButton;
  moveTo( c, snaptogrid );
  amo.calc( mdoc );
  KigPainter p( v->screenInfo(), &v->curPix, mdoc );
  p.drawObjects( amo );
  v->updateWidget( p.overlay() );
  v->updateScrollBars();
};

class MovingMode::Private
{
public:
  // explicitly moving objects: these are the objects that the user
  // requested to move...
  Objects emo;
  // point where we started moving..
  Coordinate pwwsm;
  MonitorDataObjects* mon;
  // we keep a map from the emo objects to their reference location.
  // This is the location that they claim to be at before moving
  // starts, and we use it as a reference point to determine where
  // they should move next..
  std::map<const Object*, Coordinate> refmap;
};

MovingMode::MovingMode( const Objects& os, const Coordinate& c,
                        KigWidget& v, KigDocument& doc )
  : MovingModeBase( doc, v ), d( new Private )
{
  d->pwwsm = c;
  Objects objs;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( (*i)->canMove() )
    {
      d->emo.upush( *i );
      d->refmap[*i] = (*i)->moveReferencePoint();
      objs.upush( *i );
      objs |= getAllParents( Objects( *i ) );
    };

  d->mon = new MonitorDataObjects( objs );

  Objects tmp = objs;
  for ( Objects::const_iterator i = tmp.begin(); i != tmp.end(); ++i )
    objs |= (*i)->getAllChildren();

  initScreen( objs );
}

void MovingMode::stopMove()
{
  QString text = d->emo.size() == 1 ?
                 d->emo[0]->imp()->type()->moveAStatement() :
                 i18n( "Move %1 Objects" ).arg( d->emo.size() );
  KigCommand* mc = new KigCommand( mdoc, text );
  mc->addTask( d->mon->finish() );
  mdoc.history()->addCommand( mc );
}

void MovingMode::moveTo( const Coordinate& o, bool snaptogrid )
{
  for( Objects::iterator i = d->emo.begin(); i != d->emo.end(); ++i )
  {
    assert( d->refmap.find( *i ) != d->refmap.end() );
    Coordinate nc = d->refmap[*i] + ( o - d->pwwsm );
    if ( snaptogrid ) nc = mdoc.coordinateSystem().snapToGrid( nc, mview );
    (*i)->move( nc, mdoc );
  };
}

PointRedefineMode::PointRedefineMode( RealObject* p, KigDocument& d, KigWidget& v )
  : MovingModeBase( d, v ), mp( p ), moldparents( p->parents() ), moldtype( p->type() ),
    mref( new ReferenceObject( moldparents ) ), mmon( 0 )
{
  using namespace std;
  Objects os( mp );
  Objects tmp = mp->getAllChildren();
  mmon = new MonitorDataObjects( getAllParents( os ) );
  copy( tmp.begin(), tmp.end(), back_inserter( os ) );
  initScreen( os );
}

void PointRedefineMode::moveTo( const Coordinate& o, bool snaptogrid )
{
  Coordinate realo =
    snaptogrid ? mdoc.coordinateSystem().snapToGrid( o, mview ) : o;
  (void) ObjectFactory::instance()->redefinePoint( mp, realo, mdoc, mview );
}

PointRedefineMode::~PointRedefineMode()
{
}

MovingModeBase::MovingModeBase( KigDocument& doc, KigWidget& v )
  : KigMode( doc ), mview( v )
{
}

MovingModeBase::~MovingModeBase()
{
}

void MovingModeBase::leftMouseMoved( QMouseEvent* e, KigWidget* v )
{
  mouseMoved( e, v );
};

MovingMode::~MovingMode()
{
  delete d->mon;
  delete d;
}

void PointRedefineMode::stopMove()
{
  Objects parents = mp->parents();
  const ObjectType* type = mp->type();
  mp->setType( moldtype );
  mp->setParents( moldparents );
  mp->calc( mdoc );

  // mref was used to keep an artificial reference to the objects in
  // moldparents.. we don't want it anymore after this..
  delete mref;

  KigCommand* command = new KigCommand( mdoc, i18n( "Redefine Point" ) );
  command->addTask(
    new ChangeParentsAndTypeTask( mp, parents, type ) );
  command->addTask( mmon->finish() );
  mdoc.history()->addCommand( command );
}
