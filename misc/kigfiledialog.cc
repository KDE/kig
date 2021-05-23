// SPDX-FileCopyrightText: 2005 Pino Toscano <toscano.pino@tiscali.it>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kigfiledialog.h"

#include <cassert>

#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <KMessageBox>

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
    QPointer<QDialog> optdlg = new QDialog( this );
    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
    QPushButton *okButton = buttonBox->button( QDialogButtonBox::Ok );
    QVBoxLayout* mainLayout = new QVBoxLayout;
    
    okButton->setDefault( true );
    okButton->setShortcut( Qt::CTRL | Qt::Key_Return );
    optdlg->setLayout( mainLayout );
    mainLayout->addWidget( mow );
    mainLayout->addWidget( buttonBox );
    optdlg->setWindowTitle( moptcaption );
    
    connect( buttonBox, &QDialogButtonBox::accepted, optdlg.data(), &QDialog::accept );
    connect( buttonBox, &QDialogButtonBox::rejected, optdlg.data(), &QDialog::reject);

    (optdlg->exec() == QDialog::Accepted) ? QFileDialog::accept() : QFileDialog::reject();
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

