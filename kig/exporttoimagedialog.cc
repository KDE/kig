// exporttoimagedialog.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "exporttoimagedialog.h"
#include "exporttoimagedialog.moc"

#include "kig_view.h"

#include <knuminput.h>
#include <kpushbutton.h>
#include <kurlrequester.h>

ExportToImageDialog::ExportToImageDialog( KigView* v, KigDocument* d )
  : ExportToImageDialogBase( v, "Export to image dialog", true ),
    mv( v ), md( d )
{
  WidthInput->setValue( v->realWidget()->size().width() );
  HeightInput->setValue( v->realWidget()->size().height() );

  URLRequester->setFilter( // TODO
    "*" );
  URLRequester->setMode( KFile::File | KFile::LocalOnly );

  connect( OKButton, SIGNAL( clicked() ), this, SLOT( slotOKPressed() ) );
  connect( CancelButton, SIGNAL( clicked() ), this, SLOT( slotCancelPressed() ) );
  connect( WidthInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotWidthChanged( int ) ) );
  connect( HeightInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotHeightChanged( int ) ) );
}

void ExportToImageDialog::slotOKPressed()
{
  // TODO..
}

void ExportToImageDialog::slotCancelPressed()
{
  // TODO..
}

void ExportToImageDialog::slotWidthChanged( int )
{
  // TODO...
}

void ExportToImageDialog::slotHeightChanged( int )
{
  // TODO...
}

ExportToImageDialog::~ExportToImageDialog()
{

}
