/*
   This file is part of Kig, a KDE program for Interactive Geometry...
   Copyright (C) 2006  Pino Toscano <toscano.pino@tiscali.it>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
   USA
*/

#include "historydialog.h"
#include "historydialog.moc"

#include "ui_historywidget.h"

#include <qlabel.h>
#include <qtextedit.h>
#include <qtimer.h>
#include <qundostack.h>

#include <kdebug.h>
#include <QIcon>
#include <klineedit.h>
#include <klocale.h>
#include <knumvalidator.h>
#include <kpushbutton.h>
#include <QDialogButtonBox>
#include <KConfigGroup>
#include <QPushButton>
#include <QVBoxLayout>

HistoryDialog::HistoryDialog( QUndoStack* kch, QWidget* parent )
  : QDialog( parent ), mch( kch )
{
  setWindowTitle( i18n( "History Browser" ) );
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
  QWidget *mainWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);
  mainLayout->addWidget(mainWidget);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  //PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
  mainLayout->addWidget(buttonBox);

  QWidget* main = new QWidget( this );
  mwidget = new Ui_HistoryWidget();
  mwidget->setupUi( main );
//PORTING: Verify that widget was added to mainLayout   setMainWidget( main );

  mtotalsteps = mch->count() + 1;

  bool reversed = layoutDirection() == Qt::RightToLeft;

  mwidget->buttonFirst->setIcon( QIcon::fromTheme( reversed ? "go-last" : "go-first" ) );
  connect( mwidget->buttonFirst, SIGNAL( clicked() ), this, SLOT( goToFirst() ) );

  mwidget->buttonBack->setIcon( QIcon::fromTheme( reversed ? "go-next" : "go-previous" ) );
  connect( mwidget->buttonBack, SIGNAL( clicked() ), this, SLOT( goBack() ) );

  mwidget->editStep->setValidator( new KIntValidator( 1, mtotalsteps, mwidget->editStep ) );
  mwidget->labelSteps->setText( QString::number( mtotalsteps ) );

  mwidget->buttonNext->setIcon( QIcon::fromTheme( reversed ? "go-previous" : "go-next" ) );
  connect( mwidget->buttonNext, SIGNAL( clicked() ), this, SLOT( goToNext() ) );

  mwidget->buttonLast->setIcon( QIcon::fromTheme( reversed ? "go-first" : "go-last" ) );
  connect( mwidget->buttonLast, SIGNAL( clicked() ), this, SLOT( goToLast() ) );

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
