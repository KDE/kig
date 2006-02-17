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

#ifndef KIG_SCRIPTING_NEWSCRIPTWIZARD_H
#define KIG_SCRIPTING_NEWSCRIPTWIZARD_H

#include "newscriptwizardbase.h"

#include "script-common.h"

#include <ktextedit.h>
#include <ktexteditor/document.h>
#include <ktexteditor/highlightinginterface.h>
#include <ktexteditor/view.h>

#include <algorithm>

class ScriptModeBase;

class NewScriptWizard
  : public NewScriptWizardBase
{
  Q_OBJECT
  ScriptModeBase* mmode;
public:
  NewScriptWizard( QWidget* parent, ScriptModeBase* mode );
  ~NewScriptWizard();

  void back();
  void next();
  void reject();

  void setText( const QString& text );
  QString text();

  void setType( ScriptType::Type type );

public slots:
  void slotHelpClicked();
  void accept();

  void slotUndo();
  void slotRedo();
  void slotCut();
  void slotCopy();
  void slotPaste();

protected:
  KTextEdit* textedit;
  KTextEditor::Document* document;
  KTextEditor::HighlightingInterface* hli;
  KTextEditor::View* editor;

  uint noHlStyle;
  bool prevDynWordWrap;
};

#endif
