// Copyright (C)  2005  Pino Toscano       <toscano.pino@tiscali.it>

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
// USA

#include "kigfiledialog.h"
#include "kigfiledialog.moc"

#include <QVBoxLayout>
#include <QPushButton>
#include <qfile.h>

#include <klocale.h>
#include <kmessagebox.h>

#include <cassert>

KigFileDialog::KigFileDialog( const QString& startDir, const QString& filter,
              const QString& caption,  QWidget* parent )
  : QFileDialog( parent, caption, startDir, filter ),
    mow( 0L )
{
  setAcceptMode( QFileDialog::AcceptSave );
  setFileMode( QFileDialog::AnyFile );
  moptcaption = i18n( "Options" );
}

void KigFileDialog::setOptionsWidget( QWidget* w )
{
  mow = w;
}

void KigFileDialog::accept()
{
  // i know this is an ugly hack, but i hadn't found other ways to get
  // the selected file name _before_ the dialog is accept()'ed or
  // reject()'ed... in every case, below we make sure to accept() or
  // reject()...
  setResult( QDialog::Accepted );

  QString sFile = selectedFile();

  if ( QFile::exists( sFile ) )
  {
    int ret = KMessageBox::warningContinueCancel( this,
                                         i18n( "The file \"%1\" already exists. Do you wish to overwrite it?" ,
                                           sFile ), i18n( "Overwrite File?" ), KStandardGuiItem::overwrite() );
    if ( ret != KMessageBox::Continue )
    {
      QFileDialog::reject();
      return;
    }
  }
  if ( mow )
  {
    QDialog* optdlg = new QDialog( this );
    QWidget* mainWidget = new QWidget( optdlg );
    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
    QPushButton *okButton = buttonBox->button( QDialogButtonBox::Ok );
    QVBoxLayout* mainLayout = new QVBoxLayout;
    
    okButton->setDefault( true );
    okButton->setShortcut( Qt::CTRL | Qt::Key_Return );
    optdlg->setLayout( mainLayout );
    mainLayout->addWidget( mainWidget );
    mainLayout->addWidget( buttonBox );
    optdlg->setWindowTitle( moptcaption );
    
    connect( buttonBox, SIGNAL( accepted() ), optdlg, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ));

    mow->setParent( optdlg );
//PORTING: Verify that widget was added to mainLayout     optdlg->setMainWidget( mow );
    optdlg->exec() == QDialog::Accepted ? QFileDialog::accept() : QFileDialog::reject();
  }
  else
    QFileDialog::accept();
}

void KigFileDialog::setOptionCaption( const QString& caption )
{
  if ( caption.isEmpty() )
    return;

  moptcaption = caption;
}

QString KigFileDialog::selectedFile()
{
  QStringList files = selectedFiles();

  assert( files.size() == 1 );

  return files[0];
}

