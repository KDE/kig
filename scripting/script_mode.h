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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_SCRIPTING_SCRIPT_MODE_H
#define KIG_SCRIPTING_SCRIPT_MODE_H

#include "script-common.h"

#include "../modes/base_mode.h"

#include <list>

class NewScriptWizard;

/**
 * Base mode to interact with a script.
 */
class ScriptModeBase
  : public BaseMode
{
protected:
  ScriptModeBase( KigPart& doc );

// mp: argument list is implemented as a std::list instead of std::set
// because otherwise the user loses the correct argument ordering (in
// case of more than one argument
  std::list<ObjectHolder*> margs;
  NewScriptWizard* mwizard;

  KigPart& mpart;

  enum WAWD { SelectingArgs, EnteringCode };
  WAWD mwawd;

private:
  ScriptType::Type mtype;

public:
  virtual ~ScriptModeBase();

  using BaseMode::midClicked;
  using BaseMode::rightClicked;
  using BaseMode::mouseMoved;

  void dragRect( const QPoint& p, KigWidget& w ) override;
  void leftClickedObject( ObjectHolder* o, const QPoint& p,
                          KigWidget& w, bool actrlOrShiftDown ) override;
  void mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p,
                   KigWidget& w, bool shiftpressed ) override;
  void midClicked( const QPoint&, KigWidget& ) override;
  void rightClicked( const std::vector<ObjectHolder*>&, const QPoint&, KigWidget& ) override;

  void argsPageEntered();
  void codePageEntered();

  virtual bool queryFinish() = 0;
  virtual bool queryCancel() = 0;

  void redrawScreen( KigWidget* w ) override;

  void killMode();

  void enableActions() override;

  void setScriptType( ScriptType::Type type );

  void addArgs( const std::vector<ObjectHolder*>& obj, KigWidget& w );

  void goToCodePage();

};

/**
 * Script mode to create a script.
 */
class ScriptCreationMode
  : public ScriptModeBase
{
public:
  explicit ScriptCreationMode( KigPart& doc );
  virtual ~ScriptCreationMode();

  bool queryFinish() override;
  bool queryCancel() override;
};

/**
 * Script mode to edit an already-built script.
 */
class ScriptEditMode
  : public ScriptModeBase
{
private:
  ObjectTypeCalcer* mexecuted;
  std::vector<ObjectCalcer*> mexecargs;
  std::vector<ObjectCalcer*> mcompiledargs;

  QString morigscript;

public:
  ScriptEditMode( ObjectTypeCalcer* exec_calc, KigPart& doc );
  virtual ~ScriptEditMode();

  bool queryFinish() override;
  bool queryCancel() override;
};

#endif
