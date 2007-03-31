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

#include <qwizard.h>

#include "script-common.h"

class QLabel;
class KTextEdit;
namespace KTextEditor
{
  class Document;
  class HighlightingInterface;
  class View;
}
class ScriptModeBase;

class NewScriptWizard
  : public QWizard
{
  Q_OBJECT
  ScriptModeBase* mmode;
public:
  NewScriptWizard( QWidget* parent, ScriptModeBase* mode );
  ~NewScriptWizard();

  void setText( const QString& text );
  QString text() const;

  void setType( ScriptType::Type type );

public slots:
  void accept();
  void reject();

protected slots:
  void slotHelpClicked();
  void currentIdChanged( int id );

protected:
  QLabel* mLabelFillCode;
  KTextEdit* textedit;
  KTextEditor::Document* document;
  KTextEditor::HighlightingInterface* hli;
  KTextEditor::View* docview;
};

#endif
