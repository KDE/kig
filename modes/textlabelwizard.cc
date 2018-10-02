// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#include "textlabelwizard.h"

#include "label.h"
#include "linkslabel.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtextedit.h>

#include <QDialog>

#include <KHelpClient>

// defined in label.cc
extern uint percentCount( const QString& s );

class TextPage : public QWizardPage
{
public:
  TextPage( QWidget* parent );

  QTextEdit* mtext;
};

TextPage::TextPage( QWidget* parent )
  : QWizardPage( parent )
{
  setTitle( i18n( "Enter Label Text" ) );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  setFinalPage( true );

  QVBoxLayout* lay = new QVBoxLayout( this );
  lay->setMargin( 0 );
  QLabel* label = new QLabel( this );
  lay->addWidget( label );
  label->setText(
      i18n( "Enter the text for your label here and press \"Next\".\n"
            "If you want to show variable parts, then put %1, %2, ... "
            "at the appropriate places (e.g. \"This segment is %1 units "
            "long.\").", QString( "%1" ), QString( "%2" ) ) ); // grrr i18n()
  label->setAlignment( Qt::AlignTop );
  label->setWordWrap( true );
  mtext = new QTextEdit( this );
  lay->addWidget( mtext );
  QCheckBox* wantframe = new QCheckBox( this );
  lay->addWidget( wantframe );
  wantframe->setText( i18n( "Show text in a frame" ) );

  registerField( "wantframe", wantframe );

  connect( mtext, SIGNAL(textChanged()), parent, SLOT(textChanged()) );
}


class ArgsPage : public QWizardPage
{
public:
  ArgsPage( QWidget* parent, TextLabelModeBase* mode );

  bool validatePage() Q_DECL_OVERRIDE;

  LinksLabel* mlinks;

private:
  TextLabelModeBase* mmode;
};

ArgsPage::ArgsPage( QWidget* parent, TextLabelModeBase* mode )
  : QWizardPage( parent ), mmode( mode )
{
  setTitle( i18n( "Select Arguments" ) );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  setFinalPage( true );

  QVBoxLayout* lay = new QVBoxLayout( this );
  lay->setMargin( 0 );
  QLabel* label = new QLabel( this );
  lay->addWidget( label );
  label->setText(
      i18n( "Now select the argument(s) you need.  For every argument, "
            "click on it, select an object and a property in the Kig "
            "window, and click finish when you are done..." ) );
  label->setWordWrap( true );
  mlinks = new LinksLabel( this );
  lay->addWidget( mlinks );

  connect( mlinks, SIGNAL(changed()), this, SIGNAL(completeChanged()) );
}

bool ArgsPage::validatePage()
{
  return mmode->canFinish();
}


TextLabelWizard::TextLabelWizard( QWidget* parent, TextLabelModeBase* mode )
  : QWizard( parent ), mmode( mode )
{
  setModal( false );
  setObjectName( QLatin1String( "TextLabelWizard" ) );
  setWindowTitle( i18n( "Construct Label" ) );
  setOption( HaveHelpButton );
  setOption( HaveFinishButtonOnEarlyPages );

  mtextPage = new TextPage( this );
  setPage( TextPageId, mtextPage );
  margsPage = new ArgsPage( this, mmode );
  setPage( ArgsPageId, margsPage );

  connect( this, SIGNAL(helpRequested()), this,
           SLOT(slotHelpClicked()) );
  connect( linksLabel(), SIGNAL(linkClicked(int)),
           SLOT(linkClicked(int)) );
  connect( this, SIGNAL(currentIdChanged(int)),
           this, SLOT(currentIdChanged(int)) );

  mtextPage->mtext->setFocus();
}

TextLabelWizard::~TextLabelWizard()
{
}

LinksLabel* TextLabelWizard::linksLabel()
{
  return margsPage->mlinks;
}

QString TextLabelWizard::text() const
{
  return mtextPage->mtext->toPlainText();
}

void TextLabelWizard::setText( const QString& newtext )
{
  mtextPage->mtext->setPlainText( newtext );
}

void TextLabelWizard::reject()
{
  QWizard::reject();
  mmode->cancelPressed();
}

void TextLabelWizard::accept()
{
  if( validateCurrentPage() )
  {
    QWizard::accept();
    mmode->finishPressed();
  }
}

void TextLabelWizard::textChanged()
{
  uint percentcount = percentCount( text() );
  bool finish = mmode->percentCountChanged( percentcount );
  (void)finish;
  button( QWizard::FinishButton )->setEnabled( percentcount == 0 );
  button( QWizard::NextButton )->setEnabled( percentcount > 0 );
}

void TextLabelWizard::linkClicked( int which )
{
  mmode->linkClicked( which );
}

void TextLabelWizard::currentIdChanged( int id )
{
  switch ( id )
  {
    case TextPageId:
      mmode->enterTextPageEntered();
      // simulate a text change
      textChanged();
      break;
    case ArgsPageId:
      mmode->selectArgumentsPageEntered();
      break;
    case -1: // no id - skip it
      break;
    default:
      ;
  }
}

void TextLabelWizard::slotHelpClicked()
{
  KHelpClient::invokeHelp( "text-labels", "kig" );
}

