// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>
// SPDX-FileCopyrightText: 2012 Raoul Bourquin <raoulb@bluewin.ch>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "pgfexporterimpvisitor.h"

#include "../misc/goniometry.h"
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


void PGFExporterImpVisitor::newLine()
{
    mstream << ";\n";
}


QString PGFExporterImpVisitor::emitPenColor( const QColor& c )
{
  QString pencolor(QLatin1String(""));
  pencolor = "color={rgb,255:red," + QString::number(c.red()) + ";green," + QString::number(c.green()) + ";blue," + QString::number(c.blue()) + '}';
  return pencolor;
}


QString PGFExporterImpVisitor::emitPenStyle( const Qt::PenStyle& style )
{
  QString penstyle(QStringLiteral("line style="));
  // PGF/TikZ definition of pen (line) style
  // TODO: finetune styles
  if ( style == Qt::SolidLine ) {
    penstyle = QStringLiteral("solid");
  } else if ( style == Qt::DashLine ) {
    penstyle = QStringLiteral("dashed");
  } else if ( style == Qt::DotLine ) {
    penstyle = QStringLiteral("dotted,dotsep=2pt");
  } else if ( style == Qt::DashDotLine ) {
    penstyle = QStringLiteral("solid");
  } else if ( style == Qt::DashDotDotLine ) {
    penstyle = QStringLiteral("solid");
  }
  return penstyle;
}


QString PGFExporterImpVisitor::emitPenSize( const int width )
{
  // In this function we map the logical (integer) linewidth of Kig
  // to real line widths that can be used in PGF/TikZ.
  QString pensize(QLatin1String(""));
  if ( width < 0 )
  {
    // Nothing specified, use TikZ default
    pensize = QStringLiteral("line width=1pt");
  }
  else
  {
    // TikZ definition of pen size
    pensize = "line width=" + QString::number(width / 2.0) + "pt";
  }
  return pensize;
}


QString PGFExporterImpVisitor::emitPen( const QColor& c, const int width, const Qt::PenStyle& style )
{
  QString pen(QLatin1String(""));
  // TikZ definition of a pen
  pen = emitPenColor(c) + ", " + emitPenSize(width) + ", " + emitPenStyle(style);
  return  pen;
}


QString PGFExporterImpVisitor::emitStyle( const ObjectDrawer* od )
{
    int width = od->width();
    if ( width == -1 ) width = 1;

    // TODO: finetune line width

    QString style(QLatin1String(""));
    style = emitPen( od->color(), width, od->style() );
    return style;
}


QString PGFExporterImpVisitor::emitCoord( const Coordinate& c )
{
  QString ret(QLatin1String(""));
  ret = '(' + QString::number(c.x) + ',' + QString::number(c.y) + ')';
  return ret;
}


void PGFExporterImpVisitor::emitLine( const Coordinate& a, const Coordinate& b,
                                      const ObjectDrawer* od, bool vector )
{
  if (vector)
  {
    mstream << "\\draw[" << emitStyle(od) << ", ->]";
  }
  else
  {
    mstream << "\\draw[" << emitStyle(od) << "]";
  }
  mstream << " " << emitCoord(a)
          << " -- "
          << emitCoord(b);
  newLine();
}


void PGFExporterImpVisitor::plotGenericCurve( const CurveImp* imp )
{
    std::vector< std::vector< Coordinate > > coordlist;
    coordlist.push_back( std::vector< Coordinate >() );
    uint curid = 0;

    Coordinate c;
    Coordinate prev = Coordinate::invalidCoord();
    for ( double i = 0.0; i <= 1.0; i += 0.0001 )
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
        if ( ! ( ( fabs( c.x ) <= 10000 ) && ( fabs( c.y ) <= 10000 ) ) )
            continue;
        // if there's too much distance between this coordinate and the previous
        // one, then it's another piece of curve not joined with the rest
        if ( prev.valid() && ( c.distance( prev ) > 50.0 ) )
        {
            coordlist.push_back( std::vector< Coordinate >() );
            ++curid;
        }
        coordlist[curid].push_back( c );
        prev = c;
    }

    for ( uint i = 0; i < coordlist.size(); ++i )
    {
        uint s = coordlist[i].size();
        // there's no point in draw curves empty or with only one point
        if ( s <= 1 )
            continue;

        QString tmp = "\\draw [" + emitStyle( mcurobj->drawer() ) + ", /pgf/fpu,/pgf/fpu/output format=fixed ] ";
        mstream << tmp;

        uint linelength = tmp.length();

        for ( uint j = 0; j < s; ++j )
        {
            tmp = emitCoord( coordlist[i][j] );
            // Avoid too long lines in the output file
            if(linelength + tmp.length() > maxlinelength)
            {
                linelength = tmp.length();
                mstream << "\n";
            }
            else
            {
                linelength += tmp.length();
            }
            mstream << tmp;

            if ( j < s-1 )
            {
                linelength += 4;
                mstream << " -- ";
            }
            else
            {
                newLine();
                linelength = 0;
            }
        }
        newLine();
    }
}


void PGFExporterImpVisitor::visit( ObjectHolder* obj )
{
    mstream << "%% " << obj->imp()->type()->translatedName();
    newLine();
    if ( ! obj->drawer()->shown() )
        return;
    mcurobj = obj;
    obj->imp()->visit( this );
}


void PGFExporterImpVisitor::visit( const LineImp* imp )
{
    Coordinate a = imp->data().a;
    Coordinate b = imp->data().b;
    calcBorderPoints( a, b, msr );

    emitLine( a, b, mcurobj->drawer(), false );
}


void PGFExporterImpVisitor::visit( const PointImp* imp )
{
    float width = mcurobj->drawer()->width();
    // Some magic numbers that result in good point sizes
    if ( width == -1) { width = 2.5; }
    else { width /= 2.5; }

    mstream << "\\filldraw [" << emitPenColor(mcurobj->drawer()->color()) << "] "
            << emitCoord (imp->coordinate())
            << " circle (" << width << "pt )";
    newLine();
}


void PGFExporterImpVisitor::visit( const TextImp* imp )
{
    mstream << "\\node ";
    if (imp->hasFrame())
    {
        mstream << "[rectangle,draw,align=left] ";
    }
    else
    {
        mstream << "[align=left] ";
    }    
    mstream << "at "
            << emitCoord(imp->coordinate())
            << " {" << imp->text().replace(QStringLiteral("\n"),QStringLiteral("\\\\")) << "}";
    newLine();
}


void PGFExporterImpVisitor::visit( const AngleImp* imp )
{
    double start = Goniometry::convert( imp->startAngle(), Goniometry::Rad, Goniometry::Deg );
    double end = Goniometry::convert( imp->startAngle() + imp->angle(), Goniometry::Rad, Goniometry::Deg );
    double radius = 0.75;
    mstream << "\\draw [" << emitStyle( mcurobj->drawer() ) << ",->] "
            << emitCoord(imp->point())
            << " +(" << start << ":" << radius << ")"
            << " arc (" << start << ":" << end <<  ":" << radius << ")";
    newLine();
}


void PGFExporterImpVisitor::visit( const VectorImp* imp )
{
    Coordinate a = imp->data().a;
    Coordinate b = imp->data().b;

    emitLine( a, b, mcurobj->drawer(), true );
}


void PGFExporterImpVisitor::visit( const LocusImp* imp )
{
    plotGenericCurve( imp );
}


void PGFExporterImpVisitor::visit( const CircleImp* imp )
{
    mstream << "\\draw [" << emitStyle( mcurobj->drawer() ) << "] "
            << emitCoord( imp->center() )
            << " circle (" << imp->radius() << ")";
    newLine();
}


void PGFExporterImpVisitor::visit( const ConicImp* imp )
{
    plotGenericCurve(imp);
}


void PGFExporterImpVisitor::visit( const CubicImp* imp )
{
    plotGenericCurve(imp);
}


void PGFExporterImpVisitor::visit( const SegmentImp* imp )
{
    Coordinate a = imp->data().a;
    Coordinate b = imp->data().b;

    emitLine( a, b, mcurobj->drawer(), false );
}


void PGFExporterImpVisitor::visit( const RayImp* imp )
{
    Coordinate a = imp->data().a;
    Coordinate b = imp->data().b;
    calcRayBorderPoints( a, b, msr );

    emitLine( a, b, mcurobj->drawer(), false );
}


void PGFExporterImpVisitor::visit( const ArcImp* imp )
{
    double start = Goniometry::convert( imp->startAngle(), Goniometry::Rad, Goniometry::Deg );
    double end = Goniometry::convert( imp->startAngle() + imp->angle(), Goniometry::Rad, Goniometry::Deg );
    double radius = imp->radius();
    mstream << "\\draw [" << emitStyle( mcurobj->drawer() ) << "] "
            << emitCoord(imp->center())
            << " +(" << start << ":" << radius << ")"
            << " arc (" << start << ":" << end <<  ":" << radius << ")";
    newLine();
}


void PGFExporterImpVisitor::visit( const FilledPolygonImp* imp )
{
    mstream << "\\filldraw [" << emitStyle( mcurobj->drawer() ) << "] ";

    std::vector<Coordinate> pts = imp->points();
    for ( uint i = 0; i < pts.size(); i++ )
    {
        mstream << emitCoord( pts[i] );
        mstream << "  --  ";
    }
    mstream << "cycle";
    newLine();
}


void PGFExporterImpVisitor::visit ( const ClosedPolygonalImp* imp )
{
    mstream << "\\draw [" << emitStyle( mcurobj->drawer() ) << "] ";

    std::vector<Coordinate> pts = imp->points();
    for ( uint i = 0; i < pts.size(); i++ )
    {
        mstream << emitCoord( pts[i] );
        mstream << "  --  ";
    }
    mstream << "cycle";
    newLine();
}


void PGFExporterImpVisitor::visit ( const OpenPolygonalImp* imp )
{
    mstream << "\\draw [" << emitStyle( mcurobj->drawer() ) << "] ";

    std::vector<Coordinate> pts = imp->points();
    for ( uint i = 0; i < pts.size(); i++ )
    {
        mstream << emitCoord( pts[i] );
        if (i < pts.size() - 1)
        {
            mstream << "  --  ";
        }
    }
    newLine();
}


void PGFExporterImpVisitor::visit(const BezierImp* imp)
{
    std::vector<Coordinate> pts = imp->points();
    switch (pts.size())
    {
    case 3:
        // Formula for cubic control points
        // CP1 = QP0 + 2/3 *(QP1-QP0)
        // CP2 = CP1 + 1/3 *(QP2-QP0)
        mstream << "\\draw [" << emitStyle( mcurobj->drawer() ) << "] "
                << emitCoord(pts.at(0))
                << ".. controls ($"
                << emitCoord(pts.at(0))
                << "+2/3*"
                << emitCoord(pts.at(1))
                << "-2/3*"
                << emitCoord(pts.at(0))
                << "$) and ($"
                << emitCoord(pts.at(0))
                << "+2/3*"
                << emitCoord(pts.at(1))
                << "-2/3*"
                << emitCoord(pts.at(0))
                << "+1/3*"
                << emitCoord(pts.at(2))
                << "-1/3*"
                << emitCoord(pts.at(0))
                << "$) .. "
                << emitCoord(pts.at(2));
        newLine();
        break;
    case 4:
        mstream << "\\draw [" << emitStyle( mcurobj->drawer() ) << "] "
                << emitCoord(pts.front())
                << ".. controls "
                << emitCoord(pts.at(1))
                << " and "
                << emitCoord(pts.at(2))
                << " .. "
                << emitCoord(pts.back());
        newLine();
        break;
    default:
        plotGenericCurve(imp);
        break;
    }
}


void PGFExporterImpVisitor::visit(const RationalBezierImp* imp)
{
    plotGenericCurve(imp);
}
