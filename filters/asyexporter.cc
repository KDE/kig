// Copyright (C)  2010 Raoul Bourquin

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

#include "asyexporter.h"
#include "asyexporterimpvisitor.h"
#include "asyexporteroptions.h"

#include "../misc/kigfiledialog.h"

#include <qfile.h>
#include <qtextstream.h>
#include <kmessagebox.h>

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
  return "text-plain";
}

void AsyExporter::run( const KigPart& doc, KigWidget& w )
{
  KigFileDialog* kfd = new KigFileDialog(
      QString(), i18n( "*.asy|Asymptote Documents (*.asy)" ),
      i18n( "Export as Asymptote script" ), &w );
  kfd->setOptionCaption( i18n( "Asymptote Options" ) );
  AsyExporterOptions* opts = new AsyExporterOptions( 0L );
  kfd->setOptionsWidget( opts );
  opts->setGrid( false );
  opts->setAxes( false );
  opts->setExtraFrame( true );
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
  
  // grid
  if ( showgrid )
  {
    // TODO: Polar grid
    // vertical lines...
    double startingpoint = startingpoint = static_cast<double>( KDE_TRUNC( left ) );
    for ( double i = startingpoint; i < left+width; ++i )
    {
      stream << "draw((" << i << "," << bottom << ")--(" << i << "," << bottom+height << "),gray);\n";
    }
    // horizontal lines...
    startingpoint = static_cast<double>( KDE_TRUNC( bottom ) );
    for ( double i = startingpoint; i < bottom+height; ++i )
    {
      stream << "draw((" << left << "," << i << ")--(" << left+width << "," << i << "),gray);\n";
    }
  }

  // axes
  if ( showaxes )
  {
    stream << "xaxis(\"\", Arrow);\n";
    stream << "yaxis(\"\", Arrow);\n";
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
  // extra frame
  if ( showframe )
  {
    stream << "draw(frame, black);\n";
  }
  stream << "clip(frame);\n";

}
