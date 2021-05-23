// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef MACROWIZARD_H
#define MACROWIZARD_H

#include <QWizard>

class DefineMacroMode;
class GivenArgsPage;
class FinalArgsPage;

class MacroWizard : public QWizard
{
  Q_OBJECT
public:
  MacroWizard( QWidget* parent, DefineMacroMode* m );
  ~MacroWizard();

  static const int GivenArgsPageId = 0;
  static const int FinalArgsPageId = 1;
  static const int MacroInfoPageId = 2;

  void givenArgsChanged();
  void finalArgsChanged();

public slots:
  void reject() Q_DECL_OVERRIDE;
  void accept() Q_DECL_OVERRIDE;

private slots:
  void currentIdChanged( int id );
  void slotHelpClicked();
private:
  DefineMacroMode* mmode;
  GivenArgsPage* mgivenArgsPage;
  FinalArgsPage* mfinalArgsPage;
};

#endif // MACROWIZARD_H
