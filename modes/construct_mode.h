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

#ifndef KIG_NW_MODES_CONSTRUCT_MODE_H
#define KIG_NW_MODES_CONSTRUCT_MODE_H

#include "base_mode.h"

class ObjectConstructor;

class ConstructMode
  : public BaseMode
{
  const ObjectConstructor* mctor;
  Object* mpt;

  Objects mparents;

  void selectObject( Object* o, const QPoint& p, KigWidget& w );
public:
  ConstructMode( KigDocument& d, const ObjectConstructor* ctor );
  ~ConstructMode();
protected:
  void leftClickedObject( Object* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown );
  void midClicked( const QPoint& p, KigWidget& w );
  void rightClicked( const Objects& oco, const QPoint& p, KigWidget& w );
  void mouseMoved( const Objects& os, const QPoint& p, KigWidget& w );
};

#endif
