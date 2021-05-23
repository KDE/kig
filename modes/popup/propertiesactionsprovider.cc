/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "propertiesactionsprovider.h"

#include <cassert>

#include "../../kig/kig_part.h"
#include "../../kig/kig_view.h"
#include "../../objects/bogus_imp.h"
#include "../../objects/object_factory.h"
#include "../../objects/object_holder.h"
#include "../../objects/point_imp.h"

#include <QIcon>

#include <KIconEngine>

void PropertiesActionsProvider::fillUpMenu( NormalModePopupObjects& popup,
                                            int menu, int& nextfree )
{
  if ( popup.objects().size() != 1 ) return;
  ObjectHolder* o = popup.objects()[0];
  int np = o->imp()->numberOfProperties();
  if ( menu != NormalModePopupObjects::ConstructMenu &&
       menu != NormalModePopupObjects::ShowMenu ) return;
  for ( int i = 0; i < np; ++i )
  {
    ObjectImp* prop = o->imp()->property( i, popup.part().document() );
    const char* iconfile = o->imp()->iconForProperty( i );
    bool add = true;
    if ( menu == NormalModePopupObjects::ConstructMenu )
    {
      // we don't want imp's like DoubleImp, since we can't show them
      // anyway..
      add &= ! prop->inherits( BogusImp::stype() );
      // we don't want to construct PointImp's coordinate property,
      // since it would construct a point at the same place as its
      // parent..
      add &= ! ( o->imp()->inherits( PointImp::stype() ) &&
                 prop->inherits( PointImp::stype() ) );
    }
    else if ( menu == NormalModePopupObjects::ShowMenu )
      add &= prop->canFillInNextEscape();
    if ( add )
    {
      if ( iconfile && *iconfile )
      {
        popup.addInternalAction( menu, QIcon( new KIconEngine( iconfile, popup.part().iconLoader() ) ), i18n( o->imp()->properties()[i] ), nextfree++ );
      }
      else
      {
        popup.addInternalAction( menu, i18n( o->imp()->properties()[i] ), nextfree++ );
      };
      mprops[menu-1].push_back( i );
    };
    delete prop;
  };
}

bool PropertiesActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os,
  NormalModePopupObjects& popup,
  KigPart& doc, KigWidget& w, NormalMode& )
{
  if ( menu != NormalModePopupObjects::ConstructMenu &&
       menu != NormalModePopupObjects::ShowMenu )
    return false;
  if ( (uint) id >= mprops[menu - 1].size() )
  {
    id -= mprops[menu - 1].size();
    return false;
  }
  int propid = mprops[menu-1][id];
  assert( os.size() == 1 );
  ObjectHolder* parent = os[0];
  if ( menu == NormalModePopupObjects::ShowMenu )
  {
    std::vector<ObjectCalcer*> args;
    args.push_back( new ObjectPropertyCalcer( parent->calcer(), propid, true ) );
    args.back()->calc( doc.document() );
// TODO: recover the cursor position somehow... the following does not work
// in general...
//    Coordinate c = w.fromScreen( w.mapFromGlobal( popup.mapToGlobal( QPoint( 5, 0 ) ) ) );
// mp: it seems that we have no idea where to position the label, 
// btw what's the meaning of (5,0)?    let the
// attach method decide what to do... (passing an invalidCoord)
//  ///////    Coordinate c = Coordinate::invalidCoord();
    Coordinate c = w.fromScreen( popup.plc() );
    ObjectHolder* label = ObjectFactory::instance()->attachedLabel(
      QStringLiteral( "%1" ), parent->calcer(), c,
      false, args, doc.document() );
    doc.addObject( label );
  }
  else
  {
    ObjectHolder* h = new ObjectHolder(
      new ObjectPropertyCalcer( parent->calcer(), propid, true ) );
    h->calc( doc.document() );
    doc.addObject( h );
  };
  return true;
}

