// moving.cc
// Copyright (C)  2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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
#include "../objects/normalpoint.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/kigpainter.h"
#include "../misc/calcpaths.h"

#include <functional>
#include <algorithm>

MovingModeBase::MovingModeBase( NormalMode* prev, KigView* v, KigDocument* d )
  : KigMode( d ), mprev( prev ), mview( v )
{

}

MovingModeBase::~MovingModeBase()
{

}

void MovingModeBase::initScreen( Objects& in )
{
  amo = in;
  // calc nmo: basically ( mdoc->objects() - amo )
  // we use some stl magic here, tmp and tmp2 are set to os and
  // mdoc->objects(), sorted, and then used in std::set_difference...
  Objects tmp( amo.begin(), amo.end() );
  std::sort( tmp.begin(), tmp.end() );
  Objects tmp2( mDoc->objects().begin(), mDoc->objects().end() );
  std::sort( tmp2.begin(), tmp2.end() );
  std::set_difference( tmp2.begin(), tmp2.end(),
                       tmp.begin(), tmp.end(),
                       std::back_inserter( nmo ) );

  mview->clearStillPix();
  KigPainter p( mview->screenInfo(), &mview->stillPix );
  p.drawGrid( mDoc->coordinateSystem() );
  p.drawObjects( nmo );
// not necessary, i think.. speed is important in MovingMode...
//   mview->drawObjects( amo, p );

  mview->updateCurPix();
}

void MovingModeBase::leftMouseMoved( QMouseEvent* e, KigView* v )
{
  mouseMoved( e, v );
};

void MovingModeBase::leftReleased( QMouseEvent*, KigView* v )
{
  // clean up after ourselves:
  amo.calc( v->screenInfo() );
  stopMove();
  mDoc->setModified( true );

  // refresh the screen:
  v->redrawScreen();

  // get rid of ourselves:
  KigMode* prev = mprev;
  KigDocument* doc = mDoc;
  delete this;
  doc->setMode( prev );
}

void MovingModeBase::mouseMoved( QMouseEvent* e, KigView* v )
{
  v->updateCurPix();
  Coordinate c = v->fromScreen( e->pos() );
  moveTo( c );
  amo.calc( v->screenInfo() );
  KigPainter p( v->screenInfo(), &v->curPix );
  p.drawObjects( amo );
  v->updateWidget( p.overlay() );
};

MovingMode::MovingMode( const Objects& os, const Coordinate& c,
                        NormalMode* previousMode, KigView* v,
                        KigDocument* d )
  : MovingModeBase( previousMode, v, d ), emo( os )
{
  // FIXME: fix this algorithm..., have objects tell us what other
  // objects they are going to move... e.g. only a segment moves its
  // parents, but right now, we have to take into account that every
  // object could move its parents...
  for ( Objects::iterator i = emo.begin(); i != emo.end(); ++i )
    (*i)->startMove( c );

  Objects objs;
  Objects tmp( emo ), tmp2;
  while (!tmp.empty())
  {
    tmp2.clear();
    // sos and their children/parents and their children's
    // children...: these will be changing
    for (Objects::iterator i = tmp.begin(); i != tmp.end(); ++i )
    {
      if (!objs.contains(*i))
      {
        objs.push_back(*i);
        tmp2 |= (*i)->getChildren();
        Objects tmp3 = (*i)->getParents();
        tmp2 |= tmp3;
      };
    };
    tmp = tmp2;
  };
  initScreen( objs );
}

MovingMode::~MovingMode()
{
}

void MovingMode::stopMove()
{
  std::for_each( emo.begin(), emo.end(), std::mem_fun( &Object::stopMove ) );
}

void MovingMode::moveTo( const Coordinate& o )
{
  for( Objects::iterator i = emo.begin(); i != emo.end(); ++i )
    (*i)->moveTo( o );
}

NormalPointRedefineMode::NormalPointRedefineMode( NormalPoint* p, KigDocument* d, KigView* v, NormalMode* m )
  : MovingModeBase( m, v, d ), mp( p )
{

  Objects os( mp );
  Objects tmp = mp->getAllChildren();
  std::copy( tmp.begin(), tmp.end(), std::back_inserter( os ) );
  kdDebug() << k_funcinfo << os.size() << endl;
  initScreen( os );
}

void NormalPointRedefineMode::stopMove()
{
}

void NormalPointRedefineMode::moveTo( const Coordinate& o )
{
  mp->redefine( o, *mDoc, 3*mview->pixelWidth() );
}

NormalPointRedefineMode::~NormalPointRedefineMode()
{

}
