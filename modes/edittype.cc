/**
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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
   USA
**/


#include "edittype.h"
#include "edittype.moc"

#include <kapplication.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kpushbutton.h>

EditType::EditType( QWidget* parent, QString name, QString desc, QString icon )
  : EditTypeBase( parent, "edittype", true ), mname( name ), mdesc( desc ), micon( icon )
{
  // improving GUI look'n'feel...
  il = KGlobal::iconLoader();
  buttonHelp->setIconSet( QIconSet( il->loadIcon( "help", KIcon::Small ) ) );
  buttonOk->setIconSet( QIconSet( il->loadIcon( "button_ok", KIcon::Small ) ) );
  buttonCancel->setIconSet( QIconSet( il->loadIcon( "button_cancel", KIcon::Small ) ) );

  editName->setText( mname );
  editDescription->setText( mdesc );
  typeIcon->setIcon( !micon.isNull() ? micon : "gear" );
}

EditType::~EditType()
{
}

void EditType::helpSlot()
{
  kapp->invokeHelp( QString::fromLatin1( "working-with-types" ),
                    QString::fromLatin1( "kig" ) );
}

void EditType::okSlot()
{
  bool namechanged = false;
  bool descchanged = false;
  bool iconchanged = false;
  QString tmp = editName->text();
  if ( tmp != mname )
  {
    mname = tmp;
    namechanged = true;
  }
  tmp = editDescription->text();
  if ( tmp != mdesc )
  {
    mdesc = tmp;
    descchanged = true;
  }
  tmp = typeIcon->icon();
  if ( tmp != micon )
  {
    micon = tmp;
    iconchanged = true;
  }
  done( namechanged || descchanged || iconchanged );
}

void EditType::cancelSlot()
{
  done( 0 );
}

const QString EditType::name() const
{
  return mname;
}

const QString EditType::description() const
{
  return mdesc;
}

const QString EditType::icon() const
{
  return micon;
}
