// exporter.cc
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "exporter.h"

#include "exporttoimagedialog.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../objects/object_imp.h"
#include "../objects/object.h"
#include "../objects/line_imp.h"
#include "../objects/circle_imp.h"
#include "../objects/point_imp.h"
#include "../misc/common.h"

#include <kaction.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>

class ExporterAction
  : public KAction
{
  KigExporter* mexp;
  const KigDocument* mdoc;
  KigWidget* mw;
public:
  ExporterAction( const KigDocument* doc, KigWidget* w,
                  KActionCollection* parent, KigExporter* exp );
  void slotActivated();
};

ExporterAction::ExporterAction( const KigDocument* doc, KigWidget* w,
                                KActionCollection* parent, KigExporter* exp )
  : KAction( exp->menuEntryName(), KShortcut(), 0, 0, parent ),
    mexp( exp ), mdoc( doc ), mw( w )
{
};

void ExporterAction::slotActivated()
{
  mexp->run( *mdoc, *mw );
};

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

void ImageExporter::run( const KigDocument& doc, KigWidget& w )
{
  ExportToImageDialog* d = new ExportToImageDialog( &w, &doc );
  d->exec();
  delete d;
}

KigExportManager::KigExportManager()
{
  mexporters.push_back( new ImageExporter );
  // working on this one ;)
//  mexporters.push_back( new XFigExporter );
}

KigExportManager::~KigExportManager()
{
  for ( uint i = 0; i < mexporters.size(); ++i )
    delete mexporters[i];
}

void KigExportManager::addMenuAction( const KigDocument* doc, KigWidget* w,
                                      KActionCollection* coll )
{
  KActionMenu* m = new KActionMenu( i18n( "&Export to" ), coll, "file_export" );
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
  return i18n( "&XFig file" );
}

class XFigExportImpVisitor
  : public ObjectImpVisitor
{
  QTextStream& mstream;
  RealObject* mcurobj;
  const KigWidget& mw;
  Rect msr;

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
    };

  void emitLine( const Coordinate& a, const Coordinate& b, int width );
public:
  void visit( Object* obj );

  XFigExportImpVisitor( QTextStream& s, const KigWidget& w )
    : mstream( s ), mw( w ), msr( mw.showingRect() )
    {
    };
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

void XFigExportImpVisitor::visit( Object* obj )
{
  if ( ! obj->shown() ) return;
  assert( obj->inherits( Object::ID_RealObject ) );
  mcurobj = static_cast<RealObject*>( obj );
  obj->imp()->visit( this );
};

void XFigExportImpVisitor::visit( const LineImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;
  calcBorderPoints( a, b, msr );
  int width = mcurobj->width();
  if ( width == -1 ) width = 1;

  emitLine( a, b, width );
};

void XFigExportImpVisitor::emitLine( const Coordinate& a, const Coordinate& b, int width )
{
  mstream << "2 "; // polyline type;
  mstream << "1 "; // polyline subtype;
  mstream << "0 "; // line_style: Solid
  mstream << width << " "; // thickness: *1/80 inch
  mstream << "0 "; // TODO pen_color: default
  mstream << "7 "; // TODO fill_color: default
  mstream << "50 "; // depth: 50
  mstream << "-1 "; // pen_style: unused by XFig
  mstream << "-1 "; // area_fill: no fill
  mstream << "0.000 "; // style_val: the distance between dots and
                       // dashes in case of dotted or dashed lines..
  mstream << "0 "; // join_style: Miter
  mstream << "0 "; // cap_style: Butt
  mstream << "-1 "; // radius in case of an arc-box, but we're a
                    // polyline, so nothing here..
  mstream << "0 "; // forward arrow: no
  mstream << "0 "; // backward arrow: no
  mstream << "2"; // a two points polyline..

  mstream << "\n\t ";

  QPoint ca = convertCoord( a );
  QPoint cb = convertCoord( b );

  mstream << ca.x() << " " << ca.y() << " " << cb.x() << " " << cb.y() << "\n";
}

void XFigExportImpVisitor::visit( const PointImp* imp )
{
  const QPoint center = convertCoord( imp->coordinate() );
  int width = mcurobj->width();
  if ( width == -1 ) width = 5;
  width *= 10;

  mstream << "1 "  // Ellipse type
          << "3 "  // circle defined by radius subtype
          << "0 "; // line_style: Solid
  mstream << "1 " << " " // thickness: *1/80 inch
          << "0 " // TODO pen_color: default
          << "0 " // TODO fill_color: black
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

void XFigExportImpVisitor::visit( const TextImp* )
{

}

void XFigExportImpVisitor::visit( const AngleImp* )
{

}

void XFigExportImpVisitor::visit( const VectorImp* )
{

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
  int width = mcurobj->width();
  if ( width == -1 ) width = 1;
  mstream << width << " " // thickness: *1/80 inch
          << "0 " // TODO pen_color: default
          << "7 " // TODO fill_color: default
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

void XFigExportImpVisitor::visit( const ConicImp* )
{
}

void XFigExportImpVisitor::visit( const CubicImp* )
{
}

void XFigExportImpVisitor::visit( const SegmentImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;
  int width = mcurobj->width();
  if ( width == -1 ) width = 1;

  emitLine( a, b, width );
};

void XFigExportImpVisitor::visit( const RayImp* imp )
{
  Coordinate a = imp->data().a;
  Coordinate b = imp->data().b;
  calcRayBorderPoints( a, b, msr );

  int width = mcurobj->width();
  if ( width == -1 ) width = 1;

  emitLine( a, b, width );
}

void XFigExportImpVisitor::visit( const ArcImp* )
{

}

void XFigExporter::run( const KigDocument& doc, KigWidget& w )
{
  QString formats = QString::fromUtf8( "*.fig|XFig Documents (*.fig)" );
  QString file_name = KFileDialog::getSaveFileName(":document", formats );
  if ( file_name.isEmpty() ) return;
  else if ( QFileInfo( file_name ).exists() )
  {
    int ret = KMessageBox::warningYesNo(
      &w,
      i18n( "The file \"%1\" already exists.  Do you wish to overwrite it?" ).arg( file_name ) );
    if ( ret != KMessageBox::Yes ) return;
  };
  QFile file( file_name );
  if ( ! file.open( IO_WriteOnly ) )
  {
    KMessageBox::sorry( &w, i18n( "The file \"%1\" could not be opened.  Please "
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

  Objects os = doc.objects();
  XFigExportImpVisitor visitor( stream, w );
  for ( Objects::const_iterator i = os.begin();
        i != os.end(); ++i )
  {
    visitor.visit( *i );
  };
}
