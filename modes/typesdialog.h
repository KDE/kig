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

#include "typesdialogbase.h"

#include "../misc/lists.h"

#include <qpopupmenu.h>

#include <klistview.h>
#include <kiconloader.h>

class KigPart;
class KigDocument;

/**
 * Manage the macro types...
 */
class TypesDialog : public TypesDialogBase
{
  Q_OBJECT

  // necessary because some MacroList functions need it..
  KigPart& mpart;
  const KIconLoader* il;
  QPopupMenu* popup;
public:
  TypesDialog( QWidget* parent, KigPart& );
  ~TypesDialog();

public slots:
  void helpSlot();
  void okSlot();
  void cancelSlot();

protected slots:
  void deleteType();
  void exportType();
  void importTypes();
  void editType();
  void contextMenuRequested( QListViewItem* i, const QPoint& p, int c );

private:
  QListViewItem* newListItem( Macro* m );
  QString fetchIconFromListItem( QListViewItem* i );
  void loadAllMacros();
  typedef MacroList::vectype vec;
};

#endif
