// moving.h
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

#ifndef MOVING_H
#define MOVING_H

#include "mode.h"

#include "../misc/objects.h"

class Coordinate;
class NormalMode;
class NormalPoint;
class KigWidget;
class KigDocument;

/**
 * "Template method" pattern ( see the Design patterns book ):
 * This is a base class for two modes: normal MovingMode: used for
 * moving a set of objects around, using Object::startMove,
 * Object::moveTo and Object::stopMove, and another mode
 * NormalPointRedefineMode, used for redefining a NormalPoint...
 */
class MovingModeBase
  : public KigMode
{
protected:
  MovingModeBase( NormalMode* prev, KigWidget* v, KigDocument* d );
  ~MovingModeBase();
  // Subclasses should call this in their constructor, when they know
  // which objects will be moving around... They are expected to be in
  // the right order for being calc()'ed...
  void initScreen( Objects& amo );
  // in these functions, subclasses should do the equivalent of
  // Object::stopMove() and moveTo()...  Note that no calc()'ing or
  // drawing is to be done..
  virtual void stopMove() = 0;
  virtual void moveTo( const Coordinate& o ) = 0;
public:
  void leftReleased( QMouseEvent*, KigWidget* );
  void leftMouseMoved( QMouseEvent*, KigWidget* );
  void mouseMoved( QMouseEvent*, KigWidget* );
private:
  // all moving objects: these objects are all of the objects that
  // need to be redrawn every time the cursor moves, and after calc is
  // called.  Subclasses should set it in their constructors.
  Objects amo;
  // these are the objects that are not moving at all.. It is
  // calculated as (mdoc->objects() - amo)...
  Objects nmo;

protected:
  NormalMode* mprev;

  KigWidget* mview;
};

class MovingMode
  : public MovingModeBase
{
  // explicitly moving objects: these are the objects that the user
  // requested to move...
  Objects emo;
  void stopMove();
  void moveTo( const Coordinate& o );
public:
  MovingMode( const Objects& objects, const Coordinate& c,
	      NormalMode* previousMode, KigWidget*, KigDocument* );
  ~MovingMode();
};

class NormalPointRedefineMode
  : public MovingModeBase
{
  NormalPoint* mp;
  void stopMove();
  void moveTo( const Coordinate& o );
public:
  NormalPointRedefineMode( NormalPoint* p, KigDocument* d, KigWidget* v, NormalMode* m );
  ~NormalPointRedefineMode();
};

#endif
