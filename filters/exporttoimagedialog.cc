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

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

#include "../misc/kigpainter.h"

#include <qcheckbox.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kurlrequester.h>
#include <kimageio.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <klocale.h>

ExportToImageDialog::ExportToImageDialog( KigWidget* v, const KigPart* part )
  : ExportToImageDialogBase( v, "Export to image dialog", true ),
    mv( v ), mpart( part ), msize( v->size() ), minternallysettingstuff( false )
{
  KIconLoader* l = part->instance()->iconLoader();
  OKButton->setIconSet( QIconSet( l->loadIcon( "button_ok", KIcon::Small ) ) );
  CancelButton->setIconSet( QIconSet( l->loadIcon( "button_cancel", KIcon::Small ) ) );

  WidthInput->setValue( msize.width() );
  HeightInput->setValue( msize.height() );

  static bool kimageioRegistered = false;
  if ( ! kimageioRegistered )
  {
    KImageIO::registerFormats();
    kimageioRegistered = true;
  };

  URLRequester->setFilter( KImageIO::pattern( KImageIO::Writing ) );
  URLRequester->setMode( KFile::File | KFile::LocalOnly );
  URLRequester->setCaption( i18n( "Export to Image" ) );

  connect( OKButton, SIGNAL( clicked() ), this, SLOT( slotOKPressed() ) );
  connect( CancelButton, SIGNAL( clicked() ), this, SLOT( slotCancelPressed() ) );
  connect( WidthInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotWidthChanged( int ) ) );
  connect( HeightInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotHeightChanged( int ) ) );
}

void ExportToImageDialog::slotOKPressed()
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
                                         .arg( filename ), i18n( "Overwrite file?" ) );
    if ( ret != KMessageBox::Yes ) return;
  };

  if ( ! file.open( IO_WriteOnly ) )
  {
    KMessageBox::sorry( mv,
                        i18n( "The file \"%1\" could not be opened. Please check if the file permissions are set correctly." )
                        .arg( filename ) );
    return;
  };

  QString type = KImageIO::type( filename );
  if ( type.isNull() )
  {
    KMessageBox::sorry( mv,
                        i18n( "Sorry, this file format is not supported." ) );
    return;
  };

  kdDebug() << type << endl;

  QPixmap img( QSize( WidthInput->value(), HeightInput->value() ) );
  img.fill( Qt::white );
  KigPainter p( ScreenInfo( mv->screenInfo().shownRect(), img.rect() ), &img, mpart->document());
  p.setWholeWinOverlay();
  p.drawGrid( mpart->document().coordinateSystem(), showgridCheckBox->isOn(), showAxesCheckBox->isOn() );
  // FIXME: show the selections ?
  p.drawObjects( mpart->document().objects(), false );
  if ( ! img.save( filename, type.latin1() ) )
  {
    KMessageBox::error( mv, i18n( "Sorry, something went wrong while saving to image \"%1\"" ).arg( filename ) );
    return;
  }
  else accept();
}

void ExportToImageDialog::slotCancelPressed()
{
  reject();
}

void ExportToImageDialog::slotWidthChanged( int w )
{
  if ( ! minternallysettingstuff )
  {
    minternallysettingstuff = true;
    HeightInput->setValue( w * msize.height() / msize.width() );
    minternallysettingstuff = false;
  };
}

void ExportToImageDialog::slotHeightChanged( int h )
{
  if ( ! minternallysettingstuff )
  {
    minternallysettingstuff = true;
    WidthInput->setValue( h * msize.width() / msize.height() );
    minternallysettingstuff = false;
  };
}

ExportToImageDialog::~ExportToImageDialog()
{

}
