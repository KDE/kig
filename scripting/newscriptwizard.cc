// newscriptwizard.cc
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

#include "newscriptwizard.h"
#include "newscriptwizard.moc"

#include "script_mode.h"

#include <kapplication.h>
#include <kglobalsettings.h>
#include <ktextedit.h>

#include <assert.h>

NewScriptWizard::~NewScriptWizard()
{
}

NewScriptWizard::NewScriptWizard( QWidget* parent, ScriptMode* mode )
  : NewScriptWizardBase( parent, "New Script Wizard" ),
    mmode( mode )
{
  codeeditor->setFont( KGlobalSettings::fixedFont() );

  connect( this, SIGNAL( helpClicked() ), this, SLOT( slotHelpClicked() ) );
}

void NewScriptWizard::back()
{
  if ( currentPage() == mpcode )
  {
    // currentPage() is not yet updated, so we're now entering the
    // args page..
    mmode->argsPageEntered();
  }
  else assert( false );
  NewScriptWizardBase::back();
}

void NewScriptWizard::next()
{
  if ( currentPage() == mpargs )
    mmode->codePageEntered();
  else assert( false );
  NewScriptWizardBase::next();
}

void NewScriptWizard::reject()
{
  if ( mmode->queryCancel() )
    NewScriptWizardBase::reject();
}

void NewScriptWizard::accept()
{
  if ( mmode->queryFinish() )
    NewScriptWizardBase::accept();
}

void NewScriptWizard::slotHelpClicked()
{
  kapp->invokeHelp( QString::fromLatin1( "scripting" ),
                    QString::fromLatin1( "kig" ) );
}
