// moving.h
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

#ifndef MOVING_H
#define MOVING_H

#include "mode.h"

#include "../misc/objects.h"

class Coordinate;
class NormalMode;
class KigView;
class KigDocument;

/**
 * i have thought about whether this should be a separate
 * mode, and i think it should.  Thus, every other mode can
 * decide to start moving some objects.  You could argue that it
 * doesn't handle much events, but ignoring events is a way of
 * handling them too...
 */
class MovingMode
  : public KigMode
{
public:
  MovingMode( const Objects& objects, const Coordinate& c,
	      NormalMode* previousMode, KigView*, KigDocument* );
  ~MovingMode();
  void leftReleased( QMouseEvent*, KigView* );
  void leftMouseMoved( QMouseEvent*, KigView* );
  void mouseMoved( QMouseEvent*, KigView* );
protected:
  // explicitly moving objects: these are the objects that the user
  // requested to move...
  Objects emo;
  // all moving objects: these objects are the emo, along with the
  // objects that depend upon them...
  Objects amo;
  // these are the objects that are not moving at all..
  Objects nmo;
  NormalMode* mPrevious;

  KigView* mView;
};

#endif
