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

#ifndef NORMAL_H
#define NORMAL_H

#include "mode.h"

#include "../misc/objects.h"

#include <qpoint.h>

class Object;

class NormalMode
  : public KigMode
{
public:
  NormalMode( KigDocument& );
  ~NormalMode();
  void leftClicked( QMouseEvent*, KigWidget* );
  void leftMouseMoved( QMouseEvent*, KigWidget* );
  void leftReleased( QMouseEvent*, KigWidget* );
  void midClicked( QMouseEvent*, KigWidget* v );
  void midMouseMoved( QMouseEvent*, KigWidget* );
  void midReleased( QMouseEvent*, KigWidget* );
  void rightClicked( QMouseEvent*, KigWidget* );
  void rightMouseMoved( QMouseEvent*, KigWidget* );
  void rightReleased( QMouseEvent*, KigWidget* );
  void mouseMoved( QMouseEvent*, KigWidget* );

  /**
   * Objcects were added by a command in mDoc->history.
   */
  void objectsAdded();

   /**
   * Objcects were removed by a command in mDoc->history.
   */
  void objectsRemoved();

  void enableActions();

  void deleteObjects();
  void showHidden();
  void newMacro();
  void editTypes();

  void selectObject( Object* o );
  void selectObjects( Objects& os );
  void unselectObject( Object* o );
  void clearSelection();

//   KigObjectsPopup* popup( const Objects& os );
//   KigDocumentPopup* popup( KigDocument* );
protected:
  // selected objects...
  Objects sos;

  // objects clicked on...
  Objects oco;

  // point last clicked..
  QPoint plc;
};

#endif
