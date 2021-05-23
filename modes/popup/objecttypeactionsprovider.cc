/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "objecttypeactionsprovider.h"

#include "popup.h"

#include "../../kig/kig_part.h"
#include "../../kig/kig_view.h"
#include "../../modes/normal.h"
#include "../../objects/object_holder.h"
#include "../../objects/object_type.h"

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

