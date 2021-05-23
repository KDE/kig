// This file is part of Kig, a KDE program for Interactive Geometry...
// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>
// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MODES_EDITTYPE_H
#define KIG_MODES_EDITTYPE_H

#include <QDialog>

class Ui_EditTypeWidget;

/**
 * Simply dialog that allow the user the editing of a macro type...
 */
class EditType : public QDialog
{
  Q_OBJECT

  Ui_EditTypeWidget* medittypewidget;

  QString mname;
  QString mdesc;
  QString micon;
public:
  explicit EditType( QWidget* parent, const QString& name = QString(),
                     const QString& desc = QString(), const QString& icon = QString() );
  ~EditType();

  QString name() const;
  QString description() const;
  QString icon() const;

private slots:
  void slotHelp();
  void slotOk();
  void slotCancel();
};

#endif
