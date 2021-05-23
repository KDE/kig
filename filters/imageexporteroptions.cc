// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "imageexporteroptions.h"

#include "ui_imageexporteroptionswidget.h"

#include <QApplication>
#include <QCheckBox>
#include <QDesktopWidget>
#include <QLayout>
#include <QSize>
#include <QSpinBox>

ImageExporterOptions::ImageExporterOptions( QWidget* parent )
  : QWidget( parent ), minternallysettingstuff( false )
{
  expwidget = new Ui_ImageExporterOptionsWidget();
  expwidget->setupUi( this );

  msize = QSize( 1, 1 );

  // detecting the dpi resolutions
  QDesktopWidget* dw = QApplication::desktop();
  // and creating the Unit objects
  mxunit = Unit( msize.width(), Unit::pixel, dw->logicalDpiX() );
  myunit = Unit( msize.height(), Unit::pixel, dw->logicalDpiY() );

  maspectratio = (double)msize.height() / (double)msize.width();

  expwidget->keepAspectRatio->setChecked( true );
  layout()->setContentsMargins( 0, 0, 0, 0 );

  expwidget->comboUnit->addItems( Unit::unitList() );

  connect( expwidget->WidthInput, SIGNAL(valueChanged(double)), this, SLOT(slotWidthChanged(double)) );
  connect( expwidget->HeightInput, SIGNAL(valueChanged(double)), this, SLOT(slotHeightChanged(double)) );
  connect( expwidget->comboUnit, SIGNAL(activated(int)), this, SLOT(slotUnitChanged(int)) );
}

ImageExporterOptions::~ImageExporterOptions()
{
  delete expwidget;
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
  mxunit.setValue( size.width() );
  myunit.setValue( size.height() );
  maspectratio = (double)msize.height() / (double)msize.width();
  minternallysettingstuff = false;
}

QSize ImageExporterOptions::imageSize() const
{
  return QSize( (int)qRound( mxunit.getValue( Unit::pixel ) ),
                (int)qRound( myunit.getValue( Unit::pixel ) ) );
}

void ImageExporterOptions::slotWidthChanged( double w )
{
  if ( ! minternallysettingstuff && expwidget->keepAspectRatio->isChecked() )
  {
    minternallysettingstuff = true;
    expwidget->HeightInput->setValue( w * maspectratio );
    mxunit.setValue( w );
    myunit.setValue( w * maspectratio );
    minternallysettingstuff = false;
  };
}

void ImageExporterOptions::slotHeightChanged( double h )
{
  if ( ! minternallysettingstuff && expwidget->keepAspectRatio->isChecked() )
  {
    minternallysettingstuff = true;
    expwidget->WidthInput->setValue( h / maspectratio );
    mxunit.setValue( h / maspectratio );
    myunit.setValue( h );
    minternallysettingstuff = false;
  };
}

void ImageExporterOptions::slotUnitChanged( int index )
{
  minternallysettingstuff = true;
  Unit::MetricalUnit newunit = Unit::intToUnit( index );
  mxunit.convertTo( newunit );
  myunit.convertTo( newunit );
  int newprecision = Unit::precision( newunit );
  expwidget->WidthInput->setDecimals( newprecision );
  expwidget->WidthInput->setValue( mxunit.value() );
  expwidget->HeightInput->setDecimals( newprecision );
  expwidget->HeightInput->setValue( myunit.value() );
  minternallysettingstuff = false;
}
