// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>
// SPDX-FileCopyrightText: 2005-2006 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "newscriptwizard.h"

#include "script_mode.h"

#include <QAction>
#include <QDialog>
#include <QFontDatabase>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

#include <KActionCollection>
#include <KIconEngine>
#include <KHelpClient>

#include <KTextEditor/ConfigInterface>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>

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
  setObjectName( QStringLiteral( "New Script Wizard" ) );
  setWindowTitle( i18nc("@title:window", "New Script") );
  setOption( HaveHelpButton );

  QWizardPage* firstPage = new QWizardPage( this );
  firstPage->setTitle( i18n( "Select Arguments" ) );
  firstPage->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  QVBoxLayout* lay1 = new QVBoxLayout( firstPage );
  lay1->setContentsMargins( 0 ,  0 ,  0 ,  0 );
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
  lay2->setContentsMargins( 0 ,  0 ,  0 ,  0 );
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
    textedit->setObjectName( QStringLiteral("textedit") );
    textedit->setFont( QFontDatabase::systemFont( QFontDatabase::FixedFont ) );
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
    KTextEditor::ConfigInterface* cfgiface = qobject_cast<KTextEditor::ConfigInterface*>( docview );
    if ( cfgiface )
    {
      cfgiface->setConfigValue( QStringLiteral( "line-numbers" ), true );
    }

    // creating the popup menu
    QMenu* menu = docview->defaultContextMenu();

    // finally, we install the popup menu
    docview->setContextMenu( menu );
  }

  connect( this, SIGNAL(currentIdChanged(int)), this, SLOT(currentIdChanged(int)) );
  connect( this, &QWizard::helpRequested, this, &NewScriptWizard::slotHelpClicked );
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
  KHelpClient::invokeHelp( QStringLiteral("scripting"), QStringLiteral("kig") );
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
  QIcon scriptIcon( new KIconEngine( ScriptType::icon( type ), mIconLoader ) );
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
