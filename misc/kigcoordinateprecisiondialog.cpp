/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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


