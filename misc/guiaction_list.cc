// guiaction_list.cc
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

#include "guiaction_list.h"

#include "guiaction.h"
#include "../kig/kig_part.h"

GUIActionList* GUIActionList::instance()
{
  static GUIActionList l;
  return &l;
};

GUIActionList::~GUIActionList()
{
  for ( avectype::iterator i = mactions.begin(); i != mactions.end(); ++i )
    delete *i;
}

GUIActionList::GUIActionList()
{
}

void GUIActionList::regDoc( KigDocument* d )
{
  mdocs.push_back( d );
}

void GUIActionList::unregDoc( KigDocument* d )
{
  mdocs.remove( d );
}

void GUIActionList::add( GUIAction* a )
{
  mactions.push_back( a );
  for ( uint i = 0; i < mdocs.size(); ++i )
    mdocs[i]->actionAdded( a );
}

void GUIActionList::remove( GUIAction* a )
{
  mactions.remove( a );
  for ( uint i = 0; i < mdocs.size(); ++i )
    mdocs[i]->actionRemoved( a );
}
