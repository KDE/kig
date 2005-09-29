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

#ifndef KIG_MODES_NORMAL_H
#define KIG_MODES_NORMAL_H

#include "base_mode.h"

#include <qpoint.h>
#include <set>

class NormalMode
  : public BaseMode
{
public:
  NormalMode( KigPart& );
  ~NormalMode();
protected:
  void dragRect( const QPoint& p, KigWidget& w );
  void dragObject( const std::vector<ObjectHolder*>& os, const QPoint& pointClickedOn,
                   KigWidget& w, bool ctrlOrShiftDown );
  void leftClickedObject( ObjectHolder* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown );
  void midClicked( const QPoint& p, KigWidget& w );
  void rightClicked( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w );
  void mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w,
                   bool shiftpressed );
  void selectAll();
  void deselectAll();
  void invertSelection();

protected:
  /**
   * Objcects were added..
   */
  void redrawScreen( KigWidget* );

  void enableActions();

  void deleteObjects();
  void showHidden();
  void newMacro();
  void editTypes();

public:
  void selectObject( ObjectHolder* o );
  void selectObjects( const std::vector<ObjectHolder*>& os );
  void unselectObject( ObjectHolder* o );
  void clearSelection();

//   KigObjectsPopup* popup( const Objects& os );
//   KigDocumentPopup* popup( KigDocument* );
protected:
  /**
   * selected objects...
   */
  std::set<ObjectHolder*> sos;
};

#endif
