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

#include "exporttolatexdialog.h"
#include "exporttolatexdialog.moc"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

#include <qcheckbox.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kurlrequester.h>

ExportToLatexDialog::ExportToLatexDialog( KigWidget* v, const KigPart* part )
  : ExportToLatexDialogBase( v, "Export to latex dialog", true ),
    mv( v ), mpart( part )
{
  KIconLoader* l = part->instance()->iconLoader();
  OKButton->setIconSet( QIconSet( l->loadIcon( "button_ok", KIcon::Small ) ) );
  CancelButton->setIconSet( QIconSet( l->loadIcon( "button_cancel", KIcon::Small ) ) );

  showGridCheckBox->setChecked( part->document().grid() );
  showAxesCheckBox->setChecked( part->document().axes() );
  showExtraFrameCheckBox->setChecked( false );

  QString formats = i18n( "*.tex|Latex Documents (*.tex)" );

  URLRequester->setFilter( formats );
  URLRequester->setMode( KFile::File | KFile::LocalOnly );
  URLRequester->setCaption( i18n( "Export as Latex" ) );

  connect( OKButton, SIGNAL( clicked() ), this, SLOT( slotOKPressed() ) );
  connect( CancelButton, SIGNAL( clicked() ), this, SLOT( slotCancelPressed() ) );
}

void ExportToLatexDialog::slotOKPressed()
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

  accept();
}

void ExportToLatexDialog::slotCancelPressed()
{
  reject();
}

ExportToLatexDialog::~ExportToLatexDialog()
{
}

QString ExportToLatexDialog::fileName()
{
  return URLRequester->url();
}

bool ExportToLatexDialog::showAxes()
{
  return showAxesCheckBox->isOn();
}

bool ExportToLatexDialog::showGrid()
{
  return showGridCheckBox->isOn();
}

bool ExportToLatexDialog::extraFrame()
{
  return showExtraFrameCheckBox->isOn();
}
