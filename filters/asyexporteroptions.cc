// SPDX-FileCopyrightText: 2010, 2011 Raoul Bourquin <raoulb@bluewin.ch>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "asyexporteroptions.h"

#include "ui_asyexporteroptionswidget.h"

#include <QCheckBox>
#include <QLayout>

AsyExporterOptions::AsyExporterOptions( QWidget* parent )
  : QWidget( parent )
{
  expwidget = new Ui_AsyExporterOptionsWidget();
  expwidget->setupUi( this );

  layout()->setContentsMargins( 0, 0, 0, 0 );
}

AsyExporterOptions::~AsyExporterOptions()
{
  delete expwidget;
}

void AsyExporterOptions::setGrid( bool grid )
{
  expwidget->showGridCheckBox->setChecked( grid );
}

bool AsyExporterOptions::showGrid() const
{
  return expwidget->showGridCheckBox->isChecked();
}

void AsyExporterOptions::setAxes( bool axes )
{
  expwidget->showAxesCheckBox->setChecked( axes );
}

bool AsyExporterOptions::showAxes() const
{
  return expwidget->showAxesCheckBox->isChecked();
}

void AsyExporterOptions::setExtraFrame( bool frame )
{
  expwidget->showFrameCheckBox->setChecked( frame );
}

bool AsyExporterOptions::showExtraFrame() const
{
  return expwidget->showFrameCheckBox->isChecked();
}
