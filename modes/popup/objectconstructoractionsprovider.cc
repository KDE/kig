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

#include "objectconstructoractionsprovider.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/argsparser.h"
#include "../misc/lists.h"
#include "../misc/object_constructor.h"
#include "../modes/construct_mode.h"
#include "../modes/normal.h"

void ObjectConstructorActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  const KigDocument& d = popup.part().document();
  const KigWidget& v = popup.widget();
  typedef ObjectConstructorList::vectype vectype;
  vectype vec = ObjectConstructorList::instance()->constructors();

  for ( vectype::iterator i = vec.begin(); i != vec.end(); ++i )
  {
    bool add = false;
    if ( popup.objects().empty() )
    {
      add = menu == NormalModePopupObjects::StartMenu && ! (*i)->isTransform() && ! (*i)->isTest();
    }
    else
    {
      int ret = (*i)->wantArgs( getCalcers( popup.objects() ), d, v );
      if ( ret == ArgsParser::Invalid ) continue;
      if ( (*i)->isTransform() && popup.objects().size() == 1 ) add = menu == NormalModePopupObjects::TransformMenu;
      else if ( (*i)->isTest() ) add = menu == NormalModePopupObjects::TestMenu;
      else if ( ( *i )->isIntersection() ) add = menu == NormalModePopupObjects::ToplevelMenu;
      else if ( ret == ArgsParser::Complete ) add = menu == NormalModePopupObjects::ConstructMenu;
      else add = menu == NormalModePopupObjects::StartMenu;
    };
    if ( add )
    {
      QByteArray iconfile = (*i)->iconFileName();
      if ( !iconfile.isEmpty() && !iconfile.isNull() )
      {
        popup.addInternalAction( menu, KIcon( iconfile, popup.part().iconLoader() ), (*i)->descriptiveName(), nextfree++ );
      }
      else
        popup.addInternalAction( menu, (*i)->descriptiveName(), nextfree++ );
      mctors[menu].push_back( *i );
    }
  };
}

bool ObjectConstructorActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os,
  NormalModePopupObjects&,
  KigPart& doc, KigWidget& w, NormalMode& m )
{
  if ( (uint) id >= mctors[menu].size() )
  {
    id -= mctors[menu].size();
    return false;
  }

  ObjectConstructor* ctor = mctors[menu][id];
  std::vector<ObjectCalcer*> osc = getCalcers( os );
  if ( ! os.empty() && ctor->wantArgs( osc, doc.document(), w ) == ArgsParser::Complete )
  {
    ctor->handleArgs( osc, doc, w );
    m.clearSelection();
  }
  else
  {
    BaseConstructMode* mode = ctor->constructMode( doc );
    mode->selectObjects( os, w );
    doc.runMode( mode );
    delete mode;
  };
  return true;
}

