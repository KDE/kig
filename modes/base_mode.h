// base_mode.h
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

#ifndef KIG_MODE_BASE_MODE_H
#define KIG_MODE_BASE_MODE_H

#include "mode.h"

#include "../misc/objects.h"

#include <qpoint.h>

class Object;
class KigWidget;
class KigDocument;

class BaseMode
  : public KigMode
{
  QPoint mplc;
  Objects moco;

  void leftClicked( QMouseEvent* e, KigWidget* v );
  void leftMouseMoved( QMouseEvent*, KigWidget* );
  void leftReleased( QMouseEvent* e, KigWidget* v );
  void midClicked( QMouseEvent* e, KigWidget* v );
  void midReleased( QMouseEvent* e, KigWidget* v );
  void rightClicked( QMouseEvent*, KigWidget* );
  void mouseMoved( QMouseEvent* e, KigWidget* v );

protected:
  void enableActions();
protected:

  virtual void dragRect( const QPoint& p, KigWidget& w );
  virtual void dragObject( const Objects& os, const QPoint& pointClickedOn, KigWidget& w, bool ctrlOrShiftDown );
  virtual void leftClickedObject( Object* o, const QPoint& p,
                                  KigWidget& w, bool ctrlOrShiftDown ) = 0;
  virtual void midClicked( const QPoint& p, KigWidget& w ) = 0;
  virtual void rightClicked( const Objects& oco, const QPoint& p, KigWidget& w ) = 0;
  virtual void mouseMoved( const Objects& os, const QPoint& p, KigWidget& w ) = 0;

protected:
  BaseMode( KigDocument& );
  ~BaseMode();
};

#endif
