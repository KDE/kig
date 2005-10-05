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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef MOVING_H
#define MOVING_H

#include "mode.h"

#include "../misc/coordinate.h"
#include "../objects/object_calcer.h"

class ObjectType;
class Coordinate;
class NormalPoint;
class KigWidget;
class KigDocument;
class MonitorDataObjects;

/**
 * "Template method" pattern ( see the Design patterns book ):
 * This is a base class for two modes: normal MovingMode: used for
 * moving a set of objects around, using Object::startMove,
 * Object::moveTo and Object::stopMove, and another mode
 * PointRedefineMode, used for redefining a NormalPoint...
 */
class MovingModeBase
  : public KigMode
{
protected:
  KigWidget& mview;
private:
  // all moving objects: these objects are all of the objects that
  // need to be redrawn every time the cursor moves, and after calc is
  // called.
  std::vector<ObjectCalcer*> mcalcable;
  std::vector<ObjectHolder*> mdrawable;
protected:
  MovingModeBase( KigPart& doc, KigWidget& v );
  ~MovingModeBase();

  /**
   * Subclasses should call this in their constructor, when they know
   * which objects will be moving around... They are expected to be in
   * the right order for being calc()'ed...
   */
  void initScreen( const std::vector<ObjectCalcer*>& amo );

  // in these functions, subclasses should do the equivalent of
  // Object::stopMove() and moveTo()...  Note that no calc()'ing or
  // drawing is to be done..
  virtual void stopMove() = 0;
  virtual void moveTo( const Coordinate& o, bool snaptogrid ) = 0;

public:
  void leftReleased( QMouseEvent*, KigWidget* );
  void leftMouseMoved( QMouseEvent*, KigWidget* );
  void mouseMoved( QMouseEvent*, KigWidget* );
};

class MovingMode
  : public MovingModeBase
{
  class Private;
  Private* d;
  void stopMove();
  void moveTo( const Coordinate& o, bool snaptogrid );
public:
  MovingMode( const std::vector<ObjectHolder*>& objects, const Coordinate& c,
	      KigWidget&, KigPart& );
  ~MovingMode();
};

class PointRedefineMode
  : public MovingModeBase
{
  ObjectHolder* mp;
  std::vector<ObjectCalcer::shared_ptr> moldparents;
  const ObjectType* moldtype;
  MonitorDataObjects* mmon;
  void stopMove();
  void moveTo( const Coordinate& o, bool snaptogrid );
public:
  PointRedefineMode( ObjectHolder* p, KigPart& d, KigWidget& v );
  ~PointRedefineMode();
};

#endif
