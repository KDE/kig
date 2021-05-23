/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "scriptactionsprovider.h"

#ifdef KIG_ENABLE_PYTHON_SCRIPTING

#include "popup.h"

#include "../../kig/kig_part.h"
#include "../../modes/normal.h"
#include "../../objects/text_type.h"

#include <KIconEngine>

/**
 * this is a local function that looks for a python script associated
 * to a clicked object
 */
static ObjectTypeCalcer* getPythonExecuteTypeFromCalcer( ObjectCalcer* o )
{
  ObjectTypeCalcer* oc = dynamic_cast<ObjectTypeCalcer *>( o );
  if ( !oc ) return 0;
  const PythonExecuteType* pythonexec = dynamic_cast<const PythonExecuteType*>( oc->type() );
  if ( pythonexec ) return oc;

  const GenericTextType* text = dynamic_cast<const GenericTextType*>( oc->type() );
  if ( !text ) return 0;
  std::vector<ObjectCalcer*> parents = oc->parents();

  for (uint i = 3; i < parents.size(); i++)
  {
    oc = dynamic_cast<ObjectTypeCalcer *>( parents[i] );
    if ( oc )
    {
      pythonexec = dynamic_cast<const PythonExecuteType*>( oc->type() );
      if ( pythonexec ) return oc;
    }
  }
  return 0;
}

void ScriptActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  if ( menu == NormalModePopupObjects::StartMenu )
  {
    KIconLoader* l = popup.part().iconLoader();
    popup.addInternalAction( menu, QIcon( new KIconEngine( ScriptType::icon( ScriptType::Python ), l ) ), i18n( "Python Script" ), nextfree++ );
    mns++;
  }
  else if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    if ( !popup.objects().empty() &&
         getPythonExecuteTypeFromCalcer( popup.objects().front()->calcer() ) )
    {
      popup.addInternalAction( menu, i18n("Edit Script..."), nextfree );
    }
    nextfree++;
  }
}

bool ScriptActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os,
  NormalModePopupObjects&, KigPart& doc, KigWidget& w, NormalMode& mode )
{
  if ( menu == NormalModePopupObjects::StartMenu )
  {
    if ( id == 0 )
    {
      ScriptCreationMode m( doc );
      m.setScriptType( ScriptType::Python );
      if ( os.size() > 0 )
      {
        mode.clearSelection();
        m.addArgs( os, w );
        m.goToCodePage();
      }
      doc.runMode( &m );
      return true;
    }
    else
    {
      id -= mns;
    }
  }
  else if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    if ( id == 0 )
    {
      ObjectTypeCalcer* oc = getPythonExecuteTypeFromCalcer( os.front()->calcer() );
      if ( oc )
      {
        ScriptEditMode m( oc, doc );
        m.setScriptType( ScriptType::Python );
        doc.runMode( &m );
      }
      return true;
    }
    else
    {
      id -= 1;
    }
  }

  return false;
}
#endif
