// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#include "exporter.h"
#include "exporter.moc"

#include "imageexporteroptions.h"
#include "latexexporter.h"
#include "svgexporter.h"
#include "xfigexporter.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/common.h"
#include "../misc/kigfiledialog.h"
#include "../misc/kigpainter.h"

#include <qfile.h>

#include <kactionmenu.h>
#include <kactioncollection.h>
#include <kicon.h>
#include <kimageio.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>

ExporterAction::ExporterAction( const KigPart* doc, KigWidget* w,
                                KActionCollection* parent, KigExporter* exp )
  : KAction( exp->menuEntryName(), parent),
    mexp( exp ), mdoc( doc ), mw( w )
{
  QString iconstr = exp->menuIcon();
  if ( !iconstr.isEmpty() )
    setIcon( KIcon( iconstr, const_cast<KigPart*>( doc )->iconLoader() ) );
  connect( this, SIGNAL( triggered() ), this, SLOT( slotActivated() ) );
  if(parent)
    parent->addAction("action", this );
}

void ExporterAction::slotActivated()
{
  mexp->run( *mdoc, *mw );
}

KigExporter::~KigExporter()
{
}

ImageExporter::~ImageExporter()
{
}

QString ImageExporter::exportToStatement() const
{
  return i18n( "&Export to image" );
}

QString ImageExporter::menuEntryName() const
{
  return i18n( "&Image..." );
}

QString ImageExporter::menuIcon() const
{
  return "image-x-generic";
}

void ImageExporter::run( const KigPart& doc, KigWidget& w )
{
  KigFileDialog* kfd = new KigFileDialog(
      QString(), KImageIO::pattern( KImageIO::Writing ),
      i18n( "Export as Image" ), &w );
  kfd->setOptionCaption( i18n( "Image Options" ) );
  ImageExporterOptions* opts = new ImageExporterOptions( 0L );
  kfd->setOptionsWidget( opts );
  opts->setImageSize( w.size() );
  opts->setGrid( doc.document().grid() );
  opts->setAxes( doc.document().axes() );
  if ( !kfd->exec() )
    return;

  QString filename = kfd->selectedFile();
  bool showgrid = opts->showGrid();
  bool showaxes = opts->showAxes();
  QSize imgsize = opts->imageSize();

  delete opts;
  delete kfd;

  KMimeType::Ptr mimeType = KMimeType::findByPath( filename );
  kDebug() << "mimetype: " << mimeType->name();
  if ( !KImageIO::isSupported( mimeType->name(), KImageIO::Writing ) )
  {
    KMessageBox::sorry( &w, i18n( "Sorry, this file format is not supported." ) );
    return;
  };

  QFile file( filename );
  if ( ! file.open( QIODevice::WriteOnly ) )
  {
    KMessageBox::sorry( &w,
                        i18n( "The file \"%1\" could not be opened. Please check if the file permissions are set correctly." ,
                          filename ) );
    return;
  };

  QPixmap img( imgsize );
  img.fill( Qt::white );
  KigPainter p( ScreenInfo( w.screenInfo().shownRect(), img.rect() ), &img, doc.document() );
  p.setWholeWinOverlay();
  p.drawGrid( doc.document().coordinateSystem(), showgrid, showaxes );
  // FIXME: show the selections ?
  p.drawObjects( doc.document().objects(), false );
  QStringList types = KImageIO::typeForMime( mimeType->name() );
  if ( types.isEmpty() ) return; // TODO error dialog?
  if ( !img.save( filename, types.at(0).toLatin1() ) )
  {
    KMessageBox::error( &w, i18n( "Sorry, something went wrong while saving to image \"%1\"", filename ) );
  }

}

KigExportManager::KigExportManager()
{
  mexporters.push_back( new ImageExporter );
  // working on this one ;)
  mexporters.push_back( new XFigExporter );
  mexporters.push_back( new LatexExporter );
  mexporters.push_back( new SVGExporter );
}

KigExportManager::~KigExportManager()
{
  for ( uint i = 0; i < mexporters.size(); ++i )
    delete mexporters[i];
}

void KigExportManager::addMenuAction( const KigPart* doc, KigWidget* w,
                                      KActionCollection* coll )
{
  KActionMenu* m = new KActionMenu( i18n( "&Export To" ), w );
  m->setIcon( KIcon( "document-export", const_cast<KigPart*>( doc )->iconLoader() ) );
  for ( uint i = 0; i < mexporters.size(); ++i )
    m->addAction( new ExporterAction( doc, w, coll, mexporters[i] ) );
  if(coll)
    coll->addAction("file_export", m );
}

KigExportManager* KigExportManager::instance()
{
  static KigExportManager m;
  return &m;
}
