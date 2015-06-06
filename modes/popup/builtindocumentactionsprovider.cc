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

#include "builtindocumentactionsprovider.h"

#include "popup.h"

#include "../../kig/kig_commands.h"
#include "../../kig/kig_document.h"
#include "../../kig/kig_part.h"
#include "../../kig/kig_view.h"
#include "../../modes/normal.h"
#include "../../misc/coordinate_system.h"

void BuiltinDocumentActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    popup.addInternalAction( menu, i18n( "U&nhide All" ), nextfree++ );
    popup.addInternalAction( menu, popup.part().action( "view_zoom_in" ) );
    popup.addInternalAction( menu, popup.part().action( "view_zoom_out" ) );
    popup.addInternalAction( menu, popup.part().action( "fullscreen" ) );
    nextfree += 3;
  }
  else if ( menu == NormalModePopupObjects::SetCoordinateSystemMenu )
  {
    QStringList l = CoordinateSystemFactory::names();
    mnumberofcoordsystems = l.count();
    int current = popup.part().document().coordinateSystem().id();
    QAction* act = 0;
    for ( int i = 0; i < mnumberofcoordsystems; ++i )
    {
      act = popup.addInternalAction( menu, l.at( i ), nextfree++ );
      act->setCheckable( true );
      if ( i == current )
        act->setChecked( true );
    }
  }
}

bool BuiltinDocumentActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>&,
  NormalModePopupObjects&,
  KigPart& doc, KigWidget&, NormalMode& m )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    qDebug() << "id: " << id;
    if ( id == 0 )
    {
      doc.showHidden();
      m.clearSelection();
      return true;
    }
    id -= 1;
    return false;
  }
  else if ( menu == NormalModePopupObjects::SetCoordinateSystemMenu )
  {
    if ( id >= mnumberofcoordsystems )
    {
      id -= mnumberofcoordsystems;
      return false;
    };
    CoordinateSystem* sys = CoordinateSystemFactory::build( id );
    assert( sys );
    doc.history()->push( KigCommand::changeCoordSystemCommand( doc, sys ) );
    m.clearSelection();
    return true;
  }
  else return false;
}
