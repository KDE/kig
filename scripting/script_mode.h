// script_mode.h
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_SCRIPTING_SCRIPT_MODE_H
#define KIG_SCRIPTING_SCRIPT_MODE_H

#include "../modes/base_mode.h"

class NewScriptWizard;

/**
 * Mode to create a new scripted type..
 */
class ScriptMode
  : public BaseMode
{
  Objects margs;
  NewScriptWizard* mwizard;

  enum WAWD { SelectingArgs, EnteringCode };
  WAWD mwawd;
public:
  ScriptMode( KigDocument& doc );
  ~ScriptMode();

  void dragRect( const QPoint& p, KigWidget& w );
//  void dragObject( const Objects& os, const QPoint& pointClickedOn, KigWidget& w, bool ctrlOrShiftDown );
  void leftClickedObject( Object* o, const QPoint& p,
                          KigWidget& w, bool actrlOrShiftDown );
  void mouseMoved( const Objects& os, const QPoint& p, KigWidget& w, bool shiftpressed );
  void midClicked( const QPoint&, KigWidget& );
  void rightClicked( const Objects&, const QPoint&, KigWidget& );

  void argsPageEntered();
  void codePageEntered();

  void finishPressed();
  void cancelPressed();

  void redrawScreen();

  void killMode();

  void enableActions();
};

#endif
