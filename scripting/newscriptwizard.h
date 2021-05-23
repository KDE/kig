// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_SCRIPTING_NEWSCRIPTWIZARD_H
#define KIG_SCRIPTING_NEWSCRIPTWIZARD_H

#include <QWizard>

#include "script-common.h"

class QLabel;
class QTextEdit;

class KIconLoader;

namespace KTextEditor
{
  class Document;
  class View;
}
class ScriptModeBase;

class NewScriptWizard
  : public QWizard
{
  Q_OBJECT
  ScriptModeBase* mmode;
public:
  NewScriptWizard( QWidget* parent, ScriptModeBase* mode, KIconLoader* il );
  ~NewScriptWizard();

  void setText( const QString& text );
  QString text() const;

  void setType( ScriptType::Type type );

public slots:
  void accept() override;
  void reject() override;

protected slots:
  void slotHelpClicked();
  void currentIdChanged( int id );

protected:
  QLabel* mLabelFillCode;
  QTextEdit* textedit;
  KTextEditor::Document* document;
  KTextEditor::View* docview;
  KIconLoader* mIconLoader;
};

#endif
