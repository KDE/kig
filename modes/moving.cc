// moving.cc
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

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
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/kigpainter.h"

#include <functional>
#include <algorithm>

MovingMode::MovingMode( const Objects& os, const Coordinate& c,
                        NormalMode* previousMode, KigView* v,
                        KigDocument* d )
  : KigMode( d ), emo( os ), mPrevious( previousMode ), mView( v )
{
  // FIXME: fix this algorithm..., have objects tell us what other
  // objects they are going to move... e.g. only a segment moves its
  // parents, but right now, we have to take into account that every
  // object could move its parents...
  for ( Objects::iterator i = emo.begin(); i != emo.end(); ++i )
    (*i)->startMove( c );
  kdDebug() << k_funcinfo << c << endl;
  amo.clear();
  nmo.clear();
  Objects tmp( emo ), tmp2;
  while (!tmp.empty())
  {
    tmp2.clear();
    // sos and their children/parents and their children's
    // children...: these will be changing
    for (Objects::iterator i = tmp.begin(); i != tmp.end(); ++i )
    {
      if (!amo.contains(*i))
      {
        amo.push_back(*i);
        tmp2 |= (*i)->getChildren();
        Objects tmp3 = (*i)->getParents();
        tmp2 |= tmp3;
      };
    };
    tmp = tmp2;
  };
  // all the rest is still
  for ( Objects::const_iterator i = mDoc->objects().begin();
        i != mDoc->objects().end(); ++i )
    if (!amo.contains( *i ) )
      nmo.push_back(*i);
  v->clearStillPix();
  {
    KigPainter p( v->screenInfo(), &v->stillPix );
    p.drawGrid( mDoc->coordinateSystem() );
    p.drawObjects( nmo );
  }
  v->updateCurPix();
  // not necessary, i think.. speed is important in MovingMode...
//   {
//     KigPainter p( v, &v->curPix );
//     v->drawObjects( amo, p );
//     v->updateWidget( p.overlay() );
//   };
}

MovingMode::~MovingMode()
{
}

void MovingMode::leftMouseMoved( QMouseEvent* e, KigView* v )
{
  mouseMoved( e, v );
};

void MovingMode::leftReleased( QMouseEvent*, KigView* v )
{
  // clean up after ourselves:
  calc( amo, v->screenInfo() );
  std::for_each( emo.begin(), emo.end(), std::mem_fun( &Object::stopMove ) );
  mDoc->setModified( true );
  // refresh the screen:
  v->clearStillPix();
  KigPainter p( v->screenInfo(), &v->stillPix );
  p.drawGrid( mDoc->coordinateSystem() );
  p.drawObjects( mDoc->objects() );
  v->updateCurPix();
  // unnecessary ?
//  v->updateEntireWidget();
  // get rid of ourselves:
  KigMode* prev = mPrevious;
  KigDocument* doc = mDoc;
  delete this;
  doc->setMode( prev );
}

void MovingMode::mouseMoved( QMouseEvent* e, KigView* v )
{
  v->updateCurPix();
  Coordinate c = v->fromScreen( e->pos() );
  for( Objects::iterator i = emo.begin(); i != emo.end(); ++i )
    (*i)->moveTo( c );
  calc( amo, v->screenInfo() );
  KigPainter p( v->screenInfo(), &v->curPix );
  p.drawObjects( amo );
  v->updateWidget( p.overlay() );
};
