// normal.h
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

#ifndef KIG_MODES_NORMAL_H
#define KIG_MODES_NORMAL_H

#include "base_mode.h"

#include "../misc/objects.h"

#include <qpoint.h>

class Object;

class NormalMode
  : public BaseMode
{
public:
  NormalMode( KigDocument& );
  ~NormalMode();
protected:
  void dragRect( const QPoint& p, KigWidget& w );
  void dragObject( const Objects& os, const QPoint& pointClickedOn,
                           KigWidget& w, bool ctrlOrShiftDown );
  void leftClickedObject( Object* o, const QPoint& p,
                                  KigWidget& w, bool ctrlOrShiftDown );
  void midClicked( const QPoint& p, KigWidget& w );
  void rightClicked( const Objects& os, const QPoint& p, KigWidget& w );
  void mouseMoved( const Objects& os, const QPoint& p, KigWidget& w );
  void selectAll();
  void deselectAll();
  void invertSelection();

protected:
  /**
   * Objcects were added..
   */
  void redrawScreen();

  void enableActions();

  void deleteObjects();
  void showHidden();
  void newMacro();
  void editTypes();

public:
  void selectObject( Object* o );
  void selectObjects( const Objects& os );
  void unselectObject( Object* o );
  void clearSelection();

//   KigObjectsPopup* popup( const Objects& os );
//   KigDocumentPopup* popup( KigDocument* );
protected:
  // selected objects...
  Objects sos;
};

#endif
