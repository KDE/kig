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
#include "../objects/object_factory.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
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
  for ( uint i = 0; i < tin.size(); ++i )
    if ( tin[i]->inherits( Object::ID_RealObject ) )
      in.push_back( tin[i] );

  // here we calc what objects will be moving, and we draw the others
  // on KigWidget::stillPix..  nmo are the Not Moving Objects
  Objects nmo;

  using namespace std;
  amo = in;
  // calc nmo: basically ( mdoc->objects() - amo )
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
  KigPainter p( mview.screenInfo(), &mview.stillPix );
  p.drawGrid( mdoc.coordinateSystem() );
  p.drawObjects( nmo );
// not necessary
//   mview.drawObjects( amo, p );

  mview.updateCurPix();
}

void MovingModeBase::leftReleased( QMouseEvent*, KigWidget* v )
{
  // clean up after ourselves:
  amo.calc();
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
  amo.calc();
  KigPainter p( v->screenInfo(), &v->curPix );
  p.drawObjects( amo );
  v->updateWidget( p.overlay() );
  v->updateScrollBars();
};

MovingMode::MovingMode( const Objects& os, const Coordinate& c,
                        KigWidget& v, KigDocument& d )
  : MovingModeBase( d, v ), pwwlmt( c ), emo( os )
{
  // FIXME: fix this algorithm..., have objects tell us what other
  // objects they are going to move... e.g. only a segment moves its
  // parents, but right now, we have to take into account that every
  // object could move its parents...

  Objects objs( emo );
  for ( Objects::const_iterator i = emo.begin(); i != emo.end(); ++i )
  {
    objs |= (*i)->parents();
  };
  Objects tmp = objs;
  for ( Objects::const_iterator i = tmp.begin(); i != tmp.end(); ++i )
    objs |= (*i)->getAllChildren();

  initScreen( objs );
}

void MovingMode::stopMove()
{
}

void MovingMode::moveTo( const Coordinate& o )
{
  for( Objects::iterator i = emo.begin(); i != emo.end(); ++i )
    (*i)->move( pwwlmt, o - pwwlmt );
  pwwlmt = o;
}

PointRedefineMode::PointRedefineMode( RealObject* p, KigDocument& d, KigWidget& v )
  : MovingModeBase( d, v ), mp( p )
{
  using namespace std;
  Objects os( mp );
  Objects tmp = mp->getAllChildren();
  copy( tmp.begin(), tmp.end(), back_inserter( os ) );
  initScreen( os );
}

void PointRedefineMode::moveTo( const Coordinate& o )
{
  ObjectFactory::instance()->redefinePoint( mp, o, mdoc, mview );
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
}

void PointRedefineMode::stopMove()
{
}
