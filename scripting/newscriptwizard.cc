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
#include <qmenu.h>

#include <kaction.h>
#include <kactioncollection.h>
#include <kglobalsettings.h>
#include <ktextedit.h>
#if 0
#include <ktexteditor/clipboardinterface.h>
#include <ktexteditor/dynwordwrapinterface.h>
#endif
#include <ktexteditor/editor.h>
#if 0
#include <ktexteditor/undointerface.h>
#endif
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
#if 0
    //restoring the state of the dynamic word wrap
    dynamic_cast<KTextEditor::DynWordWrapInterface*>( editor )->setDynWordWrap( prevDynWordWrap );
#endif
    delete document;
  }
}

NewScriptWizard::NewScriptWizard( QWidget* parent, ScriptModeBase* mode )
  : NewScriptWizardBase( parent, "New Script Wizard" ),
    mmode( mode ), textedit( 0 ), document( 0 ), hli( 0 ), docview( 0 )
{
//  document = KTextEditor::EditorChooser::createDocument( 0, "KTextEditor::Document" );
#if 0
  KTextEditor::Editor* editor = KTextEditor::editor( "katepart" );
  kDebug() << "EDITOR: " << editor << endl;
#endif
  KTextEditor::Editor* editor = 0;

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
    docview = dynamic_cast<KTextEditor::View*>( document->createView( mpcode ) );

    gridLayout->addWidget( docview, 1, 0 );

    // casting to the interfaces we'll use often
    hli = dynamic_cast<KTextEditor::HighlightingInterface*>( document );

    // displaying the left border with line numbers
    KToggleAction *a = dynamic_cast<KToggleAction*>( document->actionCollection()->action( "view_line_numbers" ) );
    a->trigger();

#if 0
    // saving the state of dynamic word wrap and disabling it
    prevDynWordWrap = dynamic_cast<KTextEditor::DynWordWrapInterface*>( editor )->dynWordWrap();
    dynamic_cast<KTextEditor::DynWordWrapInterface*>( editor )->setDynWordWrap( false );
#endif

    // saving the "no highlight" id
    noHlStyle = hli->hlMode();

    // creating the popup menu
    QMenu* menu = new QMenu( docview );
    // creating the actions for the code editor and plugging them into
    // the popup menu (to build it, of course :) )
    KAction* act = document->actionCollection()->action( "edit_undo" );
    connect( act, SIGNAL( activated() ), this, SLOT( slotUndo() ) );
    act->plug( menu );
    act = document->actionCollection()->action( "edit_redo" );
    connect( act, SIGNAL( activated() ), this, SLOT( slotRedo() ) );
    act->plug( menu );
    act = document->actionCollection()->action( "edit_cut" );
    connect( act, SIGNAL( activated() ), this, SLOT( slotCut() ) );
    act->plug( menu );
    act = document->actionCollection()->action( "edit_copy" );
    connect( act, SIGNAL( activated() ), this, SLOT( slotCopy() ) );
    act->plug( menu );
    act = document->actionCollection()->action( "edit_paste" );
    connect( act, SIGNAL( activated() ), this, SLOT( slotPaste() ) );
    act->plug( menu );

#if 0
    KActionCollection* ac = new KActionCollection( document );
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
#endif

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

QString NewScriptWizard::text()
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
#if 0
  dynamic_cast<KTextEditor::UndoInterface*>( document )->undo();
#endif
}

void NewScriptWizard::slotRedo() {
#if 0
  dynamic_cast<KTextEditor::UndoInterface*>( document )->redo();
#endif
}

void NewScriptWizard::slotCut() {
#if 0
  dynamic_cast<KTextEditor::ClipboardInterface*>( editor )->cut();
#endif
}

void NewScriptWizard::slotCopy() {
#if 0
  dynamic_cast<KTextEditor::ClipboardInterface*>( editor )->copy();
#endif
}

void NewScriptWizard::slotPaste() {
#if 0
  dynamic_cast<KTextEditor::ClipboardInterface*>( editor )->paste();
#endif
}
