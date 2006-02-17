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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "newscriptwizard.h"
#include "newscriptwizard.moc"

#include "script_mode.h"

#include <qlabel.h>
#include <qlayout.h>

//#include <kactionclasses.h>
//#include <kactioncollection.h>
// make it still work on old kde 3.1...
#include <kaction.h>
//
#include <kapplication.h>
#include <kglobalsettings.h>
#include <kpopupmenu.h>
#include <ktextedit.h>
#include <ktexteditor/clipboardinterface.h>
#include <ktexteditor/dynwordwrapinterface.h>
#include <ktexteditor/editinterface.h>
#include <ktexteditor/editorchooser.h>
#include <ktexteditor/popupmenuinterface.h>
#include <ktexteditor/undointerface.h>
#include <ktexteditor/view.h>

#include <assert.h>

NewScriptWizard::~NewScriptWizard()
{
  if ( !document )
  {
    delete textedit;
  }
  else
  {
    //restoring the state of the dynamic word wrap
    dynamic_cast<KTextEditor::DynWordWrapInterface*>( editor )->setDynWordWrap( prevDynWordWrap );
    delete editor->document();
  }
}

NewScriptWizard::NewScriptWizard( QWidget* parent, ScriptModeBase* mode )
  : NewScriptWizardBase( parent, "New Script Wizard" ),
    mmode( mode )
{
  document = KTextEditor::EditorChooser::createDocument( 0, "KTextEditor::Document" );
//  document = 0;

  gridLayout->expand( 2, 1 );

  if ( !document )
  {
    // there is no KDE textditor component installed, so we'll use a
    // simplier KTextEdit
    textedit = new KTextEdit( mpcode, "textedit" );
    textedit->setFont( KGlobalSettings::fixedFont() );
    gridLayout->addWidget( textedit, 1, 0 );
  }
  else
  {
    // creating the 'view', hat is what the user see and interact with
    editor = document->createView( mpcode, "editor" );
    gridLayout->addWidget( editor, 1, 0 );

    // casting to the interfaces we'll use often
    hli = dynamic_cast<KTextEditor::HighlightingInterface*>( document );

    // displaying the left border with line numbers
    KToggleAction *a = dynamic_cast<KToggleAction*>( editor->actionCollection()->action("view_line_numbers") );
    a->activate();

    // saving the state of dynamic word wrap and disabling it
    prevDynWordWrap = dynamic_cast<KTextEditor::DynWordWrapInterface*>( editor )->dynWordWrap();
    dynamic_cast<KTextEditor::DynWordWrapInterface*>( editor )->setDynWordWrap( false );

    // saving the "no highlight" id
    noHlStyle = hli->hlMode();

    // creating the popup menu
    KPopupMenu* pm = new KPopupMenu( editor );
    // creating the actions for the code editor...
    KActionCollection* ac = new KActionCollection( editor );
    KAction* undoAction = KStdAction::undo( this, SLOT( slotUndo() ), ac );
    KAction* redoAction = KStdAction::redo( this, SLOT( slotRedo() ), ac );
    KAction* cutAction = KStdAction::cut( this, SLOT( slotCut() ), ac );
    KAction* copyAction = KStdAction::copy( this, SLOT( slotCopy() ), ac );
    KAction* pasteAction = KStdAction::paste( this, SLOT( slotPaste() ), ac );
    // ... and plugging them into the popup menu (to build it, of course :) )
    undoAction->plug( pm );
    redoAction->plug( pm );
    pm->insertSeparator();
    cutAction->plug( pm );
    copyAction->plug( pm );
    pasteAction->plug( pm );

    // finally, we install the popup menu
    dynamic_cast<KTextEditor::PopupMenuInterface*>( editor )->installPopup( pm );
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
    editor->setFocus();
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
  kapp->invokeHelp( QString::fromLatin1( "scripting" ),
                    QString::fromLatin1( "kig" ) );
}

void NewScriptWizard::setText( const QString& text )
{
  if ( !document )
  {
    textedit->setText( text );
  }
  else
  {
    dynamic_cast<KTextEditor::EditInterface*>( document )->setText( text );
  }
}

QString NewScriptWizard::text()
{
  if ( !document )
  {
    return textedit->text();
  }
  else
  {
    return dynamic_cast<KTextEditor::EditInterface*>( document )->text();
  }
}

void NewScriptWizard::setType( ScriptType::Type type )
{
  labelFillCode->setText( ScriptType::fillCodeStatement( type ) );

  if ( !!document )
  {
    if ( type != ScriptType::Unknown )
    {
      for ( uint i = 0; i < hli->hlModeCount(); ++i )
      {
        if ( hli->hlModeName( i ) == ScriptType::highlightStyle( type ) )
        {
          // we found our highlight style, setting it
          hli->setHlMode( i );
          return;
        }
      }
    }
    else
    {
      hli->setHlMode( noHlStyle );
    }
  }
}

void NewScriptWizard::slotUndo()
{
  dynamic_cast<KTextEditor::UndoInterface*>( document )->undo();
}

void NewScriptWizard::slotRedo() {
  dynamic_cast<KTextEditor::UndoInterface*>( document )->redo();
}

void NewScriptWizard::slotCut() {
  dynamic_cast<KTextEditor::ClipboardInterface*>( editor )->cut();
}

void NewScriptWizard::slotCopy() {
  dynamic_cast<KTextEditor::ClipboardInterface*>( editor )->copy();
}

void NewScriptWizard::slotPaste() {
  dynamic_cast<KTextEditor::ClipboardInterface*>( editor )->paste();
}
