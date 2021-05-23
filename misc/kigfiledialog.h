// SPDX-FileCopyrightText: 2005 Pino Toscano <toscano.pino@tiscali.it>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MISC_KIGFILEDIALOG_H
#define KIG_MISC_KIGFILEDIALOG_H

#include <QFileDialog>

/**
 * This file dialog is pretty like QFileDialog, but allow us to make an option
 * widget popup to the user.
 */
class KigFileDialog
  : public QFileDialog
{
  Q_OBJECT

private:
  /**
   * Options widget
   */
  QWidget* mow;

  QString moptcaption;

public:
  /**
   * Construct a new KigFileDialog.
   *
   * \param startDir the start dir of the file dialog. Consult the
   *                 documentation of QFileDialog for more help about this
   * \param filter the filter for the file dialog
   * \param caption the caption of this file dialog
   * \param parent the parent for this file dialog
   */
  KigFileDialog( const QString& startDir, const QString& filter,
                 const QString& caption, QWidget *parent );

  /**
   * Use this to set the widget containing the options of eg an export filter.
   * The option widget will be popped up in a dialog right after the user
   * presses OK and before the dialog is closed.
   *
   * You can construct the option widget with no parent, as it will be
   * reparented.
   *
   * \param w the option widget
   */
  void setOptionsWidget( QWidget* w );

  /**
   * Set the caption of the option dialog
   *
   * \param caption the caption of the option dialog
   */
  void setOptionCaption( const QString& caption );

  /**
   * We lost this from KigFileDialog
   */
  QString selectedFile();

protected slots:
  void accept() Q_DECL_OVERRIDE;

};

#endif
