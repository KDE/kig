// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>
// SPDX-FileCopyrightText: 2010, 2011 Raoul Bourquin <raoulb@bluewin.ch> (asymptote exporter part)
// SPDX-FileCopyrightText: 2012 Raoul Bourquin <raoulb@bluewin.ch> (PGF/TikZ exporter part)

// SPDX-License-Identifier: GPL-2.0-or-later

#include "latexexporter.h"
#include "latexexporteroptions.h"

#include "asyexporterimpvisitor.h"
#include "pgfexporterimpvisitor.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/common.h"
#include "../misc/goniometry.h"
#include "../misc/kigfiledialog.h"
#include "../misc/rect.h"
#include "../objects/circle_imp.h"
#include "../objects/cubic_imp.h"
#include "../objects/bezier_imp.h"
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

#include <kig_version.h>

#include <cmath>
#include <algorithm>

#include <QFile>
#include <QTextStream>

#include <KConfigGroup>
#include <KMessageBox>
#include <KSharedConfig>

#ifdef HAVE_TRUNC
#define KDE_TRUNC(a)    trunc(a)
#else
#define KDE_TRUNC(a)    rint(a)
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
    return QStringLiteral("text-x-tex");
}

class PSTricksExportImpVisitor
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
    void mapColor( const QColor& color );

    PSTricksExportImpVisitor( QTextStream& s, const KigWidget& w )
            : mstream( s ), mw( w ), msr( mw.showingRect() )
    {
    }
    using ObjectImpVisitor::visit;
    void visit( const LineImp* imp ) Q_DECL_OVERRIDE;
    void visit( const PointImp* imp ) Q_DECL_OVERRIDE;
    void visit( const TextImp* imp ) Q_DECL_OVERRIDE;
    void visit( const AngleImp* imp ) Q_DECL_OVERRIDE;
    void visit( const VectorImp* imp ) Q_DECL_OVERRIDE;
    void visit( const LocusImp* imp ) Q_DECL_OVERRIDE;
    void visit( const CircleImp* imp ) Q_DECL_OVERRIDE;
    void visit( const ConicImp* imp ) Q_DECL_OVERRIDE;
    void visit( const CubicImp* imp ) Q_DECL_OVERRIDE;
    void visit( const SegmentImp* imp ) Q_DECL_OVERRIDE;
    void visit( const RayImp* imp ) Q_DECL_OVERRIDE;
    void visit( const ArcImp* imp ) Q_DECL_OVERRIDE;
    void visit( const FilledPolygonImp* imp ) Q_DECL_OVERRIDE;
    void visit( const ClosedPolygonalImp* imp ) Q_DECL_OVERRIDE;
    void visit( const OpenPolygonalImp* imp ) Q_DECL_OVERRIDE;
    void visit( const BezierImp* imp ) Q_DECL_OVERRIDE;
    void visit( const RationalBezierImp* imp ) Q_DECL_OVERRIDE;

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
    int findColor( const QColor& c );
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

void PSTricksExportImpVisitor::emitCoord( const Coordinate& c )
{
    mstream << "(" << c.x - msr.left() << "," << c.y - msr.bottom() << ")";
}

void PSTricksExportImpVisitor::emitLine( const Coordinate& a, const Coordinate& b,
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

void PSTricksExportImpVisitor::newLine()
{
    mstream << "\n";
}

int PSTricksExportImpVisitor::findColor( const QColor& c )
{
    for ( uint i = 0; i < mcolors.size(); ++i )
    {
        if ( c == mcolors[i].color )
            return i;
    }
    return -1;
}

void PSTricksExportImpVisitor::mapColor( const QColor& color )
{
    if ( findColor( color ) == -1 )
    {
        ColorMap newcolor;
        newcolor.color = color;
        QString tmpname = color.name();
        tmpname.remove( '#' );
        newcolor.name = tmpname;
        mcolors.push_back( newcolor );
        mstream << "\\newrgbcolor{" << tmpname << "}{"
        << color.red() / 255.0 << " " << color.green() / 255.0 << " "
        << color.blue() / 255.0 << "}\n";
    }
}

double PSTricksExportImpVisitor::dimRealToCoord( int dim )
{
    QRect qr( 0, 0, dim, dim );
    Rect r = mw.screenInfo().fromScreen( qr );
    return fabs( r.width() );
}

QString PSTricksExportImpVisitor::writeStyle( Qt::PenStyle style )
{
    QString ret( QStringLiteral("linestyle=") );
    if ( style == Qt::DashLine )
        ret += QLatin1String("dashed");
    else if ( style == Qt::DotLine )
        ret += QLatin1String("dotted,dotsep=2pt");
    else
        ret += QLatin1String("solid");
    return ret;
}

void PSTricksExportImpVisitor::plotGenericCurve( const CurveImp* imp )
{
    int width = mcurobj->drawer()->width();
    if ( width == -1 ) width = 1;

    QString prefix = QStringLiteral( "\\pscurve[linecolor=%1,linewidth=%2,%3]" )
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

void PSTricksExportImpVisitor::visit( ObjectHolder* obj )
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

void PSTricksExportImpVisitor::visit( const LineImp* imp )
{
    Coordinate a = imp->data().a;
    Coordinate b = imp->data().b;
    calcBorderPoints( a, b, msr );

    int width = mcurobj->drawer()->width();
    if ( width == -1 ) width = 1;

    emitLine( a, b, width, mcurobj->drawer()->style() );
}

void PSTricksExportImpVisitor::visit( const PointImp* imp )
{
    int width = mcurobj->drawer()->width();
    if ( width == -1 ) width = 5;
    width /= 5;

    mstream << "\\psdots[linecolor=" << mcurcolorid
    << ",dotscale=" << width << ",dotstyle=";
    const int ps = mcurobj->drawer()->pointStyle();
    QString pss( "*,fillstyle=solid,fillcolor=" + mcurcolorid );
    if ( ps == 1 )
        pss = QStringLiteral("o,fillstyle=none");
    else if ( ps == 2 )
        pss = "square*,fillstyle=solid,fillcolor=" + mcurcolorid;
    else if ( ps == 3 )
        pss = QStringLiteral("square,fillstyle=none");
    else if ( ps == 4 )
        pss = QStringLiteral("+,dotangle=45");
    mstream << pss << "]";
    emitCoord( imp->coordinate() );
    newLine();
}

void PSTricksExportImpVisitor::visit( const TextImp* imp )
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

void PSTricksExportImpVisitor::visit( const AngleImp* imp )
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

void PSTricksExportImpVisitor::visit( const VectorImp* imp )
{
    Coordinate a = imp->data().a;
    Coordinate b = imp->data().b;

    int width = mcurobj->drawer()->width();
    if ( width == -1 ) width = 1;

    emitLine( a, b, width, mcurobj->drawer()->style(), true );
}

void PSTricksExportImpVisitor::visit( const LocusImp* imp )
{
    plotGenericCurve( imp );
}

void PSTricksExportImpVisitor::visit( const CircleImp* imp )
{
    int width = mcurobj->drawer()->width();
    if ( width == -1 ) width = 1;

    mstream << "\\pscircle[linecolor=" << mcurcolorid << ",linewidth=" << width / 100.0
    << "," << writeStyle( mcurobj->drawer()->style() ) << "]";
    emitCoord( imp->center() );
    mstream << "{" << imp->radius() * unit << "}";
    newLine();
}

void PSTricksExportImpVisitor::visit( const ConicImp* imp )
{
    plotGenericCurve( imp );
}

void PSTricksExportImpVisitor::visit( const CubicImp* )
{
    // FIXME: cubic are not drawn correctly with plotGenericCurve
//  plotGenericCurve( imp );
}

void PSTricksExportImpVisitor::visit( const SegmentImp* imp )
{
    Coordinate a = imp->data().a;
    Coordinate b = imp->data().b;

    int width = mcurobj->drawer()->width();
    if ( width == -1 ) width = 1;

    emitLine( a, b, width, mcurobj->drawer()->style() );
}

void PSTricksExportImpVisitor::visit( const RayImp* imp )
{
    Coordinate a = imp->data().a;
    Coordinate b = imp->data().b;
    calcRayBorderPoints( a, b, msr );

    int width = mcurobj->drawer()->width();
    if ( width == -1 ) width = 1;

    emitLine( a, b, width, mcurobj->drawer()->style() );
}

void PSTricksExportImpVisitor::visit( const ArcImp* imp )
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

void PSTricksExportImpVisitor::visit( const FilledPolygonImp* imp )
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

void PSTricksExportImpVisitor::visit(const ClosedPolygonalImp* imp)
{
    int width = mcurobj->drawer()->width();
    if ( width == -1 ) width = 1;

    mstream << "\\pspolygon[linecolor=" << mcurcolorid << ",linewidth=0"
    << "," << writeStyle( mcurobj->drawer()->style() ) << ']';

    std::vector<Coordinate> pts = imp->points();
    for ( uint i = 0; i < pts.size(); i++ )
    {
        emitCoord( pts[i] );
    }
    newLine();
}

void PSTricksExportImpVisitor::visit(const OpenPolygonalImp* imp)
{
    int width = mcurobj->drawer()->width();
    if ( width == -1 ) width = 1;

    mstream << "\\psline[linecolor=" << mcurcolorid << ",linewidth=0"
    << "," << writeStyle( mcurobj->drawer()->style() ) << ']';

    std::vector<Coordinate> pts = imp->points();
    for ( uint i = 0; i < pts.size(); i++ )
    {
        emitCoord( pts[i] );
    }
    newLine();
}

// TODO: Just a quick fix, improve when reviewing PSTricks exporter
void PSTricksExportImpVisitor::visit(const BezierImp* imp)
{
    plotGenericCurve(imp);
}

// TODO: Just a quick fix, improve when reviewing PSTricks exporter
void PSTricksExportImpVisitor::visit(const RationalBezierImp* imp)
{
    plotGenericCurve(imp);
}

void LatexExporter::run( const KigPart& doc, KigWidget& w )
{
    KigFileDialog* kfd = new KigFileDialog(
        QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ), i18n( "Latex Documents (*.tex)" ),
        i18n( "Export as Latex" ), &w );
    kfd->setOptionCaption( i18n( "Latex Options" ) );
    LatexExporterOptions* opts = new LatexExporterOptions( 0L );
    kfd->setOptionsWidget( opts );

    opts->setGrid( doc.document().grid() );
    opts->setAxes( doc.document().axes() );
    opts->setExtraFrame( false );

    KConfigGroup cg = KSharedConfig::openConfig()->group("Latex Exporter");

    int fmt = cg.readEntry<int>("OutputFormat", LatexExporterOptions::PSTricks);
    if (fmt > -1 && fmt < LatexExporterOptions::FormatCount)
    {
        opts->setFormat((LatexExporterOptions::LatexOutputFormat)fmt);
    }
    opts->setStandalone(cg.readEntry("Standalone", true));

    if ( !kfd->exec() )
        return;

    QString file_name = kfd->selectedFile();
    bool showgrid = opts->showGrid();
    bool showaxes = opts->showAxes();
    bool showframe = opts->showExtraFrame();
    LatexExporterOptions::LatexOutputFormat format = opts->format();
    bool standalone = opts->standalone();

    delete opts;
    delete kfd;

    cg.writeEntry("OutputFormat", (int)format);
    cg.writeEntry("Standalone", standalone);

    QFile file( file_name );
    if ( ! file.open( QIODevice::WriteOnly ) )
    {
        KMessageBox::sorry( &w, i18n( "The file \"%1\" could not be opened. Please "
                                      "check if the file permissions are set correctly." ,
                                      file_name ) );
        return;
    };

    QTextStream stream( &file );
    std::vector<ObjectHolder*> os = doc.document().objects();

    if (format == LatexExporterOptions::PSTricks)
    {
        if (standalone)
        {
            stream << "\\documentclass[a4paper]{minimal}\n";
            //  stream << "\\usepackage[latin1]{inputenc}\n";
            stream << "\\usepackage{pstricks}\n";
            stream << "\\usepackage{pst-plot}\n";
            stream << "\\author{Kig " << KIG_VERSION_STRING << "}\n";
            stream << "\\begin{document}\n";
        }

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

        PSTricksExportImpVisitor visitor( stream, w );
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
        if (standalone)
        {
            stream << "\\end{document}\n";
        }
    }
    else if (format == LatexExporterOptions::TikZ)
    {
        if (standalone)
        {
            stream << "\\documentclass[a4paper]{minimal}\n";
            stream << "\\usepackage{tikz}\n";
            stream << "\\usetikzlibrary{calc}\n";
			stream << "\\usepgflibrary{fpu}\n";
            stream << "\\begin{document}\n";
        }
        PGFExporterImpVisitor visitor( stream, w );

        Rect frameRect = w.showingRect();

        double size = qMax(frameRect.height(),frameRect.width());
        double scale = (size == 0) ? 1 : 10/size;

		// Start a figure and set its global options
        stream << "\\begin{tikzpicture}"
               << "[%\n"
               << "scale=" << scale << ",\n"
               << "]\n";

        double gLeft = frameRect.left();
        double gBottom = frameRect.bottom();
        double gRight = frameRect.right();
        double gTop = frameRect.top();

        // extra frame for clipping
        stream << "\\clip (" << gLeft << ',' << gBottom << ") rectangle (" << gRight << ',' << gTop << ");\n";

        if (showgrid)
        {
            stream << "\\draw [help lines] ("<< floor(qMin(0.0,gRight)) << ',' << floor(qMin(0.0,gTop))
                   << ") grid (" << ceil(qMax(0.0,gRight)) << ',' << ceil(qMax(0.0,gTop)) << ");\n";
            stream << "\\draw [help lines] ("<< floor(qMin(0.0,gLeft)) << ',' << floor(qMin(0.0,gTop))
                   << ") grid (" << ceil(qMax(0.0,gLeft)) << ',' << ceil(qMax(0.0,gTop)) << ");\n";
            stream << "\\draw [help lines] ("<< floor(qMin(0.0,gRight)) << ',' << floor(qMin(0.0,gBottom))
                   << ") grid (" << ceil(qMax(0.0,gRight)) << ',' << ceil(qMax(0.0,gBottom)) << ");\n";
            stream << "\\draw [help lines] ("<< floor(qMin(0.0,gLeft)) << ',' << floor(qMin(0.0,gBottom))
                   << ") grid (" << ceil(qMax(0.0,gLeft)) << ',' << ceil(qMax(0.0,gBottom)) << ");\n";
        }
        if (showaxes)
        {
            if (gBottom < 0 && gTop > 0)
            {
                stream << "\\draw [color=black,->] (" << gLeft << ",0) -- (" << gRight << ",0);\n";
            }
            if (gLeft < 0 && gRight > 0)
            {
                stream << "\\draw [color=black,->] (0," << gBottom <<") -- (0," << gTop << ");\n";
            }
        }
        if (showframe)
        {
            stream << "\\draw [color=black] (" << gLeft << ',' << gBottom << ") rectangle (" << gRight << ',' << gTop << ");\n";
        }

        // Visit all the objects
        for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
        {
            visitor.visit( *i );
        };

        stream << "\\end{tikzpicture}\n";

		// The file footer in case we embed into full latex document
        if (standalone)
        {
            stream << "\\end{document}\n";
        }

    }
    else if (format == LatexExporterOptions::Asymptote)
    {
        const double bottom = w.showingRect().bottom();
        const double left = w.showingRect().left();
        const double height = w.showingRect().height();
        const double width = w.showingRect().width();

        if (standalone)
        {
            // The header if we embed into latex
            stream << "\\documentclass[a4paper,10pt]{article}\n";
            stream << "\\usepackage{asymptote}\n";
            stream << "\n";
            stream << "\\pagestyle{empty}";
            stream << "\n";
            stream << "\\begin{document}\n";
        }

        stream << "\\begin{asy}[width=\\the\\linewidth]\n";
        stream << "\n";
        stream << "import math;\n";
        stream << "import graph;\n";
        stream << "\n";
        stream << "real textboxmargin = 2mm;\n";
        stream << "\n";

        // grid
        if ( showgrid )
        {
            // TODO: Polar grid
            // vertical lines...
            double startingpoint = static_cast<double>( KDE_TRUNC( left ) );
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
            stream << "draw(("<<left<<",0)--("<<left+width<<",0), black, Arrow);\n";
            stream << "draw((0,"<<bottom<<")--(0,"<<bottom+height<<"), black, Arrow);\n";
        }

        // Visit all the objects
        AsyExporterImpVisitor visitor( stream, w );

        for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
        {
            visitor.visit( *i );
        }

        // extra frame for clipping
        stream << "path frame = ("<<left<<","<<bottom<<")--("
        <<left<<","<<bottom+height<<")--("
        <<left+width<<","<<bottom+height<<")--("
        <<left+width<<","<<bottom<<")--cycle;\n";

        if ( showframe )
        {
            stream << "draw(frame, black);\n";
        }
        stream << "clip(frame);\n";
        stream << "\n";
        stream << "\\end{asy}\n";

        // The file footer in case we embed into latex
        if ( standalone )
        {
            stream << "\\end{document}\n";
        }
    }

    // And close the output file
    file.close();
}
