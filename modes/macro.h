// macro.h
// Copyright (C)  2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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

#ifndef MACRO_H
#define MACRO_H

#include "mode.h"

#include "../misc/objects.h"

#include <qobject.h>

class MacroWizard;

class DefineMacroMode
  : public KigMode
{
public:
  DefineMacroMode( KigDocument*, KigMode* previousMode );
  ~DefineMacroMode();
  void leftClicked( QMouseEvent*, KigWidget* );
  void leftReleased( QMouseEvent*, KigWidget* );
  void rightClicked( QMouseEvent*, KigWidget* );
  void rightReleased( QMouseEvent*, KigWidget* );
  void mouseMoved( QMouseEvent*, KigWidget* );

  void enableActions();

  // called by MacroWizard class
  void givenPageEntered();
  void finalPageEntered();
  void namePageEntered();
  void finishPressed();
  void cancelPressed();
  void macroNameChanged();

protected:
  // update the enabled state of the next buttons on the wizard...
  void updateNexts();
  // quit this mode...
  void abandonMacro();

  QPoint plc;
  MacroWizard* mwizard;
  KigMode* mprev;

  Objects mgiven;
  Object* mfinal;
};

#endif
