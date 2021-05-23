/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2006 Pino Toscano <toscano.pino@tiscali.it>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KIG_MODES_HISTORYDIALOG_H
#define KIG_MODES_HISTORYDIALOG_H

class QUndoStack;
class QWidget;
class Ui_HistoryWidget;

#include <QDialog>

/**
 * The HistoryDialog represents a small dialog to navigate back and forth in
 * the history of a construction.
 */
class HistoryDialog
  : public QDialog
{
Q_OBJECT

public:
  HistoryDialog( QUndoStack* kch, QWidget* parent );
  virtual ~HistoryDialog();

private slots:
  void updateWidgets();

  void goToFirst();
  void goBack();
  void goToNext();
  void goToLast();

private:
  QUndoStack* mch;

  Ui_HistoryWidget* mwidget;

  int mtotalsteps;
};

#endif
