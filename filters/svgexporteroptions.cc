// SPDX-FileCopyrightText: 2005 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "svgexporteroptions.h"

#include "ui_svgexporteroptionswidget.h"

#include <QCheckBox>
#include <QLayout>

SVGExporterOptions::SVGExporterOptions( QWidget* parent )
  : QWidget( parent )
{
  expwidget = new Ui_SVGExporterOptionsWidget();
  expwidget->setupUi( this );

  layout()->setContentsMargins( 0, 0, 0, 0 );
}

SVGExporterOptions::~SVGExporterOptions()
{
  delete expwidget;
}

void SVGExporterOptions::setGrid( bool grid )
{
  expwidget->showGridCheckBox->setChecked( grid );
}

bool SVGExporterOptions::showGrid() const
{
  return expwidget->showGridCheckBox->isChecked();
}

void SVGExporterOptions::setAxes( bool axes )
{
  expwidget->showAxesCheckBox->setChecked( axes );
}

bool SVGExporterOptions::showAxes() const
{
  return expwidget->showAxesCheckBox->isChecked();
}
