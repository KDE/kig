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

#ifndef KIG_MODES_TEXTLABELWIZARD_H
#define KIG_MODES_TEXTLABELWIZARD_H

#include <QWizard>

class ArgsPage;
class LinksLabel;
class TextLabelModeBase;
class TextPage;

class TextLabelWizard : public QWizard
{
  Q_OBJECT
public:
  TextLabelWizard( QWidget* parent, TextLabelModeBase* mode );
  ~TextLabelWizard();

  static const int TextPageId = 0;
  static const int ArgsPageId = 1;

  LinksLabel* linksLabel();
  QString text() const;
  void setText( const QString& newtext );

public slots:
  void reject() Q_DECL_OVERRIDE;
  void accept() Q_DECL_OVERRIDE;

private slots:
  void textChanged();
  void linkClicked( int which );
  void currentIdChanged( int id );
  void slotHelpClicked();
private:
  TextLabelModeBase* mmode;
  TextPage* mtextPage;
  ArgsPage* margsPage;
};

#endif // TEXTLABELWIZARD_H
