// dragrectmode.h
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


#ifndef KIG_MODES_DRAGRECTMODE_H
#define KIG_MODES_DRAGRECTMODE_H

#include "mode.h"

#include "../misc/objects.h"

#include <qpoint.h>

/**
 * DragRectMode is a mode that provides a rect for selecting the
 * objects inside it.  It uses some QEventLoop magic to provide a very
 * clean interface.  Here's an example of how to use it, from
 * normal.cc:
 * <code>
 * DragRectMode d( mDoc );
 * d.run( e->pos(), *v, this );
 * Objects sel = d.ret();
 * </code>
 */
class DragRectMode
  : public KigMode
{
  QPoint mstart;
  Objects mret;
  bool mnc;
private:
  void released( const QPoint& p, KigWidget& w, bool nc );
  void released( QMouseEvent* e, KigWidget& w );
  void moved( const QPoint& p, KigWidget& w );
  void moved( QMouseEvent*, KigWidget& w );

  void leftMouseMoved( QMouseEvent*, KigWidget* );
  void leftReleased( QMouseEvent*, KigWidget* );
  void midMouseMoved( QMouseEvent*, KigWidget* );
  void midReleased( QMouseEvent*, KigWidget* );
  void rightMouseMoved( QMouseEvent*, KigWidget* );
  void rightReleased( QMouseEvent*, KigWidget* );
  void mouseMoved( QMouseEvent*, KigWidget* );

  void enableActions();

public:
  DragRectMode( KigDocument* );
  ~DragRectMode();

  // "execute the DragRectMode". This returns after a mouse button is
  // released..
  void run( const QPoint& start, KigWidget& w, KigMode* prev );

  // this returns the selected objects..
  Objects ret() const;

  // this returns false if the control or shift button were pressed
  // when the mouse button was released, and true otherwise.  This is
  // because the user expects us to not clear the selection before
  // adding the newly selected objects if (s)he pressed control or
  // shift..
  bool needClear() const;
};

#endif
