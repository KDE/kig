// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>

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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef EXPORTTOLATEXDIALOG_H
#define EXPORTTOLATEXDIALOG_H

#include "exporttolatexdialogbase.h"

class KigDocument;
class KigPart;
class KigWidget;

class ExportToLatexDialog
  : public ExportToLatexDialogBase
{
  Q_OBJECT

  KigWidget* mv;
  const KigPart* mpart;

public:
  ExportToLatexDialog( KigWidget* v, const KigPart* part );
  ~ExportToLatexDialog();
  QString fileName();
  bool showAxes();
  bool showGrid();
  bool extraFrame();
public slots:
  void slotOKPressed();
  void slotCancelPressed();
};

#endif
