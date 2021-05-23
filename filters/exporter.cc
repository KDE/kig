// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "exporter.h"

#include "imageexporteroptions.h"
#include "latexexporter.h"
#include "asyexporter.h"
#include "svgexporter.h"
#include "xfigexporter.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/common.h"
#include "../misc/kigfiledialog.h"
#include "../misc/kigpainter.h"

#include <QImageWriter>
#include <QMimeDatabase>
#include <QStandardPaths>

#include <KIconEngine>
#include <KActionMenu>
#include <KActionCollection>
#include <KMessageBox>

ExporterAction::ExporterAction( const KigPart* doc, KigWidget* w,
                                KActionCollection* parent, KigExporter* exp )
  : QAction( exp->menuEntryName(), parent),
    mexp( exp ), mdoc( doc ), mw( w )
{
  QString iconstr = exp->menuIcon();
  if ( !iconstr.isEmpty() )
    setIcon( QIcon( new KIconEngine( iconstr, const_cast<KigPart*>( doc )->iconLoader() ) ) );
  connect( this, &QAction::triggered, this, &ExporterAction::slotActivated );
  if(parent)
    parent->addAction(QStringLiteral("action"), this );
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
  return QStringLiteral("image-x-generic");
}

void ImageExporter::run( const KigPart& doc, KigWidget& w )
{
  KigFileDialog* kfd = new KigFileDialog(
      QStandardPaths::writableLocation( QStandardPaths::PicturesLocation ), QString(),
      i18n( "Export as Image" ), &w );
  const QList<QByteArray> mimeFilters = QImageWriter::supportedMimeTypes();
  QStringList mimeFiltersConverted;
  // Since someone didn't get the memo about what's the type of a mime name...
  for (const auto &mimeFilter : mimeFilters) {
      mimeFiltersConverted.append( QString::fromUtf8( mimeFilter ) );
  }
  kfd->setMimeTypeFilters( mimeFiltersConverted );
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

  QMimeDatabase db;
  QMimeType mimeType = db.mimeTypeForFile( filename );
  qDebug() << "mimetype: " << mimeType.name();
  if ( !QImageWriter::supportedMimeTypes().contains( mimeType.name().toUtf8() ) )
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
  const QStringList types = mimeType.suffixes();
  if ( types.isEmpty() ) return; // TODO error dialog?
  if ( !img.save( filename, types.at(0).toLatin1() ) )
  {
    KMessageBox::error( &w, i18n( "Sorry, something went wrong while saving to image \"%1\"", filename ) );
  }

}

KigExportManager::KigExportManager()
{
  mexporters.push_back( new ImageExporter );
  mexporters.push_back( new XFigExporter );
  mexporters.push_back( new LatexExporter );
  mexporters.push_back( new AsyExporter );
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
  m->setIcon( QIcon( new KIconEngine( "document-export", const_cast<KigPart*>( doc )->iconLoader() ) ) );
  for ( uint i = 0; i < mexporters.size(); ++i )
    m->addAction( new ExporterAction( doc, w, coll, mexporters[i] ) );
  if(coll)
    coll->addAction(QStringLiteral("file_export"), m );
}

KigExportManager* KigExportManager::instance()
{
  static KigExportManager m;
  return &m;
}
