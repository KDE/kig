// Copyright (C)  2010,2011 Raoul Bourquin

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "asyexporteroptions.h"

#include "ui_asyexporteroptionswidget.h"

#include <qcheckbox.h>
#include <qlayout.h>

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
