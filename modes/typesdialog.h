/*
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
*/

#ifndef KIG_MODES_TYPESDIALOG_H
#define KIG_MODES_TYPESDIALOG_H

#include <kdialogbase.h>

#include "../misc/lists.h"

class QMenu;
class KigPart;
class KigDocument;
class Ui_TypesWidget;

/**
 * Manage the macro types...
 */
class TypesDialog : public KDialogBase
{
  Q_OBJECT

  // necessary because some MacroList functions need it..
  KigPart& mpart;
  QMenu* popup;
  Ui_TypesWidget* mtypeswidget;
public:
  TypesDialog( QWidget* parent, KigPart& );
  ~TypesDialog();

protected slots:
  virtual void slotHelp();
  virtual void slotOk();
  virtual void slotCancel();

  void deleteType();
  void exportType();
  void importTypes();
  void editType();
//  void contextMenuRequested( Q3ListViewItem* i, const QPoint& p, int c );

private:
  void loadAllMacros();
  typedef MacroList::vectype vec;
};

#endif
