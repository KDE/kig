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

#ifndef MACROWIZARD_H
#define MACROWIZARD_H

#include <qwizard.h>

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
  void reject();
  void accept();

private slots:
  void currentIdChanged( int id );
  void slotHelpClicked();
private:
  DefineMacroMode* mmode;
  GivenArgsPage* mgivenArgsPage;
  FinalArgsPage* mfinalArgsPage;
};

#endif // MACROWIZARD_H
