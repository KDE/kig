// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>

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

#include "exporttosvgdialog.h"
#include "exporttosvgdialog.moc"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

#include <qcheckbox.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kurlrequester.h>

ExportToSVGDialog::ExportToSVGDialog( KigWidget* v, const KigPart* part )
  : ExportToSVGDialogBase( v, "Export to SVG dialog", true ),
    mv( v ), mpart( part )
{
  KIconLoader* l = part->instance()->iconLoader();
  OKButton->setIconSet( QIconSet( l->loadIcon( "button_ok", KIcon::Small ) ) );
  CancelButton->setIconSet( QIconSet( l->loadIcon( "button_cancel", KIcon::Small ) ) );

  showGridCheckBox->setChecked( part->document().grid() );
  showAxesCheckBox->setChecked( part->document().axes() );

  QString formats = i18n( "*.svg|Scalable Vector Graphics (*.svg)" );

  URLRequester->setFilter( formats );
  URLRequester->setMode( KFile::File | KFile::LocalOnly );
  URLRequester->setCaption( i18n( "Export as SVG" ) );

  connect( OKButton, SIGNAL( clicked() ), this, SLOT( slotOKPressed() ) );
  connect( CancelButton, SIGNAL( clicked() ), this, SLOT( slotCancelPressed() ) );
}

void ExportToSVGDialog::slotOKPressed()
{
  QString filename = URLRequester->url();
  if ( filename.isEmpty() )
  {
    KMessageBox::sorry( mv, i18n( "Please enter a file name." ) );
    return;
  };
  QFile file( filename );
  if ( file.exists() )
  {
    int ret = KMessageBox::warningYesNo( mv,
                                         i18n( "The file \"%1\" already exists. Do you wish to overwrite it?" )
                                         .arg( filename ), i18n( "Overwrite File?" ) );
    if ( ret != KMessageBox::Yes ) return;
  };

/*
  if ( ! file.open( IO_WriteOnly ) )
  {
    KMessageBox::sorry( mv,
                        i18n( "The file \"%1\" could not be opened. Please "
                              "check if the file permissions are set correctly." )
                        .arg( filename ) );
    return;
  };
*/
  accept();
}

void ExportToSVGDialog::slotCancelPressed()
{
  reject();
}

ExportToSVGDialog::~ExportToSVGDialog()
{
}

QString ExportToSVGDialog::fileName()
{
  return URLRequester->url();
}

bool ExportToSVGDialog::showAxes()
{
  return showAxesCheckBox->isOn();
}

bool ExportToSVGDialog::showGrid()
{
  return showGridCheckBox->isOn();
}
