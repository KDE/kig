// macro.h
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

#ifndef KIG_MODES_MACRO_H
#define KIG_MODES_MACRO_H

#include "base_mode.h"

#include "../misc/objects.h"

#include <qobject.h>

class MacroWizard;

class DefineMacroMode
  : public BaseMode
{
public:
  DefineMacroMode( KigDocument& );
  ~DefineMacroMode();

  void dragRect( const QPoint& p, KigWidget& w );
  void leftClickedObject( Object* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown );
  void rightClicked( const Objects& oco, const QPoint& p, KigWidget& w );
  void midClicked( const QPoint& p, KigWidget& w );
  void mouseMoved( const Objects& os, const QPoint& p, KigWidget& w, bool shiftpressed );

  // called by MacroWizard class
  void givenPageEntered();
  void finalPageEntered();
  void namePageEntered();
  void finishPressed();
  void cancelPressed();
  void macroNameChanged();

protected:
  void enableActions();
  // update the enabled state of the next buttons on the wizard...
  void updateNexts();
  // quit this mode...
  void abandonMacro();

  QPoint plc;
  MacroWizard* mwizard;

  Objects mgiven;
  Objects mfinal;
};

#endif
