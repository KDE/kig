// construction_modes.h
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

#ifndef CONSTRUCTION_MODES_H
#define CONSTRUCTION_MODES_H

#include "mode.h"

#include "../misc/objects.h"

#include <qpoint.h>

class Object;
class NormalMode;
class KigWidget;
class Coordinate;

class NormalPoint;
class NormalMode;

class Type;
class StdConstructibleType;
class MultiConstructibleType;

class PointConstructionMode
  : public KigMode
{
protected:
  QPoint plc;
  NormalPoint* mp;
  NormalMode* mprev;

  void updatePoint( const Coordinate& c, const ScreenInfo& );
  void finish( KigWidget* v );

public:
  PointConstructionMode( NormalMode* prev, KigDocument* d );
  ~PointConstructionMode();

  void leftClicked( QMouseEvent* e, KigWidget* v );
  void leftReleased( QMouseEvent* e, KigWidget* v );
  void midClicked( QMouseEvent* e, KigWidget* v );
  void midReleased( QMouseEvent* e, KigWidget* v );
  void rightClicked( QMouseEvent*, KigWidget* ) {};
  void rightMouseMoved( QMouseEvent*, KigWidget* ) {};
  void rightReleased( QMouseEvent*, KigWidget* ) {};
  void mouseMoved( QMouseEvent* e, KigWidget* v );

  void enableActions();

  void cancelConstruction();
};

/**
 * this is a general ConstructionMode for objects that have no special
 * needs... It gets necessary information from static functions on the
 * objects.  Check out object.h or compiler errors to find out which
 * ones... :)
 *
 * the inheritance from PointConstructionMode is an implementation
 * inheritance, but i'm not using private inheritance since we also
 * need to inherit from KigMode and don't want to start messing with
 * virtual inheritance...
 *
 * There is a small Template Method pattern here, in the
 * buildCalcAndAdd() function.  This is because of a special type of
 * objects: where two objects belonging together are constructed at
 * the same time ( e.g. the intersection of a line and a circle... ).
 * This function is reimplemented in MultiConstructionMode, but
 * there's nothing special about this class other than that...
 */
class StdConstructionMode
  : public PointConstructionMode
{
  // object being constructed
  StdConstructibleType* mtype;
  // point last clicked..
  QPoint plc;
  // Objects clicked on...
  Objects oco;
  // Objects selected as args
  Objects osa;

protected:
  void selectArg( Object* o, KigWidget* v );

  // see the class description above, and the implementations of this
  // function in both this class and the MultiConstructionMode
  // below for explanation...
  virtual void buildCalcAndAdd( const Type* t,
		  		const Objects& arguments,
				KigWidget* view );

  // called by either midReleased or leftReleased...
  void addPointRequest( const Coordinate& c, KigWidget* v );

public:
  StdConstructionMode( StdConstructibleType* t, NormalMode* b,
                       KigDocument* d );
  ~StdConstructionMode();

  virtual StdConstructionMode* toStdConstructionMode();

  void leftClicked( QMouseEvent*, KigWidget* );
  void leftReleased( QMouseEvent*, KigWidget* );
  void midClicked( QMouseEvent*, KigWidget* );
  void midReleased( QMouseEvent*, KigWidget* );
  void rightClicked( QMouseEvent*, KigWidget* ) {};
  void rightMouseMoved( QMouseEvent*, KigWidget* ) {};
  void rightReleased( QMouseEvent*, KigWidget* ) {};
  void mouseMoved( QMouseEvent*, KigWidget* );

  void enableActions();

  void cancelConstruction();

  int wantArgs( const Objects& o ) const;
  void selectArgs( const Objects& o, KigWidget* v );

};

class MultiConstructionMode
  : public StdConstructionMode
{
public:
  MultiConstructionMode( MultiConstructibleType* t, NormalMode* b,
                         KigDocument* d );
  ~MultiConstructionMode();
  void buildCalcAndAdd( const Type* t, const Objects& arguments,
			KigWidget* view );
};

#endif
