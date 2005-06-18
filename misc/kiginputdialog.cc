/**
   This file is part of Kig, a KDE program for Interactive Geometry...
   Copyright (C) 2005  Pino Toscano <toscano.pino@tiscali.it>

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

#include "kiginputdialog.h"
#include "kiginputdialog.moc"

#include "coordinate.h"
#include "coordinate_system.h"

#include "../kig/kig_document.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qvalidator.h>

#include <kcombobox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <ktextedit.h>

class KigInputDialogPrivate
{
public:
  KigInputDialogPrivate();

  QLabel* m_label;
  KLineEdit* m_lineEditFirst;
  KLineEdit* m_lineEditSecond;
  KComboBox* m_comboBox;
  KTextEdit* m_textEdit;

  Coordinate m_coord1;
  Coordinate m_coord2;
  KigDocument m_doc;
  QValidator* m_vtor;
};

KigInputDialogPrivate::KigInputDialogPrivate()
  : m_label( 0L ), m_lineEditFirst( 0L ), m_lineEditSecond( 0L ), m_comboBox( 0L ),
    m_textEdit( 0L )
{
}

KigInputDialog::KigInputDialog( const QString& caption, const QString& label,
      QWidget* parent, const KigDocument& doc, Coordinate* c1, Coordinate* c2 )
  : KDialogBase( parent, "kigdialog", true, caption, Ok|Cancel, Cancel, true ),
    d( new KigInputDialogPrivate() )
{
  d->m_coord1 = c1 ? Coordinate( *c1 ) : Coordinate::invalidCoord();
  d->m_coord2 = c2 ? Coordinate( *c2 ) : Coordinate::invalidCoord();
  d->m_doc = doc;
  d->m_vtor = d->m_doc.coordinateSystem().coordinateValidator();

  int deltay = 0;
  bool ok = false;

  QFrame* frame = makeMainWidget();
  QVBoxLayout* mainlay = new QVBoxLayout( frame, 0, spacingHint() );
  mainlay->activate();

  d->m_textEdit = new KTextEdit( frame );
  d->m_textEdit->setText( label );
  d->m_textEdit->setReadOnly( true );
//  d->m_textEdit->setAlignment( d->m_textEdit->alignment() | Qt::WordBreak );
  d->m_textEdit->setFrameStyle( QFrame::NoFrame );
  mainlay->addWidget( d->m_textEdit );

  d->m_lineEditFirst = new KLineEdit( frame );
//  d->m_lineEditFirst->setValidator( d->m_vtor );
  if ( d->m_coord1.valid() )
  {
    d->m_lineEditFirst->setText( d->m_doc.coordinateSystem().fromScreen( d->m_coord1, d->m_doc ) );
    ok = true;
  }
  mainlay->addWidget( d->m_lineEditFirst );

  connect( d->m_lineEditFirst, SIGNAL(textChanged(const QString&)),
           this, SLOT(slotCoordsChanged(const QString&)) );

  if ( d->m_coord2.valid() )
  {
    d->m_lineEditSecond = new KLineEdit( frame );
//    d->m_lineEditSecond->setValidator( d->m_vtor );
    d->m_lineEditSecond->setText( d->m_doc.coordinateSystem().fromScreen( d->m_coord2, d->m_doc ) );
    mainlay->addWidget( d->m_lineEditSecond );

    connect( d->m_lineEditSecond, SIGNAL(textChanged(const QString&)),
             this, SLOT(slotCoordsChanged(const QString&)) );

    deltay += d->m_lineEditSecond->height() + spacingHint();
  }

  resize( 400, 160 + deltay );

  d->m_lineEditFirst->setFocus();

  enableButtonOK( ok );
}

void KigInputDialog::slotCoordsChanged( const QString& )
{
  int p = 0;
  QString t = d->m_lineEditFirst->text();
  bool ok = d->m_vtor->validate( t, p ) == QValidator::Acceptable;
  if ( ok )
    d->m_coord1 = d->m_doc.coordinateSystem().toScreen( t, ok );
  if ( d->m_lineEditSecond )
  {
    p = 0;
    t = d->m_lineEditSecond->text();
    ok &= d->m_vtor->validate( t, p ) == QValidator::Acceptable;
    if ( ok )
      d->m_coord2 = d->m_doc.coordinateSystem().toScreen( t, ok );
  }

  enableButtonOK( ok );
}

Coordinate KigInputDialog::coordinateFirst() const
{
  return d->m_coord1;
}

Coordinate KigInputDialog::coordinateSecond() const
{
  return d->m_coord2;
}

void KigInputDialog::getCoordinate( const QString& caption, const QString& label,
      QWidget* parent, bool* ok, const KigDocument& doc, Coordinate* cvalue )
{
  getTwoCoordinates( caption, label, parent, ok, doc, cvalue, 0 );
}

void KigInputDialog::getTwoCoordinates( const QString& caption, const QString& label,
      QWidget* parent, bool* ok, const KigDocument& doc, Coordinate* cvalue,
      Coordinate* cvalue2 )
{
  KigInputDialog dlg( caption, label, parent, doc, cvalue, cvalue2 );

  *ok = ( dlg.exec() == Accepted );

  if ( *ok )
  {
    Coordinate a = dlg.coordinateFirst();
    *cvalue = a;
    if ( cvalue2 )
    {
      Coordinate b = dlg.coordinateSecond();
      *cvalue2 = b;
    }
  }

}
