/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "kigcoordinateprecisiondialog.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

KigCoordinatePrecisionDialog::KigCoordinatePrecisionDialog(bool isUserSpecified, int currentPrecision ) : QDialog()
{
  ui = new Ui::KigCoordinatePrecisionDialog();
  QWidget *mainWidget = new QWidget(this);
  QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);
  mainLayout->addWidget(mainWidget);
  mainLayout->addWidget( buttonBox );
  ui->setupUi(mainWidget);

  ui->m_defaultCheckBox->setCheckState( isUserSpecified ? Qt::Unchecked : Qt::Checked) ;
  ui->m_precisionLabel->setEnabled( isUserSpecified );
  ui->m_precisionSpinBox->setEnabled( isUserSpecified );
  ui->m_precisionSpinBox->setValue( currentPrecision );

  connect(ui->m_defaultCheckBox, &QCheckBox::stateChanged, this, &KigCoordinatePrecisionDialog::toggleCoordinateControls);
  connect( buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
  connect( buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );

  show();
}

KigCoordinatePrecisionDialog::~KigCoordinatePrecisionDialog()
{
  delete ui;
}

int KigCoordinatePrecisionDialog::getUserSpecifiedCoordinatePrecision() const
{
  if( ui->m_defaultCheckBox->checkState() == Qt::Unchecked )
  {
    return ui->m_precisionSpinBox->value();
  }
  
  return -1;
}

void KigCoordinatePrecisionDialog::toggleCoordinateControls( int state )
{
  bool controlsEnabled = ( state == Qt::Unchecked );
  
  ui->m_precisionLabel->setEnabled( controlsEnabled );
  ui->m_precisionSpinBox->setEnabled( controlsEnabled );
}


