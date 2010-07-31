// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>
// Copyright (C)  2004  Dominique Devriese <devriese@kde.org>

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

#include "drgeo-filter-chooser.h"
#include "drgeo-filter-chooser.moc"

#include <klistbox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

KigFilterDrgeoChooser::KigFilterDrgeoChooser( const TQStringList& l )
  : KigFilterDrgeoChooserBase( 0, "drgeo_filter", true )
{
  OKButton->setGuiItem( KStdGuiItem::ok() );
  CancelButton->setGuiItem( KStdGuiItem::cancel() );

  FigureListBox->insertStringList( l );

  connect( OKButton, TQT_SIGNAL( clicked() ), TQT_SLOT( slotOKPressed() ) );
  connect( CancelButton, TQT_SIGNAL( clicked() ), TQT_SLOT( slotCancelPressed() ) );
  connect( FigureListBox, TQT_SIGNAL( executed( TQListBoxItem* ) ), TQT_SLOT( slotExecuted( TQListBoxItem* ) ) );
}

void KigFilterDrgeoChooser::slotOKPressed()
{
  const int r = FigureListBox->currentItem();
  if ( r == -1 )
  {
    KMessageBox::sorry( 0, i18n( "Please select a figure." ) );
    return;
  }
  done( r );
}

void KigFilterDrgeoChooser::slotCancelPressed()
{
  done( -1 );
}

KigFilterDrgeoChooser::~KigFilterDrgeoChooser()
{

}

void KigFilterDrgeoChooser::slotExecuted( TQListBoxItem* i )
{
  done( FigureListBox->index( i ) );
}
