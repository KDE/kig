/**
    This file is part of Kig, a KDE program for Interactive Geometry...
    Copyright (C) 2012  David E. Narvaez <david.narvaez@computer.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "objecttypeactionsprovider.h"

#include "popup.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../modes/normal.h"
#include "../objects/object_holder.h"
#include "../objects/object_type.h"

void ObjectTypeActionsProvider::fillUpMenu(
  NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( popup.objects().size() != 1 ) return;
  if ( menu != NormalModePopupObjects::ToplevelMenu ) return;
  ObjectHolder* to = popup.objects()[0];
  ObjectTypeCalcer* c = dynamic_cast<ObjectTypeCalcer*>( to->calcer() );
  if ( ! c ) return;
  const ObjectType* t = c->type();

  QStringList l = t->specialActions();
  mnoa = l.count();
  for ( int i = 0; i < mnoa; ++i )
    popup.addInternalAction( menu, l.at( i ), nextfree++ );
}

bool ObjectTypeActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os,
  NormalModePopupObjects&,
  KigPart& doc, KigWidget& w, NormalMode& m )
{
  if ( menu != NormalModePopupObjects::ToplevelMenu ) return false;
  if ( id >= mnoa )
  {
    id -= mnoa;
    return false;
  }
  assert( os.size() == 1 );
  ObjectTypeCalcer* oc = dynamic_cast<ObjectTypeCalcer*>( os[0]->calcer() );
  assert(  oc );

  oc->type()->executeAction( id, *os[0], *oc, doc, w, m );
  return true;
}

