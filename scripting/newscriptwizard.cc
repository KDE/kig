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

#include <QAction>
#include <QTextEdit>
#include <QDialog>

#include <KIconThemes/KIconLoader>
#include <kactioncollection.h>
#include <kglobalsettings.h>
#include <klocale.h>

#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>

#include <ktoolinvocation.h>

#include <assert.h>

static const int IntroPageId = 1;
static const int CodePageId = 2;

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

NewScriptWizard::NewScriptWizard( QWidget* parent, ScriptModeBase* mode, KIconLoader* il )
  : QWizard( parent ),
    mmode( mode ), textedit( 0 ), document( 0 ), docview( 0 ), mIconLoader( il )
{
  setObjectName( QLatin1String( "New Script Wizard" ) );
  setWindowTitle( QDialog::makeStandardCaption( i18n( "New Script" ) ) );
  setOption( HaveHelpButton );

  QWizardPage* firstPage = new QWizardPage( this );
  firstPage->setTitle( i18n( "Select Arguments" ) );
  firstPage->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  QVBoxLayout* lay1 = new QVBoxLayout( firstPage );
  lay1->setMargin( 0 );
  QLabel* infoText = new QLabel( firstPage );
  lay1->addWidget( infoText );
  infoText->setText( i18n( "Select the argument objects (if any)\n"
                           "in the Kig window and press \"Next\"." ) );
  infoText->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  setPage( IntroPageId, firstPage );

  QWizardPage* secondPage = new QWizardPage( this );
  secondPage->setTitle( i18n( "Enter Code" ) );
  secondPage->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  secondPage->setFinalPage( true );
  QVBoxLayout* lay2 = new QVBoxLayout( secondPage );
  lay2->setMargin( 0 );
  mLabelFillCode = new QLabel( secondPage );
  lay2->addWidget( mLabelFillCode );
  setPage( CodePageId, secondPage );

  KTextEditor::Editor* editor = KTextEditor::Editor::instance();
  qDebug() << "EDITOR: " << editor;

  if ( !editor )
  {
    // there is no KDE textditor component installed, so we'll use a
    // simplier KTextEdit
    textedit = new QTextEdit( secondPage );
    textedit->setObjectName( "textedit" );
    textedit->setFont( KGlobalSettings::fixedFont() );
    textedit->setAcceptRichText( false );
    lay2->addWidget( textedit );
  }
  else
  {
    document = editor->createDocument( 0 );
    // creating the 'view', that is what the user see and interact with
    docview = document->createView( secondPage );

    lay2->addWidget( docview );

    // displaying the left border with line numbers
    QAction *a = docview->actionCollection()->action( "view_line_numbers" );
    if ( a )
    {
      a->trigger();
    }

    // creating the popup menu
    QMenu* menu = docview->defaultContextMenu();

    // finally, we install the popup menu
    docview->setContextMenu( menu );
  }

  connect( this, SIGNAL( currentIdChanged( int ) ), this, SLOT( currentIdChanged( int ) ) );
  connect( this, SIGNAL( helpRequested() ), this, SLOT( slotHelpClicked() ) );
}

void NewScriptWizard::currentIdChanged( int id )
{
  switch ( id )
  {
    case IntroPageId:
      mmode->argsPageEntered();
      break;
    case CodePageId:
      mmode->codePageEntered();
      if ( !document )
      {
        textedit->setFocus();
      }
      else
      {
        docview->setFocus();
      }
      break;
    case -1: // no id - skip it
      break;
    default:
      assert( false );
  }
}

void NewScriptWizard::reject()
{
  if ( mmode->queryCancel() )
    QWizard::reject();
}

void NewScriptWizard::accept()
{
  if ( mmode->queryFinish() )
    QWizard::accept();
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
  mLabelFillCode->setText( ScriptType::fillCodeStatement( type ) );
  KIcon scriptIcon( ScriptType::icon( type ), mIconLoader );
  if ( type != ScriptType::Unknown )
  {
    setWindowIcon( scriptIcon );
  }
  setPixmap( LogoPixmap, scriptIcon.pixmap( 64, 64 ) );

  if ( document )
  {
    // setting the highlight mode
    document->setMode( ScriptType::highlightStyle( type ) );
  }
}
