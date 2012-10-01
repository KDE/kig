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

#include "nameobjectactionsprovider.h"

#include <cassert>

#include "popup.h"

#include "../kig/kig_commands.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../modes/normal.h"
#include "../objects/curve_imp.h"
#include "../objects/bogus_imp.h"
#include "../objects/object_calcer.h"
#include "../objects/object_factory.h"
#include "../objects/point_imp.h"

#include <QRegExpValidator>

#include <KInputDialog>
#include <KLocale>

void addNameLabel( ObjectCalcer* object, ObjectCalcer* namecalcer, const Coordinate& loc, KigPart& doc )
{
  std::vector<ObjectCalcer*> args;
  args.push_back( namecalcer );
  const bool namelabelneedsframe = false;
  ObjectCalcer* attachto = 0;
  if ( object->imp()->inherits( PointImp::stype() ) || 
       object->imp()->attachPoint().valid() || 
       object->imp()->inherits( CurveImp::stype() ) )
    attachto = object;
  ObjectHolder* label = ObjectFactory::instance()->attachedLabel(
      QString::fromLatin1( "%1" ), attachto, loc, namelabelneedsframe, args, doc.document() );
  doc.addObject( label );
}

void NameObjectActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    popup.addInternalAction( menu, i18n( "Set &Name..." ), nextfree++ );
  }
  else if ( menu == NormalModePopupObjects::ShowMenu )
  {
    popup.addInternalAction( menu, i18n( "&Name" ), nextfree++ );
  }
}

bool NameObjectActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os, NormalModePopupObjects& popup,
  KigPart& doc, KigWidget& w, NormalMode& )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    if ( id >= 1 )
    {
      id -= 1;
      return false;
    }
    assert( os.size() == 1 );
    QString name = os[0]->name();
    bool ok;
    QRegExp re( ".*" );
    QRegExpValidator* rev = new QRegExpValidator( re,  &doc );
    name = KInputDialog::getText(
               i18n( "Set Object Name" ), i18n( "Set Name of this Object:" ),
               name, &ok, &w, rev );
    if ( ok )
    {
      bool justadded = false;
      ObjectCalcer* namecalcer = os[0]->nameCalcer();
      if ( !namecalcer )
      {
        justadded = true;
        ObjectConstCalcer* c = new ObjectConstCalcer( new StringImp( i18n( "<unnamed object>" ) ) );
        os[0]->setNameCalcer( c );
        namecalcer = c;
      }
      assert( dynamic_cast<ObjectConstCalcer*>( namecalcer ) );
      ObjectConstCalcer* cnamecalcer = static_cast<ObjectConstCalcer*>( os[0]->nameCalcer() );
      MonitorDataObjects mon( cnamecalcer );
      cnamecalcer->setImp( new StringImp( name ) );
      KigCommand* kc = new KigCommand( doc, i18n( "Set Object Name" ) );
      mon.finish( kc );
      doc.history()->push( kc );

      // if we just added the name, we add a label to show it to the user.
      if ( justadded )
        addNameLabel( os[0]->calcer(), namecalcer,
//                    w.fromScreen( w.mapFromGlobal( popup.mapToGlobal( QPoint( 5, 0 ) ) ) ),
                      w.fromScreen( popup.plc() ),
                      doc );
    }
    return true;
  }
  else if ( menu == NormalModePopupObjects::ShowMenu )
  {
    if ( id >= 1 )
    {
      id -= 1;
      return false;
    }
    assert( os.size() == 1 );
    ObjectCalcer* namecalcer = os[0]->nameCalcer();
    if ( !namecalcer )
    {
      ObjectConstCalcer* c = new ObjectConstCalcer( new StringImp( i18n( "<unnamed object>" ) ) );
      os[0]->setNameCalcer( c );
      namecalcer = c;
    }
    addNameLabel( os[0]->calcer(), namecalcer,
//                  w.fromScreen( w.mapFromGlobal( popup.mapToGlobal( QPoint( 5, 0 ) ) ) ), doc );
                  w.fromScreen( popup.plc() ), doc );
    return true;
  }
  else
  {
    return false;
  }
}
