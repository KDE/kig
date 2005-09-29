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

#ifndef KIG_MODES_MACRO_H
#define KIG_MODES_MACRO_H

#include "base_mode.h"

#include <qobject.h>

class MacroWizard;

class DefineMacroMode
  : public BaseMode
{
public:
  DefineMacroMode( KigPart& );
  ~DefineMacroMode();

  void dragRect( const QPoint& p, KigWidget& w );
  void leftClickedObject( ObjectHolder* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown );
  void rightClicked( const std::vector<ObjectHolder*>& oco, const QPoint& p, KigWidget& w );
  void midClicked( const QPoint& p, KigWidget& w );
  void mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w, bool shiftpressed );

  // called by MacroWizard class
  void givenPageEntered();
  void finalPageEntered();
  void namePageEntered();
  void finishPressed();
  void cancelPressed();
  void macroNameChanged();

protected:
  void enableActions();
  /**
   * update the enabled state of the next buttons on the wizard...
   */
  void updateNexts();
  /**
   * quit this mode...
   */
  void abandonMacro();

  QPoint plc;
  MacroWizard* mwizard;

  // we can't use a set for this because the order is important
  std::vector<ObjectHolder*> mgiven;
  std::vector<ObjectHolder*> mfinal;
};

#endif
