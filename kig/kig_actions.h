// kig_actions.h
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef KIG_ACTIONS_H
#define KIG_ACTIONS_H

#include <kaction.h>

class KigDocument;
class KActionCollection;
class QIconSet;

class AddFixedPointAction
  : public KAction
{
  Q_OBJECT
  KigDocument* mdoc;
public:
  AddFixedPointAction( KigDocument* doc, const QIconSet& icon,
                       KActionCollection* parent );
protected slots:
  void slotActivated();
};

#endif
