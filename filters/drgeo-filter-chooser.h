// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef DRGEOFILTERCHOOSER_H
#define DRGEOFILTERCHOOSER_H

#include "drgeo-filter-chooserbase.h"

class QListBoxItem;
class QStringList;

class KigFilterDrgeoChooser
  : public KigFilterDrgeoChooserBase
{
  Q_OBJECT

public:
  KigFilterDrgeoChooser( const QStringList& l );
  ~KigFilterDrgeoChooser();
public slots:
  void slotOKPressed();
  void slotCancelPressed();
  void slotExecuted( QListBoxItem* );
};

#endif
