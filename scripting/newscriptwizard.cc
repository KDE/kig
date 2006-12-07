// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>
// Copyright (C)  2005-2006  Pino Toscano <toscano.pino@tiscali.it>

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "newscriptwizard.h"
#include "newscriptwizard.moc"

#include "script_mode.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qmenu.h>

#include <kaction.h>
#include <kactioncollection.h>
#include <kglobalsettings.h>
#include <ktextedit.h>
#include <ktexteditor/document.h>
#include <ktexteditor/editor.h>
#include <ktexteditor/editorchooser.h>
#include <ktexteditor/highlightinginterface.h>
#include <ktexteditor/view.h>
#include <ktoolinvocation.h>

#include <assert.h>

NewScriptWizard::~NewScriptWizard()
{
  if ( !document )
  {
    delete textedit;
  }
  else
  {
    delete document;
  }
}

NewScriptWizard::NewScriptWizard( QWidget* parent, ScriptModeBase* mode )
  : NewScriptWizardBase( parent, "New Script Wizard" ),
    mmode( mode ), textedit( 0 ), document( 0 ), hli( 0 ), docview( 0 )
{
  KTextEditor::Editor* editor = KTextEditor::EditorChooser::editor();
//  KTextEditor::Editor* editor = 0;
  kDebug() << "EDITOR: " << editor << endl;

  if ( !editor )
  {
    // there is no KDE textditor component installed, so we'll use a
    // simplier KTextEdit
    textedit = new KTextEdit( mpcode );
    textedit->setObjectName( "textedit" );
    textedit->setFont( KGlobalSettings::fixedFont() );
    gridLayout->addWidget( textedit, 1, 0 );
  }
  else
  {
    document = editor->createDocument( 0 );
    // creating the 'view', that is what the user see and interact with
    (void)document->createView( mpcode );
    docview = document->activeView();

    gridLayout->addWidget( docview, 1, 0 );

    // getting the highlight interface
    hli = qobject_cast<KTextEditor::HighlightingInterface*>( document );

    // displaying the left border with line numbers
    QAction *a = docview->actionCollection()->action( "view_line_numbers" );
    if ( a )
    {
      a->trigger();
    }

    // creating the popup menu
    QMenu* menu = new QMenu( docview );
    // adding the standard actions to the context menu
    menu->addAction( docview->actionCollection()->action( "edit_undo" ) );
    menu->addAction( docview->actionCollection()->action( "edit_redo" ) );
    menu->addSeparator();
    menu->addAction( docview->actionCollection()->action( "edit_cut" ) );
    menu->addAction( docview->actionCollection()->action( "edit_copy" ) );
    menu->addAction( docview->actionCollection()->action( "edit_paste" ) );

    // finally, we install the popup menu
    docview->setContextMenu( menu );
  }

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
  if ( !document )
  {
    textedit->setFocus();
  }
  else
  {
    docview->setFocus();
  }
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
  KToolInvocation::invokeHelp( "scripting", "kig" );
}

void NewScriptWizard::setText( const QString& text )
{
  if ( !document )
  {
    textedit->setPlainText( text );
  }
  else
  {
    document->setText( text );
  }
}

QString NewScriptWizard::text() const
{
  if ( !document )
  {
    return textedit->toPlainText();
  }
  else
  {
    return document->text();
  }
}

void NewScriptWizard::setType( ScriptType::Type type )
{
  labelFillCode->setText( ScriptType::fillCodeStatement( type ) );

  if ( !!document )
  {
    // setting the highlight mode
    if ( hli )
    {
      if ( type != ScriptType::Unknown )
      {
        hli->setHighlighting ( ScriptType::highlightStyle( type ) );
      }
      else
      {
        hli->setHighlighting ( QString () );
      }
    }
  }
}
