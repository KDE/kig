// guiaction_list.h
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_NW_MISC_GUIACTION_LIST_H
#define KIG_NW_MISC_GUIACTION_LIST_H

#include "objects.h"

class GUIAction;
class KigDocument;

class GUIActionList
{
public:
  typedef myvector<GUIAction*> avectype;
  typedef myvector<KigDocument*> dvectype;
private:
  avectype mactions;
  dvectype mdocs;
  GUIActionList();
  ~GUIActionList();
public:
  static GUIActionList* instance();
  const avectype& actions() const { return mactions; };

  // register this document, so that it receives notifications for
  // added and removed actions..
  void regDoc( KigDocument* d );
  void unregDoc( KigDocument* d );

  void add( GUIAction* a );
  void remove( GUIAction* a );
};

#endif
