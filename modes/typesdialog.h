/**
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef KIG_MODES_TYPESDIALOG_H
#define KIG_MODES_TYPESDIALOG_H

#include "typesdialogbase.h"

#include <klistbox.h>

class Macro;

class MacroListElement
  : public QListBoxText
{
  Macro* macro;
public:
  MacroListElement( Macro* m );
  Macro* getMacro() { return macro; };
};

class KigDocument;

class TypesDialog : public TypesDialogBase
{
  Q_OBJECT

  // necessary because some MacroList functions need it..
  const KigDocument& mdoc;
public:
  TypesDialog( QWidget* parent, const KigDocument& );
  ~TypesDialog();

public slots:
  void helpSlot();
  void okSlot();

protected slots:
  void deleteType();
  void exportType();
  void importTypes();
};

#endif
