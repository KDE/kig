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
  void run( const QPoint& start, KigWidget& w, KigMode* prev );
  Objects ret();
  bool needClear();
};

#endif
