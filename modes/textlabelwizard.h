// textlabelwizard.h
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

#ifndef KIG_MODES_TEXTLABELWIZARD_H
#define KIG_MODES_TEXTLABELWIZARD_H

#include "textlabelwizardbase.h"

class TextLabelConstructionMode;

class TextLabelWizard : public TextLabelWizardBase
{
  Q_OBJECT
public:
  TextLabelWizard( QWidget* parent, TextLabelConstructionMode* mode );
  ~TextLabelWizard();

  void back();
  void next();
  void reject();
  void accept();

  void relayoutArgsPage();
private slots:
  void textChanged( const QString& );
  void linkClicked( int which );
  void slotHelpClicked();
private:
  TextLabelConstructionMode* mmode;
};

#endif // TEXTLABELWIZARD_H
