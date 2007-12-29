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

#include "imageexporteroptions.h"
#include "latexexporter.h"
#include "svgexporter.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/common.h"
#include "../misc/kigfiledialog.h"
#include "../misc/kigpainter.h"
#include "../objects/circle_imp.h"
#include "../objects/line_imp.h"
#include "../objects/object_drawer.h"
#include "../objects/object_holder.h"
#include "../objects/object_imp.h"
#include "../objects/other_imp.h"
#include "../objects/point_imp.h"
#include "../objects/text_imp.h"

#include <qcheckbox.h>
#include <qcolor.h>
#include <qfile.h>
#include <qiconset.h>
#include <qtextstream.h>

#include <kaction.h>
#include <kiconloader.h>
#include <kimageio.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knuminput.h>

#include <map>

// we need this for storing colors in a std::map..
static bool operator<( const QColor& a, const QColor& b )
{
  return a.rgb() < b.rgb();
}

class ExporterAction
  : public KAction
{
  KigExporter* mexp;
  const KigPart* mdoc;
  KigWidget* mw;
public:
  ExporterAction( const KigPart* doc, KigWidget* w,
                  KActionCollection* parent, KigExporter* exp );
  void slotActivated();
};

ExporterAction::ExporterAction( const KigPart* doc, KigWidget* w,
                                KActionCollection* parent, KigExporter* exp )
  : KAction( exp->menuEntryName(), KShortcut(), 0, 0, parent ),
    mexp( exp ), mdoc( doc ), mw( w )
{
  QString iconstr = exp->menuIcon();
  if ( iconstr.isEmpty() )
    return;
  KIconLoader* l = doc->instance()->iconLoader();
  QPixmap icon = l->loadIcon( iconstr, KIcon::Small, 16, KIcon::DefaultState, 0L, true );
  if ( !icon.isNull() )
    setIconSet( QIconSet( icon ) );
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
  return "image";
}

void ImageExporter::run( const KigPart& doc, KigWidget& w )
{
  static bool kimageioRegistered = false;
  if ( ! kimageioRegistered )
  {
    KImageIO::registerFormats();
    kimageioRegistered = true;
  }

  KigFileDialog* kfd = new KigFileDialog(
      QString::null, KImageIO::pattern( KImageIO::Writing ),
      i18n( "Export as Image" ), &w );
  kfd->setOptionCaption( i18n( "Image Options" ) );
  ImageExporterOptions* opts = new ImageExporterOptions( 0L, w.size() );
  kfd->setOptionsWidget( opts );
  opts->WidthInput->setValue( w.size().width() );
  opts->HeightInput->setValue( w.size().height() );
  opts->showGridCheckBox->setChecked( doc.document().grid() );
  opts->showAxesCheckBox->setChecked( doc.document().axes() );
  if ( !kfd->exec() )
    return;

  QString filename = kfd->selectedFile();
  bool showgrid = opts->showGridCheckBox->isOn();
  bool showaxes = opts->showAxesCheckBox->isOn();
  int width = opts->WidthInput->value();
  int height = opts->HeightInput->value();

  delete opts;
  delete kfd;

  QString type = KImageIO::type( filename );
  if ( type.isNull() )
  {
    KMessageBox::sorry( &w, i18n( "Sorry, this file format is not supported." ) );
    return;
  };

  kdDebug() << k_funcinfo << type << endl;

  QFile file( filename );
  if ( ! file.open( IO_WriteOnly ) )
  {
    KMessageBox::sorry( &w,
                        i18n( "The file \"%1\" could not be opened. Please check if the file permissions are set correctly." )
                        .arg( filename ) );
    return;
  };

  QPixmap img( QSize( width, height ) );
  img.fill( Qt::white );
  KigPainter p( ScreenInfo( w.screenInfo().shownRect(), img.rect() ), &img, doc.document() );
  p.setWholeWinOverlay();
  p.drawGrid( doc.document().coordinateSystem(), showgrid, showaxes );
  // FIXME: show the selections ?
  p.drawObjects( doc.document().objects(), false );
  if ( ! img.save( filename, type.latin1() ) )
  {
    KMessageBox::error( &w, i18n( "Sorry, something went wrong while saving to image \"%1\"" ).arg( filename ) );
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
  KActionMenu* m =
    new KActionMenu( i18n( "&Export To" ), coll, "file_export" );
  for ( uint i = 0; i < mexporters.size(); ++i )
    m->insert( new ExporterAction( doc, w, coll, mexporters[i] ) );
}

KigExportManager* KigExportManager::instance()
{
  static KigExportManager m;
  return &m;
}

XFigExporter::~XFigExporter()
{
}

QString XFigExporter::exportToStatement() const
{
  return i18n( "Export to &XFig file" );
}


QString XFigExporter::menuEntryName() const
{
  return i18n( "&XFig File..." );
}

QString XFigExporter::menuIcon() const
{
  return "kig_xfig";
}

class XFigExportImpVisitor
  : public ObjectImpVisitor
{
  QTextStream& mstream;
  ObjectHolder* mcurobj;
  const KigWidget& mw;
  Rect msr;
  std::map<QColor, int> mcolormap;
  int mnextcolorid;
  int mcurcolorid;

  QPoint convertCoord( const Coordinate& c )
    {
      Coordinate ret = ( c - msr.bottomLeft() );
      ret.y = msr.height() - ret.y;
//       kdDebug() << "msr: " << msr << endl
//                 << "ret: " << ret << endl
//                 << "c: " << c << endl;
      ret *= 9450;
      ret /= msr.width();
      return ret.toQPoint();
    }

  void emitLine( const Coordinate& a, const Coordinate& b, int width, bool vector = false );
public:
  void visit( ObjectHolder* obj );
  void mapColor( const ObjectDrawer* obj );

  XFigExportImpVisitor( QTextStream& s, const KigWidget& w )
    : mstream( s ), mw( w ), msr( mw.showingRect() ),
      mnextcolorid( 32 )
    {
      // predefined colors in XFig..
      mcolormap[Qt::black] = 0;
      mcolormap[Qt::blue] = 1;
      mcolormap[Qt::green] = 2;
      mcolormap[Qt::cyan] = 3;
      mcolormap[Qt::red] = 4;
      mcolormap[Qt::magenta] = 5;
      mcolormap[Qt::yellow] = 6;
      mcolormap[Qt::white] = 7;
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
};

void XFigExportImpVisitor::mapColor( const ObjectDrawer* obj )
{
  if ( ! obj->shown() ) return;
  QColor color = obj->color();
  if ( mcolormap.find( color ) == mcolormap.end() )
  {
    int newcolorid = mnextcolorid++;
    mstream << "0 "
            << newcolorid << " "
            << color.name() << "\n";
    mcolormap[color] = newcolorid;
  }
}

void XFigExportImpVisitor::visit( ObjectHolder* obj )
{
  if ( ! obj->drawer()->shown() ) return;
  assert( mcolormap.find( obj->drawer()->color() ) != mcolormap.end() );
  mcurcolorid = mcolormap[ obj->drawer()->color() ];
  mcurobj = obj;
  obj->imp()->visit( this );
}

void XFigExportImpVisitor::visit( const LineImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;
  calcBorderPoints( a, b, msr );
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  if ( a != b )
    emitLine( a, b, width );
}

void XFigExportImpVisitor::emitLine( const Coordinate& a, const Coordinate& b, int width, bool vector )
{
  mstream << "2 "; // polyline type;
  mstream << "1 "; // polyline subtype;
  mstream << "0 "; // line_style: Solid
  mstream << width << " "; // thickness: *1/80 inch
  mstream << mcurcolorid << " "; // pen_color: default
  mstream << "7 "; // fill_color: white
  mstream << "50 "; // depth: 50
  mstream << "-1 "; // pen_style: unused by XFig
  mstream << "-1 "; // area_fill: no fill
  mstream << "0.000 "; // style_val: the distance between dots and
                       // dashes in case of dotted or dashed lines..
  mstream << "0 "; // join_style: Miter
  mstream << "0 "; // cap_style: Butt
  mstream << "-1 "; // radius in case of an arc-box, but we're a
                    // polyline, so nothing here..
  if ( ! vector )
    mstream << "0 "; // forward arrow: no
  else
    mstream << "1 "; // forward arrow: yes
  mstream << "0 "; // backward arrow: no
  mstream << "2"; // a two points polyline..

  mstream << "\n\t ";

  if ( vector )
  {
    // first the arrow line in case of a vector..
    mstream << "1 " // arrow_type: closed triangle
            << "1 " // arrow_style: filled with pen color..
            << "1.00 " // arrow_thickness: 1
            << "195.00 " // arrow_width
            << "165.00 " // arrow_height
            << "\n\t";
  }
  QPoint ca = convertCoord( a );
  QPoint cb = convertCoord( b );

  mstream << ca.x() << " " << ca.y() << " " << cb.x() << " " << cb.y() << "\n";
}

void XFigExportImpVisitor::visit( const PointImp* imp )
{
  const QPoint center = convertCoord( imp->coordinate() );
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 5;
  width *= 10;

  mstream << "1 "  // Ellipse type
          << "3 "  // circle defined by radius subtype
          << "0 "; // line_style: Solid
  mstream << "1 " << " " // thickness: *1/80 inch
          << mcurcolorid << " " // pen_color: default
          << mcurcolorid << " " // fill_color: black
          << "50 " // depth: 50
          << "-1 " // pen_style: unused by XFig
          << "20 " // area_fill: full saturation of the fill color
          << "0.000 " // style_val: the distance between dots and
                      // dashes in case of dotted or dashed lines..
          << "1 "  // direction: always 1
          << "0.0000 "  // angle: the radius of the x-axis: 0
          << center.x() << " " << center.y() << " " // the center..
          << width << " " << width << " " // radius_x and radius_y
          << center.x() << " " // start_x and start_y, appear
          << center.y() << " " // unused..
          << center.x() + width << " " // end_x and end_y,
          << center.y() << "\n";        // appear unused too...
}

void XFigExportImpVisitor::visit( const TextImp* imp )
{
  QString text = imp->text();
  QPoint coord = convertCoord( imp->surroundingRect().bottomLeft() );

  mstream << "4 "    // text type
          << "0 "    // subtype: left justfied
          << mcurcolorid << " "    // color: black
          << "50 "   // depth: 50
          << "-1 "   // pen style: unused
          << "0 "    // font: default
          << "11 "   // font-size: 11
          << "0 "    // angle
          << "0 "    // font-flags: all the defaults..
          << "500 500 " // height, width: large enough..
          << coord.x() << " " // x, y
          << coord.y() << " "
          << text.ascii() << "\\001" // text, terminated by \001
          << "\n";
}

void XFigExportImpVisitor::visit( const AngleImp* )
{
}

void XFigExportImpVisitor::visit( const VectorImp* imp )
{
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;
  emitLine( imp->a(), imp->b(), width, true );
}

void XFigExportImpVisitor::visit( const LocusImp* )
{

}

void XFigExportImpVisitor::visit( const CircleImp* imp )
{
  const QPoint center = convertCoord( imp->center() );
  const int radius =
    ( convertCoord( imp->center() + Coordinate( imp->radius(), 0 ) ) - center ).x();

  mstream << "1 "  // Ellipse type
          << "3 "  // circle defined by radius subtype
          << "0 "; // line_style: Solid
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;
  mstream << width << " " // thickness: *1/80 inch
          << mcurcolorid << " " // pen_color: default
          << "7 " // fill_color: white
          << "50 " // depth: 50
          << "-1 " // pen_style: unused by XFig
          << "-1 " // area_fill: no fill
          << "0.000 " // style_val: the distance between dots and
                      // dashes in case of dotted or dashed lines..
          << "1 "  // direction: always 1
          << "0.0000 "  // angle: the radius of the x-axis: 0
          << center.x() << " " << center.y() << " " // the center..
          << radius << " " << radius << " " // radius_x and radius_y
          << center.x() << " " // start_x and start_y, appear
          << center.y() << " " // unused..
          << center.x() + radius << " " // end_x and end_y,
          << center.y() << "\n";        // appear unused too...
}

void XFigExportImpVisitor::visit( const ConicImp* imp )
{
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;
  if ( imp->conicType() == 1 )
  {
    // an ellipse, this is good, cause this allows us to export to a
    // real ellipse type..
    const ConicPolarData data = imp->polarData();

    // gather some necessary data..
    // the angle of the x axis..
    double anglex = atan2( data.esintheta0, data.ecostheta0 );
    // the exentricity
    double e = hypot( data.esintheta0, data.ecostheta0 );
    // the x radius is easy to find..
    double radiusx = data.pdimen / ( 1 - e * e );
    // the y radius is a bit harder: we first find the distance from
    // the focus point to the mid point of the two focuses, this is:
    double d = -e * data.pdimen / ( 1 - e * e );
    // the distance from the first focus to the intersection of the
    // second axis with the conic equals radiusx:
    double r = radiusx;
    double radiusy = sqrt( r*r - d*d );

    Coordinate center = data.focus1 - Coordinate( cos( anglex ), sin( anglex ) ).normalize( d );
    const QPoint qcenter = convertCoord( center );
    const int radius_x = ( convertCoord( center + Coordinate( radiusx, 0 ) ) -
                           convertCoord( center ) ).x();
    const int radius_y = ( convertCoord( center + Coordinate( radiusy, 0 ) ) -
                           convertCoord( center ) ).x();
    const QPoint qpoint2 = convertCoord( center + Coordinate( -sin( anglex ), cos( anglex ) ) * radiusy );

    mstream << "1 "  // ellipse type
            << "1 "  // subtype: ellipse defined by readii
            << "0 "  // line_style: Solid
            << width << " " // thickness
            << mcurcolorid << " "  // pen_color: black
            << "7 "  // fill_color: white
            << "50 " // depth: 50
            << "-1 " // pan_style: not used
            << "-1 " // area_fill: no fill
            << "0.000 " // style_val: not used
            << "1 " // direction: always 1
            << anglex << " " // angle of the main axis
            << qcenter.x() << " " // center
            << qcenter.y() << " "
            << radius_x << " " // radiuses
            << radius_y << " "
            << qcenter.x() << " " // start point
            << qcenter.y() << " "
            << qpoint2.x() << " " // end point
            << qpoint2.y() << " ";
  }
  else return;
}

void XFigExportImpVisitor::visit( const CubicImp* )
{
}

void XFigExportImpVisitor::visit( const SegmentImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  emitLine( a, b, width );
}

void XFigExportImpVisitor::visit( const RayImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;
  calcRayBorderPoints( a, b, msr );

  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;

  emitLine( a, b, width );
}

void XFigExportImpVisitor::visit( const ArcImp* imp )
{
  const Coordinate center = imp->center();
  const double radius = imp->radius();
  const double startangle = imp->startAngle();
  const double endangle = startangle + imp->angle();
  const double middleangle = ( startangle + endangle ) / 2;
  const Coordinate ad = Coordinate( cos( startangle ), sin( startangle ) ).normalize( radius );
  const Coordinate bd = Coordinate( cos( middleangle ), sin( middleangle ) ).normalize( radius );
  const Coordinate cd = Coordinate( cos( endangle ), sin( endangle ) ).normalize( radius );
  const QPoint a = convertCoord( center + ad );
  const QPoint b = convertCoord( center + bd );
  const QPoint c = convertCoord( center + cd );
  const QPoint cent = convertCoord( center );

  mstream << "5 "  // Ellipse type
          << "1 "  // subtype: open ended arc
          << "0 "; // line_style: Solid
  int width = mcurobj->drawer()->width();
  if ( width == -1 ) width = 1;
  mstream << width << " " // thickness: *1/80 inch
          << mcurcolorid << " " // pen_color: default
          << "7 " // fill_color: white
          << "50 " // depth: 50
          << "-1 " // pen_style: unused by XFig
          << "-1 " // area_fill: no fill
          << "0.000 " // style_val: the distance between dots and
                      // dashes in case of dotted or dashed lines..
          << "0 "; // cap_style: Butt..
  // 0 is clockwise, 1 is counterclockwise .
  int direction = imp->angle() > 0 ? 1 : 0;
    // direction next
  mstream << direction << " "  // direction..
          << "0 "  // forward_arrow: no
          << "0 "  // backward_arrow: no
          << cent.x() << " " << cent.y() << " " // the center..
          << a.x() << " " << a.y() << " " // x1, y1
          << b.x() << " " << b.y() << " " // x2, y2
          << c.x() << " " << c.y() << " " // x3, y3
          << "\n";
}

void XFigExporter::run( const KigPart& doc, KigWidget& w )
{
  KigFileDialog* kfd = new KigFileDialog(
      ":document", i18n( "*.fig|XFig Documents (*.fig)" ),
      i18n( "Export as XFig File" ), &w );
  if ( !kfd->exec() )
    return;

  QString file_name = kfd->selectedFile();

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
  stream << "#FIG 3.2\n";
  stream << "Landscape\n";
  stream << "Center\n";
  stream << "Metric\n";
  stream << "A4\n";
  stream << "100.00\n";
  stream << "Single\n";
  stream << "-2\n";
  stream << "1200 2\n";

  std::vector<ObjectHolder*> os = doc.document().objects();
  XFigExportImpVisitor visitor( stream, w );

  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
  {
    visitor.mapColor( ( *i )->drawer() );
  };

  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
  {
    visitor.visit( *i );
  };
}
