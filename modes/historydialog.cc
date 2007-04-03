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
#include <kicon.h>
#include <klineedit.h>
#include <klocale.h>
#include <knumvalidator.h>
#include <kpushbutton.h>

HistoryDialog::HistoryDialog( QUndoStack* kch, QWidget* parent )
  : KDialog( parent ), mch( kch )
{
  setCaption( i18n( "History Browser" ) );
  setButtons( Close );

  QWidget* main = new QWidget( this );
  mwidget = new Ui_HistoryWidget();
  mwidget->setupUi( main );
  setMainWidget( main );

  mtotalsteps = mch->count() + 1;

  mwidget->buttonFirst->setIcon( KIcon( "arrow-left-double" ) );
  connect( mwidget->buttonFirst, SIGNAL( clicked() ), this, SLOT( goToFirst() ) );

  mwidget->buttonBack->setIcon( KIcon( "arrow-left" ) );
  connect( mwidget->buttonBack, SIGNAL( clicked() ), this, SLOT( goBack() ) );

  mwidget->editStep->setValidator( new KIntValidator( 1, mtotalsteps, mwidget->editStep ) );
  mwidget->labelSteps->setText( QString::number( mtotalsteps ) );

  mwidget->buttonNext->setIcon( KIcon( "arrow-right" ) );
  connect( mwidget->buttonNext, SIGNAL( clicked() ), this, SLOT( goToNext() ) );

  mwidget->buttonLast->setIcon( KIcon( "arrow-right-double" ) );
  connect( mwidget->buttonLast, SIGNAL( clicked() ), this, SLOT( goToLast() ) );

  updateWidgets();

  resize( 400, 200 );
}

HistoryDialog::~HistoryDialog()
{
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
