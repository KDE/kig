/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2006 Pino Toscano <toscano.pino@tiscali.it>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "historydialog.h"


#include "ui_historywidget.h"

#include <QIntValidator>
#include <QDebug>
#include <QDialogButtonBox>
#include <QIcon>
#include <QPushButton>
#include <QUndoStack>
#include <QVBoxLayout>

#include <KConfigGroup>

HistoryDialog::HistoryDialog( QUndoStack* kch, QWidget* parent )
  : QDialog( parent ), mch( kch )
{
  setWindowTitle( i18nc("@title:window", "History Browser") );
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
  QWidget *mainWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);
  mainLayout->addWidget(mainWidget);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  //PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
  mainLayout->addWidget(buttonBox);

  QWidget* main = new QWidget( this );
  mwidget = new Ui_HistoryWidget();
  mwidget->setupUi( main );
//PORTING: Verify that widget was added to mainLayout   setMainWidget( main );

  mtotalsteps = mch->count() + 1;

  bool reversed = layoutDirection() == Qt::RightToLeft;

  mwidget->buttonFirst->setIcon( QIcon::fromTheme( reversed ? "go-last" : "go-first" ) );
  connect( mwidget->buttonFirst, &QAbstractButton::clicked, this, &HistoryDialog::goToFirst );

  mwidget->buttonBack->setIcon( QIcon::fromTheme( reversed ? "go-next" : "go-previous" ) );
  connect( mwidget->buttonBack, &QAbstractButton::clicked, this, &HistoryDialog::goBack );

  mwidget->editStep->setValidator( new QIntValidator( 1, mtotalsteps, mwidget->editStep ) );
  mwidget->labelSteps->setText( QString::number( mtotalsteps ) );

  mwidget->buttonNext->setIcon( QIcon::fromTheme( reversed ? "go-previous" : "go-next" ) );
  connect( mwidget->buttonNext, &QAbstractButton::clicked, this, &HistoryDialog::goToNext );

  mwidget->buttonLast->setIcon( QIcon::fromTheme( reversed ? "go-first" : "go-last" ) );
  connect( mwidget->buttonLast, &QAbstractButton::clicked, this, &HistoryDialog::goToLast );

  updateWidgets();

  resize( 400, 200 );
}

HistoryDialog::~HistoryDialog()
{
  delete mwidget;
}

void HistoryDialog::goToFirst()
{
  int undosteps = mch->index();
  for ( int i = 0; i < undosteps; ++i )
  {
    mch->undo();
  }

  updateWidgets();
}

void HistoryDialog::goBack()
{
  mch->undo();

  updateWidgets();
}

void HistoryDialog::goToNext()
{
  mch->redo();

  updateWidgets();
}

void HistoryDialog::goToLast()
{
  int redosteps = mch->count() - mch->index();
  for ( int i = 0; i < redosteps; ++i )
  {
    mch->redo();
  }

  updateWidgets();
}

void HistoryDialog::updateWidgets()
{
  int currentstep = mch->index() + 1;

  mwidget->editStep->setText( QString::number( currentstep ) );
  if ( mch->index() > 0 )
  {
    mwidget->description->setPlainText( mch->text( mch->index() - 1 ) );
  }
  else
  {
    mwidget->description->setPlainText( i18n( "Start of the construction" ) );
  }

  bool notfirst = currentstep > 1;
  bool notlast = currentstep < mtotalsteps;
  mwidget->buttonFirst->setEnabled( notfirst );
  mwidget->buttonBack->setEnabled( notfirst );
  mwidget->buttonNext->setEnabled( notlast );
  mwidget->buttonLast->setEnabled( notlast );
}
