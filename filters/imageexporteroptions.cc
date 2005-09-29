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

#include <qcheckbox.h>
#include <qsize.h>

#include <knuminput.h>

ImageExporterOptions::ImageExporterOptions( QWidget* parent, const QSize& s )
  : ImageExporterOptionsBase( parent, "imageexporteroptions" ), msize( s ),
    minternallysettingstuff( false )
{
  keepAspectRatio->setChecked( true );
  connect( WidthInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotWidthChanged( int ) ) );
  connect( HeightInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotHeightChanged( int ) ) );
}

ImageExporterOptions::~ImageExporterOptions()
{
}

void ImageExporterOptions::slotWidthChanged( int w )
{
  if ( ! minternallysettingstuff && keepAspectRatio->isOn() )
  {
    minternallysettingstuff = true;
    HeightInput->setValue( w * msize.height() / msize.width() );
    minternallysettingstuff = false;
  };
}

void ImageExporterOptions::slotHeightChanged( int h )
{
  if ( ! minternallysettingstuff && keepAspectRatio->isOn() )
  {
    minternallysettingstuff = true;
    WidthInput->setValue( h * msize.width() / msize.height() );
    minternallysettingstuff = false;
  };
}
