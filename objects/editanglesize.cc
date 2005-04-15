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

#include "editanglesize.h"
#include "editanglesize.moc"

#include <kcombobox.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

EditAngleSize::EditAngleSize( QWidget* parent, double angle, Goniometry::System system )
  : EditAngleSizeBase( parent, "editsizeangle", true ), mang( angle, system ), mang_orig( angle, system )
{
  // improving GUI look'n'feel...
  buttonOk->setGuiItem( KStdGuiItem::ok() );
  buttonCancel->setGuiItem( KStdGuiItem::cancel() );

  comboSystem->insertStringList( Goniometry::systemList() );

  comboSystem->setCurrentItem( system );
  editAngle->setText( QString::number( angle ) );
  isnum = true;
}

EditAngleSize::~EditAngleSize()
{
}

void EditAngleSize::okSlot()
{
  double dang = mang.getValue( Goniometry::Rad );
  double dang_orig = mang_orig.getValue( Goniometry::Rad );
  done( dang != dang_orig );
}

void EditAngleSize::cancelSlot()
{
  done( 0 );
}

void EditAngleSize::activatedSlot( int index )
{
  if ( isnum )
  {
    Goniometry::System newsys = Goniometry::intToSystem( index );
    if ( newsys == mang_orig.system() )
      mang = mang_orig;
    else
      mang.convertTo( newsys );
    editAngle->setText( QString::number( mang.value() ) );
  }
}

void EditAngleSize::textChangedSlot( const QString& txt )
{
  if ( txt.isNull() )
    isnum = false;
  else
  {
    double d = txt.toDouble( &isnum );
    mang.setValue( d );
  }
  buttonOk->setEnabled( isnum );
}

const double EditAngleSize::angle() const
{
  return mang.value();
}

const Goniometry::System EditAngleSize::system() const
{
  return mang.system();
}
