// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "svgexporter.h"

#include "svgexporteroptions.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/common.h"
#include "../misc/kigfiledialog.h"
#include "../misc/kigpainter.h"

#include <map>

#include <QFile>
#include <QStandardPaths>
#include <QSvgGenerator>

#include <KMessageBox>

SVGExporter::~SVGExporter()
{
}

QString SVGExporter::exportToStatement() const
{
  return i18n( "&Export to SVG..." );
}

QString SVGExporter::menuEntryName() const
{
  return i18n( "&SVG..." );
}

QString SVGExporter::menuIcon() const
{
  return QStringLiteral("image-svg+xml");
}

void SVGExporter::run( const KigPart& part, KigWidget& w )
{
  KigFileDialog* kfd = new KigFileDialog(
      QStandardPaths::writableLocation( QStandardPaths::PicturesLocation ), i18n( "Scalable Vector Graphics (*.svg)" ),
      i18n( "Export as SVG" ), &w );
  kfd->setOptionCaption( i18n( "SVG Options" ) );
  SVGExporterOptions* opts = new SVGExporterOptions( 0L );
  kfd->setOptionsWidget( opts );
  opts->setGrid( part.document().grid() );
  opts->setAxes( part.document().axes() );
  if ( !kfd->exec() )
    return;

  QString file_name = kfd->selectedFile();
  bool showgrid = opts->showGrid();
  bool showaxes = opts->showAxes();

  delete opts;
  delete kfd;

  QFile file( file_name );
  if ( ! file.open( QIODevice::WriteOnly ) )
  {
    KMessageBox::sorry( &w, i18n( "The file \"%1\" could not be opened. Please "
                                  "check if the file permissions are set correctly." ,
                          file_name ) );
    return;
  };

  QRect viewrect( w.screenInfo().viewRect() );
  QRect r( 0, 0, viewrect.width(), viewrect.height() );

  // workaround for QSvgGenerator bug not checking for already open device
  file.close();
  QSvgGenerator pic;
  pic.setOutputDevice( &file );
  pic.setSize( r.size() );
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

  if ( !file.flush() )
  {
    KMessageBox::error( &w, i18n( "Sorry, something went wrong while saving to SVG file \"%1\"", file_name ) );
  }
  file.close();
}
