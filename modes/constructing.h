// construction_modes.h
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

#ifndef CONSTRUCTION_MODES_H
#define CONSTRUCTION_MODES_H

#include "mode.h"

#include "../misc/objects.h"

#include <qpoint.h>

class Object;
class NormalMode;
class KigView;
class Coordinate;


class NormalPoint;
class NormalMode;

class PointConstructionMode
  : public KigMode
{
protected:
  QPoint plc;
  NormalPoint* mp;
  NormalMode* mprev;

  void updatePoint( const Coordinate& c, double fault );
  void finish( KigView* v );
  void updateScreen( KigView* v );

public:
  PointConstructionMode( NormalMode* prev,  KigDocument* d );
  ~PointConstructionMode();

  void leftClicked( QMouseEvent* e, KigView* v );
  void leftReleased( QMouseEvent* e, KigView* v );
  void midClicked( QMouseEvent* e, KigView* v );
  void midReleased( QMouseEvent* e, KigView* v );
  void rightClicked( QMouseEvent*, KigView* ) {};
  void rightMouseMoved( QMouseEvent*, KigView* ) {};
  void rightReleased( QMouseEvent*, KigView* ) {};
  void mouseMoved( QMouseEvent* e, KigView* v );

  void enableActions();

  void cancelConstruction();
};

/**
 * this is a general ConstructionMode for objects that have no special
 * needs... It gets necessary information from static functions on the
 * objects.  Check out object.h or compiler errors to find out which
 * ones... :)
 *
 * the inheritance from PointConstructionMode is an implentation
 * inheritance, but i'm not using private inheritance since we also
 * need to inherit from KigMode and don't want to start messing with
 * virtual inheritance...
 */
class StdConstructionMode
  : public PointConstructionMode
{
  // object being constructed
  Object* mobc;
  // point last clicked..
  QPoint plc;
  // Objects clicked on...
  Objects oco;

protected:
  void selectArg( Object* o, KigView* v );

  // called by either midReleased or leftReleased...
  void addPointRequest( const Coordinate& c, KigView* v );

public:
  StdConstructionMode( Object* obc, NormalMode* b,  KigDocument* d );
  ~StdConstructionMode();
  void leftClicked( QMouseEvent*, KigView* );
  void leftReleased( QMouseEvent*, KigView* );
  void midClicked( QMouseEvent*, KigView* );
  void midReleased( QMouseEvent*, KigView* );
  void rightClicked( QMouseEvent*, KigView* ) {};
  void rightMouseMoved( QMouseEvent*, KigView* ) {};
  void rightReleased( QMouseEvent*, KigView* ) {};
  void mouseMoved( QMouseEvent*, KigView* );

  void enableActions();

  void cancelConstruction();
};

#endif
