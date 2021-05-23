// SPDX-FileCopyrightText: 2010, 2011 Raoul Bourquin <raoulb@bluewin.ch>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "asyexporter.h"
#include "asyexporterimpvisitor.h"
#include "asyexporteroptions.h"

#include "../misc/kigfiledialog.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>

#include <KMessageBox>

#ifdef HAVE_TRUNC
#define KDE_TRUNC(a)	trunc(a)
#else
#define KDE_TRUNC(a)	rint(a)
#endif

AsyExporter::~AsyExporter()
{
}

QString AsyExporter::exportToStatement() const
{
  return i18n( "Export to &Asymptote..." );
}

QString AsyExporter::menuEntryName() const
{
  return i18n( "&Asymptote..." );
}

QString AsyExporter::menuIcon() const
{
  return QStringLiteral("text-plain");
}

void AsyExporter::run( const KigPart& doc, KigWidget& w )
{
  KigFileDialog* kfd = new KigFileDialog(
      QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ), i18n( "Asymptote Documents (*.asy)" ),
      i18n( "Export as Asymptote script" ), &w );
  kfd->setOptionCaption( i18n( "Asymptote Options" ) );
  AsyExporterOptions* opts = new AsyExporterOptions( 0L );
  kfd->setOptionsWidget( opts );
  opts->setGrid( true );
  opts->setAxes( true );
  opts->setExtraFrame( false );
  if ( !kfd->exec() )
    return;

  QString file_name = kfd->selectedFile();
  bool showgrid = opts->showGrid();
  bool showaxes = opts->showAxes();
  bool showframe = opts->showExtraFrame();

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

  const double bottom = w.showingRect().bottom();
  const double left = w.showingRect().left();
  const double height = w.showingRect().height();
  const double width = w.showingRect().width();

  std::vector<ObjectHolder*> os = doc.document().objects();
  QTextStream stream( &file );
  AsyExporterImpVisitor visitor( stream, w );

  // Start building the output stream containing the asymptote script commands

  // The file header for pure asymptote
  stream << "settings.outformat=\"pdf\";\n";
  stream << "\n";
  stream << "import math;\n";
  stream << "import graph;\n";
  stream << "\n";
  stream << "size(" << 25*width << "," << 25*height << ");\n";
  stream << "\n";
  stream << "real textboxmargin = 2mm;\n";
  stream << "\n";

  // Grid
  if ( showgrid )
  {
    // TODO: Polar grid
    // Vertical lines
    double startingpoint = static_cast<double>( KDE_TRUNC( left ) );
    for ( double i = startingpoint; i < left+width; ++i )
    {
      stream << "draw((" << i << "," << bottom << ")--(" << i << "," << bottom+height << "),gray);\n";
    }
    // Horizontal lines
    startingpoint = static_cast<double>( KDE_TRUNC( bottom ) );
    for ( double i = startingpoint; i < bottom+height; ++i )
    {
      stream << "draw((" << left << "," << i << ")--(" << left+width << "," << i << "),gray);\n";
    }
  }

  // Axes
  if ( showaxes )
  {
    stream << "draw(("<<left<<",0)--("<<left+width<<",0), black, Arrow);\n";
    stream << "draw((0,"<<bottom<<")--(0,"<<bottom+height<<"), black, Arrow);\n";
  }

  // Visit all the objects
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    visitor.visit( *i );
  }

  stream << "path frame = ("<<left<<","<<bottom<<")--("
          <<left<<","<<bottom+height<<")--("
          <<left+width<<","<<bottom+height<<")--("
          <<left+width<<","<<bottom<<")--cycle;\n";

  // Extra frame
  if ( showframe )
  {
    stream << "draw(frame, black);\n";
  }
  stream << "clip(frame);\n";

  // And close the output file
  file.close();
}
