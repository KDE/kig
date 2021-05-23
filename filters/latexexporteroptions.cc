// SPDX-FileCopyrightText: 2005 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "latexexporteroptions.h"

#include "ui_latexexporteroptionswidget.h"

#include <QCheckBox>
#include <QLayout>

LatexExporterOptions::LatexExporterOptions( QWidget* parent )
  : QWidget( parent )
{
  expwidget = new Ui_LatexExporterOptionsWidget();
  expwidget->setupUi( this );

  layout()->setContentsMargins( 0, 0, 0, 0 );
}

LatexExporterOptions::~LatexExporterOptions()
{
  delete expwidget;
}

void LatexExporterOptions::setFormat(LatexExporterOptions::LatexOutputFormat format)
{
  switch (format)
  {
    case PSTricks:
      expwidget->psTricksRadioButton->setChecked(true);
      break;
    case TikZ:
      expwidget->tikzRadioButton->setChecked(true);
      break;
    case Asymptote:
      expwidget->asyRadioButton->setChecked(true);
      break;
    // This enum member is just to track the number of formats, nothing to do here
    case FormatCount:
      break;
  }
}

LatexExporterOptions::LatexOutputFormat LatexExporterOptions::format()
{
  if (expwidget->psTricksRadioButton->isChecked())
  {
    return PSTricks;
  }
  else if (expwidget->tikzRadioButton->isChecked())
  {
    return TikZ;
  } 
  else 
  {
    return Asymptote;
  } 
}

void LatexExporterOptions::setStandalone(bool standalone)
{
  if (standalone)
  {
    expwidget->documentRadioButton->setChecked(true);
  }
  else
  {
    expwidget->pictureRadioButton->setChecked(true);
  }
}

bool LatexExporterOptions::standalone()
{
  return expwidget->documentRadioButton->isChecked();
}

void LatexExporterOptions::setGrid( bool grid )
{
  expwidget->showGridCheckBox->setChecked( grid );
}

bool LatexExporterOptions::showGrid() const
{
  return expwidget->showGridCheckBox->isChecked();
}

void LatexExporterOptions::setAxes( bool axes )
{
  expwidget->showAxesCheckBox->setChecked( axes );
}

bool LatexExporterOptions::showAxes() const
{
  return expwidget->showAxesCheckBox->isChecked();
}

void LatexExporterOptions::setExtraFrame( bool frame )
{
  expwidget->showFrameCheckBox->setChecked( frame );
}

bool LatexExporterOptions::showExtraFrame() const
{
  return expwidget->showFrameCheckBox->isChecked();
}
