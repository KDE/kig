// Copyright (C)  2005  Pino Toscano       <toscano.pino@tiscali.it>

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
// USA

#ifndef KIG_MISC_KIGFILEDIALOG_H
#define KIG_MISC_KIGFILEDIALOG_H

#include <kfiledialog.h>

/**
 * This file dialog is pretty like KFileDialog, but allow us to make an option
 * widget popup to the user.
 */
class KigFileDialog
  : public KFileDialog
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
   *                 documentation of KFileDialog for more help about this
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

protected slots:
  virtual void accept();

};

#endif
