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

void MovingModeBase::initScreen( const Objects& tin )
{
  // temporary fix before i finally fix the moving objects selection
  // algorithm: don't try to move DataObjects..
  Objects in;
  Objects docobjs = mdoc.objects();
  for ( uint i = 0; i < tin.size(); ++i )
    if ( docobjs.contains( tin[i] ) )
         // don't try to move objects that have been
         // deleted from the document..
      in.push_back( tin[i] );

  // here we calc what objects will be moving, and we draw the others
  // on KigWidget::stillPix..  nmo are the Not Moving Objects
  Objects nmo;

  using namespace std;
  amo = in;
  // calc nmo: basically ( mdoc.objects() - amo )
  // we use some stl magic here, tmp and tmp2 are set to os and
  // mdoc->objects(), sorted, and then used in set_difference...
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
  moveTo( c );
  amo.calc( mdoc );
  KigPainter p( v->screenInfo(), &v->curPix, mdoc );
  p.drawObjects( amo );
  v->updateWidget( p.overlay() );
  v->updateScrollBars();
};

MovingMode::MovingMode( const Objects& os, const Coordinate& c,
                        KigWidget& v, KigDocument& doc )
  : MovingModeBase( doc, v ), pwwlmt( c ),
    mon( 0 )
{
  Objects objs;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( (*i)->canMove() )
    {
      emo.upush( *i );
      objs.upush( *i );
      objs |= getAllParents( Objects( *i ) );
    };

  mon = new MonitorDataObjects( objs );

  Objects tmp = objs;
  for ( Objects::const_iterator i = tmp.begin(); i != tmp.end(); ++i )
    objs |= (*i)->getAllChildren();

  initScreen( objs );
}

void MovingMode::stopMove()
{
  QString text = emo.size() == 1 ?
                 ObjectImp::moveAStatement( emo[0]->imp()->id() ) :
                 i18n( "Move %1 Objects" ).arg( emo.size() );
  KigCommand* mc = new KigCommand( mdoc, text );
  mc->addTask( mon->finish() );
  mdoc.history()->addCommand( mc );
}

void MovingMode::moveTo( const Coordinate& o )
{
  for( Objects::iterator i = emo.begin(); i != emo.end(); ++i )
    (*i)->move( pwwlmt, o - pwwlmt, mdoc );
  pwwlmt = o;
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

void PointRedefineMode::moveTo( const Coordinate& o )
{
  (void) ObjectFactory::instance()->redefinePoint( mp, o, mdoc, mview );
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
  delete mon;
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
