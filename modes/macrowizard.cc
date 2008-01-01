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

#include "macrowizard.h"
#include "macrowizard.moc"

#include "macro.h"

#include <qlabel.h>
#include <qlayout.h>

#include <kdebug.h>
#include <kdialog.h>
#include <kicondialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <ktoolinvocation.h>

class GivenArgsPage
  : public QWizardPage
{
public:
  GivenArgsPage( QWidget* parent, DefineMacroMode* mode );

  bool isComplete() const;

  void setChanged();

private:
  DefineMacroMode* mmode;
};

GivenArgsPage::GivenArgsPage( QWidget* parent, DefineMacroMode* mode )
  : QWizardPage( parent ), mmode( mode )
{
  setTitle( i18n( "Given Objects" ) );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  QVBoxLayout* lay = new QVBoxLayout( this );
  lay->setMargin( 0 );
  QLabel* label = new QLabel( this );
  lay->addWidget( label );
  label->setText( i18n( "Select the \"given\" objects for your new macro and press \"Next\"." ) );
  label->setAlignment( Qt::AlignCenter );
}

bool GivenArgsPage::isComplete() const
{
  return mmode->hasGivenArgs();
}

void GivenArgsPage::setChanged()
{
  emit completeChanged();
}


class FinalArgsPage
  : public QWizardPage
{
public:
  FinalArgsPage( QWidget* parent, DefineMacroMode* mode );

  bool isComplete() const;
  bool validatePage();

  void setChanged();

private:
  DefineMacroMode* mmode;
};

FinalArgsPage::FinalArgsPage( QWidget* parent, DefineMacroMode* mode )
  : QWizardPage( parent ), mmode( mode )
{
  setTitle( i18n( "Final Object" ) );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  QVBoxLayout* lay = new QVBoxLayout( this );
  lay->setMargin( 0 );
  QLabel* label = new QLabel( this );
  lay->addWidget( label );
  label->setText( i18n( "Select the final object(s) for your new macro." ) );
  label->setAlignment( Qt::AlignCenter );
}

bool FinalArgsPage::isComplete() const
{
  return mmode->hasFinalArgs();
}

bool FinalArgsPage::validatePage()
{
  return mmode->validateObjects();
}

void FinalArgsPage::setChanged()
{
  emit completeChanged();
}


class MacroInfoPage
  : public QWizardPage
{
public:
  MacroInfoPage( QWidget* parent, DefineMacroMode* mode );

private:
  DefineMacroMode* mmode;
};

MacroInfoPage::MacroInfoPage( QWidget* parent, DefineMacroMode* mode )
  : QWizardPage( parent ), mmode( mode )
{
  setTitle( i18n( "Name" ) );
  setSubTitle( i18n( "Enter a name and description for your new type." ) );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  setFinalPage( true );

  QGridLayout* lay = new QGridLayout( this );
  lay->setMargin( 0 );
  QLabel* label = new QLabel( this );
  lay->addWidget( label, 0, 0 );
  label->setText( i18n( "&Name:" ) );
  label->setAlignment( Qt::AlignRight );
  KLineEdit* editname = new KLineEdit( this );
  lay->addWidget( editname, 0, 1 );
  label->setBuddy( editname );
  label = new QLabel( this );
  lay->addWidget( label, 1, 0 );
  label->setText( i18n( "&Description:" ) );
  label->setAlignment( Qt::AlignRight );
  KLineEdit* editdesc = new KLineEdit( this );
  lay->addWidget( editdesc, 1, 1 );
  label->setBuddy( editdesc );
  label = new QLabel( this );
  label->setText( i18n( "&Icon:" ) );
  label->setAlignment( Qt::AlignRight );
  lay->addWidget( label, 2, 0 );
  QHBoxLayout* iconlay = new QHBoxLayout();
  lay->addLayout( iconlay, 2, 1 );
  KIconButton* iconbutton = new KIconButton( this );
  iconlay->addWidget( iconbutton );
  label->setBuddy( iconbutton );
  iconbutton->setIcon( "system-run" );
  iconlay->addItem( new QSpacerItem( 5, 5, QSizePolicy::Expanding, QSizePolicy::Fixed ) );

  registerField( "name*", editname );
  registerField( "description", editdesc );
  registerField( "icon", iconbutton, "icon", SIGNAL( iconChanged( const QString& ) ) );
}


MacroWizard::MacroWizard( QWidget* parent, DefineMacroMode* m )
  : QWizard( parent ), mmode( m )
{
  setModal( false );
  setObjectName( QLatin1String( "Define Macro Wizard" ) );
  setWindowTitle( KDialog::makeStandardCaption( i18n( "Define New Macro" ) ) );
  setOption( HaveHelpButton );

  mgivenArgsPage = new GivenArgsPage( this, mmode );
  setPage( GivenArgsPageId, mgivenArgsPage );
  mfinalArgsPage = new FinalArgsPage( this, mmode );
  setPage( FinalArgsPageId, mfinalArgsPage );
  setPage( MacroInfoPageId, new MacroInfoPage( this, mmode ) );

  connect( this, SIGNAL( helpRequested() ), this,
           SLOT( slotHelpClicked() ) );
  connect( this, SIGNAL( currentIdChanged( int ) ), this, SLOT( currentIdChanged( int ) ) );
}

MacroWizard::~MacroWizard()
{
}

void MacroWizard::reject()
{
  QWizard::reject();
  mmode->cancelPressed();
}

void MacroWizard::accept()
{
  QWizard::accept();
  mmode->finishPressed();
}

void MacroWizard::slotHelpClicked()
{
  KToolInvocation::invokeHelp( "defining-macros", "kig" );
}

void MacroWizard::currentIdChanged( int id )
{
  switch ( id )
  {
    case GivenArgsPageId:
      mmode->givenPageEntered();
      break;
    case FinalArgsPageId:
      mmode->finalPageEntered();
      break;
    case -1: // no id - skip it
      break;
    default:
      ;
  }
}

void MacroWizard::givenArgsChanged()
{
  mgivenArgsPage->setChanged();
}

void MacroWizard::finalArgsChanged()
{
  mfinalArgsPage->setChanged();
}
