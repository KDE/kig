// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>

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

#include "svgexporter.h"

#include "svgexporteroptions.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/common.h"
#include "../misc/kigfiledialog.h"
#include "../misc/kigpainter.h"

#include <tqcheckbox.h>
#include <tqfile.h>
#include <tqpicture.h>
#include <tqrect.h>

#include <klocale.h>
#include <kmessagebox.h>

#include <map>

SVGExporter::~SVGExporter()
{
}

TQString SVGExporter::exportToStatement() const
{
  return i18n( "&Export to SVG..." );
}

TQString SVGExporter::menuEntryName() const
{
  return i18n( "&SVG..." );
}

TQString SVGExporter::menuIcon() const
{
  // TODO
  return "vectorgfx";
}

void SVGExporter::run( const KigPart& part, KigWidget& w )
{
  KigFileDialog* kfd = new KigFileDialog(
      TQString::null, i18n( "*.svg|Scalable Vector Graphics (*.svg)" ),
      i18n( "Export as SVG" ), &w );
  kfd->setOptionCaption( i18n( "SVG Options" ) );
  SVGExporterOptions* opts = new SVGExporterOptions( 0L );
  kfd->setOptionsWidget( opts );
  opts->showGridCheckBox->setChecked( part.document().grid() );
  opts->showAxesCheckBox->setChecked( part.document().axes() );
  if ( !kfd->exec() )
    return;

  TQString file_name = kfd->selectedFile();
  bool showgrid = opts->showGridCheckBox->isOn();
  bool showaxes = opts->showAxesCheckBox->isOn();

  delete opts;
  delete kfd;

  TQFile file( file_name );
  if ( ! file.open( IO_WriteOnly ) )
  {
    KMessageBox::sorry( &w, i18n( "The file \"%1\" could not be opened. Please "
                                  "check if the file permissions are set correctly." )
                        .arg( file_name ) );
    return;
  };

  TQRect viewrect( w.screenInfo().viewRect() );
  TQRect r( 0, 0, viewrect.width(), viewrect.height() );

  TQPicture pic;
  pic.setBoundingRect( r );
  KigPainter* p = new KigPainter( ScreenInfo( w.screenInfo().shownRect(), viewrect ),
                                  &pic, part.document() );
//  p->setWholeWinOverlay();
//  p->setBrushColor( Qt::white );
//  p->setBrushStyle( Qt::SolidPattern );
//  p->drawRect( r );
//  p->setBrushStyle( Qt::NoBrush );
//  p->setWholeWinOverlay();
  p->drawGrid( part.document().coordinateSystem(), showgrid, showaxes );
  p->drawObjects( part.document().objects(), false );

  delete p;

  if ( !pic.save( file_name, "SVG" ) )
  {
    KMessageBox::error( &w, i18n( "Sorry, something went wrong while saving to SVG file \"%1\"" ).arg( file_name ) );
  }

}
