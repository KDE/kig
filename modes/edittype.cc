/*
   This file is part of Kig, a KDE program for Interactive Geometry...
   Copyright (C) 2004  Dominique Devriese <devriese@kde.org>
   Copyright (C) 2004  Pino Toscano <toscano.pino@tiscali.it>

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

#include "edittype.h"
#include "edittype.moc"

#include "ui_edittypewidget.h"

#include <kicondialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>

EditType::EditType( QWidget* parent, const QString& name, const QString& desc,
                    const QString& icon )
  : KDialog( parent ),
    mname( name ), mdesc( desc ), micon( icon )
{
  setCaption( i18n( "Edit Type" ) );
  setButtons( Help | Ok | Cancel );

  QWidget* base = new QWidget( this );
  setMainWidget( base );
  medittypewidget = new Ui_EditTypeWidget();
  medittypewidget->setupUi( base );
  base->layout()->setMargin( 0 );

  medittypewidget->editName->setText( mname );
  medittypewidget->editName->setWhatsThis(
        i18n( "Here you can edit the name of the current macro type." ) );
  medittypewidget->editDescription->setText( mdesc );
  medittypewidget->editDescription->setWhatsThis(
        i18n( "Here you can edit the description of the current macro type. "
              "This field is optional, so you can also leave this empty: if "
              "you do so, then your macro type will have no description." ) );
  medittypewidget->typeIcon->setIcon( !micon.isEmpty() ? micon : "system-run" );
  medittypewidget->typeIcon->setWhatsThis(
        i18n( "Use this button to change the icon of the current macro type." ) );

  connect( this, SIGNAL( helpClicked() ), this, SLOT( slotHelp() ) );
  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
  connect( this, SIGNAL( cancelClicked() ), this, SLOT( slotCancel() ) );

  resize( 450, 150 );
}

EditType::~EditType()
{
  delete medittypewidget;
}

void EditType::slotHelp()
{
  KToolInvocation::invokeHelp( "working-with-types", "kig" );
}

void EditType::slotOk()
{
  QString tmp = medittypewidget->editName->text();
  if ( tmp.isEmpty() )
  {
    KMessageBox::information( this, i18n( "The name of the macro can not be empty." ) );
    return;
  }

  bool namechanged = false;
  bool descchanged = false;
  bool iconchanged = false;
  if ( tmp != mname )
  {
    mname = tmp;
    namechanged = true;
  }
  tmp = medittypewidget->editDescription->text();
  if ( tmp != mdesc )
  {
    mdesc = tmp;
    descchanged = true;
  }
  tmp = medittypewidget->typeIcon->icon();
  if ( tmp != micon )
  {
    micon = tmp;
    iconchanged = true;
  }
  done( namechanged || descchanged || iconchanged );
}

void EditType::slotCancel()
{
  done( 0 );
}

QString EditType::name() const
{
  return mname;
}

QString EditType::description() const
{
  return mdesc;
}

QString EditType::icon() const
{
  return micon;
}
