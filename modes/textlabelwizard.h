// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

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
