// construct_mode.h
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_MODES_CONSTRUCT_MODE_H
#define KIG_MODES_CONSTRUCT_MODE_H

#include "base_mode.h"

class ObjectConstructor;
class RealObject;

class ConstructMode
  : public BaseMode
{
  const ObjectConstructor* mctor;
  // this is the point that we move around, in case the user wants to
  // add a point somewhere..
  Object* mpt;

  Objects mparents;

public:
  void selectObject( Object* o, KigWidget& w );
  void selectObjects( const Objects& os, KigWidget& w );
  ConstructMode( KigDocument& d, const ObjectConstructor* ctor );
  ~ConstructMode();
protected:
  void leftClickedObject( Object* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown );
  void midClicked( const QPoint& p, KigWidget& w );
  void rightClicked( const Objects& oco, const QPoint& p, KigWidget& w );
  void mouseMoved( const Objects& os, const QPoint& p, KigWidget& w, bool shiftpressed );

  void enableActions();
  void cancelConstruction();
};

class PointConstructMode
  : public BaseMode
{
  // this is the point that we move around, for the user to add
  // somewhere..
  Object* mpt;
public:
  PointConstructMode( KigDocument& d );
  ~PointConstructMode();
protected:
  void leftClickedObject( Object* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown );
  void midClicked( const QPoint& p, KigWidget& w );
  void rightClicked( const Objects& oco, const QPoint& p, KigWidget& w );
  void mouseMoved( const Objects& os, const QPoint& p, KigWidget& w, bool shiftpressed );

  void enableActions();
  void cancelConstruction();
};

#endif
