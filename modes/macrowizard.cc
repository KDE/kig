// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "macrowizard.h"


#include "macro.h"

#include <QLabel>
#include <QLayout>

#include <QDebug>
#include <QDialog>
#include <QLineEdit>

#include <KHelpClient>
#include <KIconButton>
#include <KLocalizedString>

class GivenArgsPage
  : public QWizardPage
{
public:
  GivenArgsPage( QWidget* parent, DefineMacroMode* mode );

  bool isComplete() const Q_DECL_OVERRIDE;

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
  lay->setContentsMargins( 0 ,  0 ,  0 ,  0 );
  QLabel* label = new QLabel( this );
  lay->addWidget( label );
  label->setText( i18n( "Select the \"given\" objects for your new macro and press \"Next\"." ) );
  label->setAlignment( Qt::AlignCenter );
}

bool GivenArgsPage::isComplete() const
{
  return true;
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

  bool isComplete() const Q_DECL_OVERRIDE;
  bool validatePage() Q_DECL_OVERRIDE;

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
  lay->setContentsMargins( 0 ,  0 ,  0 ,  0 );
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
  MacroInfoPage( QWidget* parent );
};

MacroInfoPage::MacroInfoPage( QWidget* parent )
  : QWizardPage( parent )
{
  setTitle( i18n( "Name" ) );
  setSubTitle( i18n( "Enter a name and description for your new type." ) );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  setFinalPage( true );

  //TODO Make this a ui file
  QGridLayout* lay = new QGridLayout( this );
  lay->setContentsMargins( 0 ,  0 ,  0 ,  0 );
  QLabel* label = new QLabel( this );
  lay->addWidget( label, 0, 0 );
  label->setText( i18n( "&Name:" ) );
  label->setAlignment( Qt::AlignRight );
  QLineEdit* editname = new QLineEdit( this );
  lay->addWidget( editname, 0, 1 );
  label->setBuddy( editname );
  label = new QLabel( this );
  lay->addWidget( label, 1, 0 );
  label->setText( i18n( "&Description:" ) );
  label->setAlignment( Qt::AlignRight );
  QLineEdit* editdesc = new QLineEdit( this );
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
  iconbutton->setIcon( QStringLiteral("system-run") );
  iconlay->addItem( new QSpacerItem( 5, 5, QSizePolicy::Expanding, QSizePolicy::Fixed ) );

  registerField( QStringLiteral("name*"), editname );
  registerField( QStringLiteral("description"), editdesc );
  registerField( QStringLiteral("icon"), iconbutton, "icon", SIGNAL(iconChanged(QString)) );
}


MacroWizard::MacroWizard( QWidget* parent, DefineMacroMode* m )
  : QWizard( parent ), mmode( m )
{
  setModal( false );
  setObjectName( QStringLiteral( "Define Macro Wizard" ) );
  setWindowTitle( i18nc("@title:window", "Define New Macro") );
  setOption( HaveHelpButton );

  mgivenArgsPage = new GivenArgsPage( this, mmode );
  setPage( GivenArgsPageId, mgivenArgsPage );
  mfinalArgsPage = new FinalArgsPage( this, mmode );
  setPage( FinalArgsPageId, mfinalArgsPage );
  setPage( MacroInfoPageId, new MacroInfoPage( this ) );

  connect( this, &QWizard::helpRequested, this,
           &MacroWizard::slotHelpClicked );
  connect( this, SIGNAL(currentIdChanged(int)), this, SLOT(currentIdChanged(int)) );
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
  KHelpClient::invokeHelp( QStringLiteral("defining-macros"), QStringLiteral("kig") );
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
