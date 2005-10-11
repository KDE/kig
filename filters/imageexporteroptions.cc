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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "imageexporteroptions.h"
#include "imageexporteroptions.moc"

#include "imageexporteroptionswidget.h"

#include <qcheckbox.h>
#include <qlayout.h>
#include <qsize.h>

#include <knuminput.h>

ImageExporterOptions::ImageExporterOptions( QWidget* parent )
  : QWidget( parent ), minternallysettingstuff( false )
{
  expwidget = new Ui_ImageExporterOptionsWidget();
  expwidget->setupUi( this );

  expwidget->keepAspectRatio->setChecked( true );
  layout()->setMargin( 0 );

  msize = QSize( 1, 1 );

  connect( expwidget->WidthInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotWidthChanged( int ) ) );
  connect( expwidget->HeightInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotHeightChanged( int ) ) );
}

ImageExporterOptions::~ImageExporterOptions()
{
}

void ImageExporterOptions::setGrid( bool grid )
{
  expwidget->showGridCheckBox->setChecked( grid );
}

bool ImageExporterOptions::showGrid() const
{
  return expwidget->showGridCheckBox->isChecked();
}

void ImageExporterOptions::setAxes( bool axes )
{
  expwidget->showAxesCheckBox->setChecked( axes );
}

bool ImageExporterOptions::showAxes() const
{
  return expwidget->showAxesCheckBox->isChecked();
}

void ImageExporterOptions::setImageSize( const QSize& size )
{
  msize = size;
  minternallysettingstuff = true;
  expwidget->WidthInput->setValue( size.width() );
  expwidget->HeightInput->setValue( size.height() );
  minternallysettingstuff = false;
}

QSize ImageExporterOptions::imageSize() const
{
  return QSize( expwidget->WidthInput->value(), expwidget->HeightInput->value() );
}

void ImageExporterOptions::slotWidthChanged( int w )
{
  if ( ! minternallysettingstuff && expwidget->keepAspectRatio->isChecked() )
  {
    minternallysettingstuff = true;
    expwidget->HeightInput->setValue( w * msize.height() / msize.width() );
    minternallysettingstuff = false;
  };
}

void ImageExporterOptions::slotHeightChanged( int h )
{
  if ( ! minternallysettingstuff && expwidget->keepAspectRatio->isChecked() )
  {
    minternallysettingstuff = true;
    expwidget->WidthInput->setValue( h * msize.width() / msize.height() );
    minternallysettingstuff = false;
  };
}
