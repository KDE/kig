/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "objectconstructoractionsprovider.h"

#include "../../kig/kig_part.h"
#include "../../kig/kig_view.h"
#include "../../misc/argsparser.h"
#include "../../misc/lists.h"
#include "../../misc/object_constructor.h"
#include "../../modes/construct_mode.h"
#include "../../modes/normal.h"

#include <KIconEngine>

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
      add = ( menu == NormalModePopupObjects::StartMenu && ! ( *i )->isTransform() && ! ( *i )->isTest() )
            || ( menu == NormalModePopupObjects::ConstructMenu && ( *i )->wantArgs( {}, d, v ) == ArgsParser::Complete );
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
        popup.addInternalAction( menu, QIcon( new KIconEngine( iconfile, popup.part().iconLoader() ) ), (*i)->descriptiveName(), nextfree++ );
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
  if ( ctor->wantArgs( osc, doc.document(), w ) == ArgsParser::Complete )
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

