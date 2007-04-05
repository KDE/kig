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

#include <config.h>

#include "latexexporter.h"

#include "latexexporteroptions.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/common.h"
#include "../misc/goniometry.h"
#include "../misc/kigfiledialog.h"
#include "../misc/rect.h"
#include "../objects/circle_imp.h"
#include "../objects/cubic_imp.h"
#include "../objects/curve_imp.h"
#include "../objects/line_imp.h"
#include "../objects/locus_imp.h"
#include "../objects/object_drawer.h"
#include "../objects/object_holder.h"
#include "../objects/object_imp.h"
#include "../objects/other_imp.h"
#include "../objects/point_imp.h"
#include "../objects/polygon_imp.h"
#include "../objects/text_imp.h"

#include <math.h>
#include <algorithm>

#include <qcheckbox.h>
#include <qcolor.h>
#include <qfile.h>
#include <qtextstream.h>

#include <klocale.h>
#include <kmessagebox.h>

#ifdef HAVE_TRUNC
#define KDE_TRUNC(a)	trunc(a)
#else
#define KDE_TRUNC(a)	rint(a)
#endif

struct ColorMap {
  QColor color;
  QString name;
};

LatexExporter::~LatexExporter()
{
}

QString LatexExporter::exportToStatement() const
{
  return i18n( "Export to &Latex..." );
}

QString LatexExporter::menuEntryName() const
{
  return i18n( "&Latex..." );
}

QString LatexExporter::menuIcon() const
{
  // TODO
  return "tex";
}

class LatexExportImpVisitor
  : public ObjectImpVisitor
{
  QTextStream& mstream;
  ObjectHolder* mcurobj;
  const KigWidget& mw;
  Rect msr;
  std::vector<ColorMap> mcolors;
  QString mcurcolorid;
public:
  void visit( ObjectHolder* obj );
  void mapColor( QColor color );

  LatexExportImpVisitor( QTextStream& s, const KigWidget& w )
    : mstream( s ), mw( w ), msr( mw.showingRect() )
    {
    }
  void visit( const LineImp* imp );
  void visit( const PointImp* imp );
  void visit( const TextImp* imp );
  void visit( const AngleImp* imp );
  void visit( const VectorImp* imp );
  void visit( const LocusImp* imp );
  void visit( const CircleImp* imp );
  void visit( const ConicImp* imp );
  void visit( const CubicImp* imp );
  void visit( const SegmentImp* imp );
  void visit( const RayImp* imp );
  void visit( const ArcImp* imp );
  void visit( const PolygonImp* imp );

  double unit;

private:
  /**
   * Converts Kig coords to pstrick coord system and sends them to stream
   * using the format: (xcoord,ycoord)
   */
  void emitCoord( const Coordinate& c );
  /**
   * Draws a line (segment) or a vector if vector is true.
   */
  void emitLine( const Coordinate& a, const Coordinate& b, const int width,
                 const Qt::PenStyle s, bool vector = false );
  /**
   * Sends a new line character ( \n ) to stream.
   */
  void newLine();
  /**
   * Searches if a color is already mapped into mcolors, and returns its
   * index or -1 if not found.
   */
  int findColor( QColor c );
  /**
   * Use to convert a dimension "on the screen" to a dimension wrt.
   * Kig coordinate system.
   */
  double dimRealToCoord( int dim );
  /**
   * Converts a pen style into latex style string.
   */
  QString writeStyle( Qt::PenStyle style );
  /**
   * Plots a generic curve though its points calc'ed with getPoint.
   */
  void plotGenericCurve( const CurveImp* imp );
};

void LatexExportImpVisitor::emitCoord( const Coordinate& c )
{
  mstream << "(" << c.x - msr.left() << "," << c.y - msr.bottom() << ")";
}

void LatexExportImpVisitor::emitLine( const Coordinate& a, const Coordinate& b,
                                      const int width, const Qt::PenStyle s,
                                      bool vector )
{
  mstream << "\\psline[linecolor=" << mcurcolorid << ",linewidth=" << width / 100.0
          << "," << writeStyle( s );
  if ( vector )
    mstream << ",arrowscale=3,arrowinset=1.3";
  mstream << "]";
  if ( vector )
    mstream << "{->}";
  emitCoord( a );
  emitCoord( b );
  newLine();
}

void LatexExportImpVisitor::newLine()
{
  mstream << "\n";
}

int LatexExportImpVisitor::findColor( QColor c )
{
  for ( uint i = 0; i < mcolors.size(); ++i )
  {
    if ( c == mcolors[i].color )
      return i;
  }
  return -1;
}

void LatexExportImpVisitor::mapColor( QColor color )
{
  if ( findColor( color ) == -1 )
  {
    ColorMap newcolor;
    newcolor.color = color;
    QString tmpname = color.name();
    tmpname.replace( "#", "" );
    newcolor.name = tmpname;
    mcolors.push_back( newcolor );
    mstream << "\\newrgbcolor{" << tmpname << "}{" 
            << color.red() / 255.0 << " " << color.green() / 255.0 << " "
            << color.blue() / 255.0 << "}\n";
  }
}

double LatexExportImpVisitor::dimRealToCoord( int dim )
{
  QRect qr( 0, 0, dim, dim );
  Rect r = mw.screenInfo().fromScreen( qr );
  return fabs( r.width() );
}

QString LatexExportImpVisitor::writeStyle( Qt::PenStyle style )
{
  QString ret( "linestyle=" );
  if ( style == Qt::DashLine )
    ret += "dashed";
  else if ( style == Qt::DotLine )
    ret += "dotted,dotsep=2pt";
  else
    ret += "solid";
  return ret;
}

void LatexExportImpVisitor::plotGenericCurve( const CurveImp* imp )
{
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  QString prefix = QString( "\\pscurve[linecolor=%1,linewidth=%2,%3]" )
      .arg( mcurcolorid )
      .arg( width / 100.0 )
      .arg( writeStyle( mcurobj->drawer()->style() ) );

  std::vector< std::vector< Coordinate > > coordlist;
  coordlist.push_back( std::vector< Coordinate >() );
  uint curid = 0;

  Coordinate c;
  Coordinate prev = Coordinate::invalidCoord();
  for ( double i = 0.0; i <= 1.0; i += 0.005 )
  {
    c = imp->getPoint( i, mw.document() );
    if ( !c.valid() )
    {
      if ( coordlist[curid].size() > 0 )
      {
        coordlist.push_back( std::vector< Coordinate >() );
        ++curid;
        prev = Coordinate::invalidCoord();
      }
      continue;
    }
    if ( ! ( ( fabs( c.x ) <= 1000 ) && ( fabs( c.y ) <= 1000 ) ) )
      continue;
    // if there's too much distance between this coordinate and the previous
    // one, then it's another piece of curve not joined with the rest
    if ( prev.valid() && ( c.distance( prev ) > 4.0 ) )
    {
      coordlist.push_back( std::vector< Coordinate >() );
      ++curid;
    }
    coordlist[curid].push_back( c );
    prev = c;
  }
  // special case for ellipse
  if ( const ConicImp* conic = dynamic_cast< const ConicImp* >( imp ) )
  {
    // if ellipse, close its path
    if ( conic->conicType() == 1 && coordlist.size() == 1 && coordlist[0].size() > 1 )
    {
      coordlist[0].push_back( coordlist[0][0] );
    }
  }
  for ( uint i = 0; i < coordlist.size(); ++i )
  {
    uint s = coordlist[i].size();
    // there's no point in draw curves empty or with only one point
    if ( s <= 1 )
      continue;

    mstream << prefix;
    for ( uint j = 0; j < s; ++j )
      emitCoord( coordlist[i][j] );
    newLine();
  }
}

void LatexExportImpVisitor::visit( ObjectHolder* obj )
{
  if ( ! obj->drawer()->shown() )
    return;
  const int id = findColor( obj->drawer()->color() );
  if ( id == -1 )
    return;
  mcurcolorid = mcolors[id].name;
  mcurobj = obj;
  obj->imp()->visit( this );
}

void LatexExportImpVisitor::visit( const LineImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;
  calcBorderPoints( a, b, msr );

  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  emitLine( a, b, width, mcurobj->drawer()->style() );
}

void LatexExportImpVisitor::visit( const PointImp* imp )
{
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 5;
  width /= 5;

  mstream << "\\psdots[linecolor=" << mcurcolorid
          << ",dotscale=" << width << ",dotstyle=";
  const int ps = mcurobj->drawer()->pointStyle();
  QString pss( "*,fillstyle=solid,fillcolor=" + mcurcolorid );
  if ( ps == 1 )
    pss = "o,fillstyle=none";
  else if ( ps == 2 )
    pss = "square*,fillstyle=solid,fillcolor=" + mcurcolorid;
  else if ( ps == 3 )
    pss = "square,fillstyle=none";
  else if ( ps == 4 )
    pss = "+,dotangle=45";
  mstream << pss << "]";
  emitCoord( imp->coordinate() );
  newLine();
}

void LatexExportImpVisitor::visit( const TextImp* imp )
{
  // FIXME: support multiline texts...
  mstream << "\\rput[tl]";
  emitCoord( imp->coordinate() );
  newLine();
  mstream << "{";
  newLine();
  if ( imp->hasFrame() )
  {
    mstream << "  \\psframebox[linecolor=c5c2c5,linewidth=0.01"
            << ",fillstyle=solid,fillcolor=ffffde]"
            << "{" << imp->text() << "}";
  }
  else
  {
    mstream << imp->text();
  }
  newLine();
  mstream << "}";
  newLine();
}

void LatexExportImpVisitor::visit( const AngleImp* imp )
{
  const Coordinate center = imp->point();
  const double radius = dimRealToCoord( 50 ) * unit;
  double startangle = imp->startAngle();
  double endangle = startangle + imp->angle();
//  if ( startangle > M_PI )
//    startangle -= 2 * M_PI;
  startangle = Goniometry::convert( startangle, Goniometry::Rad, Goniometry::Deg );
//  if ( endangle > 2 * M_PI )
//    endangle -= 2 * M_PI;
  endangle = Goniometry::convert( endangle, Goniometry::Rad, Goniometry::Deg );
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  mstream << "\\psarc[linecolor=" << mcurcolorid << ",linewidth=" << width / 100.0
          << "," << writeStyle( mcurobj->drawer()->style() ) << ",arrowscale=3,arrowinset=0]{->}";
  emitCoord( center );
  mstream << "{" <<  radius << "}{" << startangle << "}{" << endangle << "}";
  newLine();
}

void LatexExportImpVisitor::visit( const VectorImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;

  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  emitLine( a, b, width, mcurobj->drawer()->style(), true );
}

void LatexExportImpVisitor::visit( const LocusImp* imp )
{
  plotGenericCurve( imp );
}

void LatexExportImpVisitor::visit( const CircleImp* imp )
{
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  mstream << "\\pscircle[linecolor=" << mcurcolorid << ",linewidth=" << width / 100.0
          << "," << writeStyle( mcurobj->drawer()->style() ) << "]";
  emitCoord( imp->center() );
  mstream << "{" << imp->radius() * unit << "}";
  newLine();
}

void LatexExportImpVisitor::visit( const ConicImp* imp )
{
  plotGenericCurve( imp );
}

void LatexExportImpVisitor::visit( const CubicImp* )
{
  // FIXME: cubic are not drawn correctly with plotGenericCurve
//  plotGenericCurve( imp );
}

void LatexExportImpVisitor::visit( const SegmentImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;

  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  emitLine( a, b, width, mcurobj->drawer()->style() );
}

void LatexExportImpVisitor::visit( const RayImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;
  calcRayBorderPoints( a, b, msr );

  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  emitLine( a, b, width, mcurobj->drawer()->style() );
}

void LatexExportImpVisitor::visit( const ArcImp* imp )
{
  const Coordinate center = imp->center();
  const double radius = imp->radius() * unit;
  double startangle = imp->startAngle();
  double endangle = startangle + imp->angle();
//  if ( startangle > M_PI )
//    startangle -= 2 * M_PI;
  startangle = Goniometry::convert( startangle, Goniometry::Rad, Goniometry::Deg );
//  if ( endangle > M_PI )
//    endangle -= 2 * M_PI;
  endangle = Goniometry::convert( endangle, Goniometry::Rad, Goniometry::Deg );
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  mstream << "\\psarc[linecolor=" << mcurcolorid << ",linewidth=" << width / 100.0
          << "," << writeStyle( mcurobj->drawer()->style() ) << "]";
  emitCoord( center );
  mstream << "{" <<  radius << "}{" << startangle << "}{" << endangle << "}";
  newLine();
}

void LatexExportImpVisitor::visit( const PolygonImp* imp )
{
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  mstream << "\\pspolygon[linecolor=" << mcurcolorid << ",linewidth=0"
          << "," << writeStyle( mcurobj->drawer()->style() )
          << ",hatchcolor=" << mcurcolorid << ",hatchwidth=0.5pt,hatchsep=0.5pt"
          << ",fillcolor=" << mcurcolorid << ",fillstyle=crosshatch]";

  std::vector<Coordinate> pts = imp->points();
  for ( uint i = 0; i < pts.size(); i++ )
  {
    emitCoord( pts[i] );
  }
  newLine();
}

void LatexExporter::run( const KigPart& doc, KigWidget& w )
{
  KigFileDialog* kfd = new KigFileDialog(
      QString::null, i18n( "*.tex|Latex Documents (*.tex)" ),
      i18n( "Export as Latex" ), &w );
  kfd->setOptionCaption( i18n( "Latex Options" ) );
  LatexExporterOptions* opts = new LatexExporterOptions( 0L );
  kfd->setOptionsWidget( opts );
  opts->showGridCheckBox->setChecked( doc.document().grid() );
  opts->showAxesCheckBox->setChecked( doc.document().axes() );
  opts->showExtraFrameCheckBox->setChecked( false );
  if ( !kfd->exec() )
    return;

  QString file_name = kfd->selectedFile();
  bool showgrid = opts->showGridCheckBox->isOn();
  bool showaxes = opts->showAxesCheckBox->isOn();
  bool showframe = opts->showExtraFrameCheckBox->isOn();

  delete opts;
  delete kfd;

  QFile file( file_name );
  if ( ! file.open( IO_WriteOnly ) )
  {
    KMessageBox::sorry( &w, i18n( "The file \"%1\" could not be opened. Please "
                                  "check if the file permissions are set correctly." )
                        .arg( file_name ) );
    return;
  };

  QTextStream stream( &file );
  stream << "\\documentclass[a4paper]{minimal}\n";
//  stream << "\\usepackage[latin1]{inputenc}\n";
  stream << "\\usepackage{pstricks}\n";
  stream << "\\usepackage{pst-plot}\n";
  stream << "\\author{Kig " << KIGVERSION << "}\n";
  stream << "\\begin{document}\n";

  const double bottom = w.showingRect().bottom();
  const double left = w.showingRect().left();
  const double height = w.showingRect().height();
  const double width = w.showingRect().width();

/*
  // TODO: calculating aspect ratio...
  if ( 297 / 210 >= height / width )
  {
    
  }
*/
  const double tmpwidth = 15.0;
  const double xunit = tmpwidth / width;
  const double yunit = xunit;

  stream << "\\begin{pspicture*}(0,0)(" << tmpwidth << "," << yunit * height << ")\n";
  stream << "\\psset{xunit=" << xunit << "}\n";
  stream << "\\psset{yunit=" << yunit << "}\n";

  std::vector<ObjectHolder*> os = doc.document().objects();
  LatexExportImpVisitor visitor( stream, w );
  visitor.unit = xunit;

  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
  {
    if ( ! ( *i )->shown() ) continue;
    visitor.mapColor( ( *i )->drawer()->color() );
  };
  visitor.mapColor( QColor( 255, 255, 222 ) ); // ffffde - text label background
  visitor.mapColor( QColor( 197, 194, 197 ) ); // c5c2c5 - text label border line
  visitor.mapColor( QColor( 160, 160, 164 ) ); // a0a0a4 - axes color
  visitor.mapColor( QColor( 192, 192, 192 ) ); // c0c0c0 - grid color

  // extra frame
  if ( showframe )
  {
    stream << "\\psframe[linecolor=black,linewidth=0.02]"
           << "(0,0)"
           << "(" << width << "," << height << ")"
           << "\n";
  }

  // grid
  if ( showgrid )
  {
    // vertical lines...
    double startingpoint = - left - 1 + static_cast<int>( KDE_TRUNC( left ) );
    for ( double i = startingpoint; i < width; ++i )
    {
      stream << "\\psline[linecolor=c0c0c0,linewidth=0.01,linestyle=dashed]"
             << "(" << i << ",0)"
             << "(" << i << "," << height << ")"
             << "\n";
    }

    // horizontal lines...
    startingpoint = - bottom - 1 + static_cast<int>( KDE_TRUNC( bottom ) );
    for ( double i = startingpoint; i < height; ++i )
    {
      stream << "\\psline[linecolor=c0c0c0,linewidth=0.01,linestyle=dashed]"
             << "(0," << i << ")"
             << "(" << width << "," << i << ")"
             << "\n";
    }
  }

  // axes
  if ( showaxes )
  {
    stream << "\\psaxes[linecolor=a0a0a4,linewidth=0.03,ticks=none,arrowinset=0]{->}"
           << "(" << -left << "," << -bottom << ")"
           << "(0,0)"
           << "(" << width << "," << height << ")"
           << "\n";
  }

  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
  {
    visitor.visit( *i );
  };

  stream << "\\end{pspicture*}\n";
  stream << "\\end{document}\n";
}
