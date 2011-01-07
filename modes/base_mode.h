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

#ifndef KIG_MODE_BASE_MODE_H
#define KIG_MODE_BASE_MODE_H

#include "mode.h"

#include <tqpoint.h>
#include <vector>

class KigWidget;
class KigDocument;
class ObjectHolder;

class BaseMode
  : public KigMode
{
  TQPoint mplc;
  std::vector<ObjectHolder*> moco;

  void leftClicked( TQMouseEvent* e, KigWidget* v );
  void leftMouseMoved( TQMouseEvent*, KigWidget* );
  void leftReleased( TQMouseEvent* e, KigWidget* v );
  void midClicked( TQMouseEvent* e, KigWidget* v );
  void midReleased( TQMouseEvent* e, KigWidget* v );
  void rightClicked( TQMouseEvent*, KigWidget* );
  void mouseMoved( TQMouseEvent* e, KigWidget* v );

protected:
  void enableActions();

  std::vector<ObjectHolder*> oco();
  TQPoint pointLocation();
protected:

  virtual void dragRect( const TQPoint& p, KigWidget& w );
  virtual void dragObject( const std::vector<ObjectHolder*>& os, const TQPoint& pointClickedOn, KigWidget& w, bool ctrlOrShiftDown );
  virtual void leftClickedObject( ObjectHolder* o, const TQPoint& p,
                                  KigWidget& w, bool ctrlOrShiftDown ) = 0;
  virtual void midClicked( const TQPoint& p, KigWidget& w ) = 0;
  virtual void rightClicked( const std::vector<ObjectHolder*>& oco, const TQPoint& p, KigWidget& w ) = 0;
  virtual void mouseMoved( const std::vector<ObjectHolder*>& os, const TQPoint& p, KigWidget& w, bool shiftpressed ) = 0;

protected:
  BaseMode( KigPart& );
  ~BaseMode();
};

#endif
