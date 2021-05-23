/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
