/**
   This file is part of Kig, a KDE program for Interactive Geometry...
   Copyright (C) 2004  Pino Toscano <toscano.pino@tiscali.it>
   Copyright (C) 2004  Dominique Devriese <devriese@kde.org>

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


#include "zoomarea.h"
#include "zoomarea.moc"

#include "../misc/coordinate_system.h"

#include <qlabel.h>

#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

ZoomArea::ZoomArea( QWidget* parent, KigDocument& doc )
  : ZoomAreaBase( parent, "zoomarea", true ), mdoc( doc )
{
  buttonOk->setGuiItem( KStdGuiItem::ok() );
  buttonCancel->setGuiItem( KStdGuiItem::cancel() );

  labelDescription->setText( i18n( "Select the zoom area by entering the coordinates of\n"
                                   "the upper left corner and the lower right corner." ) +
                             QString::fromLatin1("\n") +
                             mdoc.coordinateSystem().coordinateFormatNotice() );

  mvtor = mdoc.coordinateSystem().coordinateValidator();
  editFirstCoord->setValidator( mvtor );
  editSecondCoord->setValidator( mvtor );

  mcoord0 = Coordinate( 0, 0 );
  mcoord1 = Coordinate( 0, 0 );
  QString tempcoord = mdoc.coordinateSystem().fromScreen( mcoord0, mdoc );
  editFirstCoord->setText( tempcoord );
  editSecondCoord->setText( tempcoord );
}

ZoomArea::~ZoomArea()
{
  delete mvtor;
}

void ZoomArea::okSlot()
{
  done( 1 );
}

void ZoomArea::cancelSlot()
{
  done( 0 );
}

void ZoomArea::coordsChangedSlot( const QString& )
{
  int p = 0;
  QString t = editFirstCoord->text();
  bool ok = mvtor->validate( t, p ) == QValidator::Acceptable;
  if ( ok )
    mcoord0 = mdoc.coordinateSystem().toScreen( t, ok );
  p = 0;
  t = editSecondCoord->text();
  ok &= mvtor->validate( t, p ) == QValidator::Acceptable;
  if ( ok )
    mcoord1 = mdoc.coordinateSystem().toScreen( t, ok );

  buttonOk->setEnabled( ok );
}

Coordinate ZoomArea::coord0() const
{
  return mcoord0;
}

Coordinate ZoomArea::coord1() const
{
  return mcoord1;
}

void ZoomArea::setCoord0( Coordinate& c )
{
  mcoord0 = c;
  editFirstCoord->setText( mdoc.coordinateSystem().fromScreen( mcoord0, mdoc ) );
}

void ZoomArea::setCoord1( Coordinate& c )
{
  mcoord1 = c;
  editSecondCoord->setText( mdoc.coordinateSystem().fromScreen( mcoord1, mdoc ) );
}
