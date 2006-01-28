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

#include <qabstractitemmodel.h>

#include <kdialog.h>

#include "../misc/lists.h"

#include <vector>

class QMenu;
class KigPart;
class Ui_TypesWidget;
class BaseListElement;

/**
 * A model for representing the datas.
 */
class TypesModel
  : public QAbstractTableModel
{
  Q_OBJECT

  std::vector<BaseListElement*> melems;

public:
  TypesModel( QObject* parent = 0 );
  virtual ~TypesModel();

  const std::vector<BaseListElement*>& elements() const;

  void addElements( const std::vector<BaseListElement*>& elems );
  void removeElements( const std::vector<BaseListElement*>& elems );

  void clear();

  // reimplementations from QAbstractTableModel
  virtual int columnCount( const QModelIndex& parent = QModelIndex() ) const;
  virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
  virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
  virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;
};

/**
 * Manage the macro types...
 */
class TypesDialog : public KDialog
{
  Q_OBJECT

  // necessary because some MacroList functions need it..
  KigPart& mpart;
  QMenu* popup;
  Ui_TypesWidget* mtypeswidget;
  TypesModel* mmodel;
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

protected:
  virtual bool eventFilter( QObject* obj, QEvent* event );

private:
  void loadAllMacros( std::vector<BaseListElement*>& el );
  typedef MacroList::vectype vec;

  std::set<int> selectedRows() const;
};

#endif
