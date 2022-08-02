// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>
// SPDX-FileCopyrightText: 2012 Raoul Bourquin <raoulb@bluewin.ch>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

#include "../objects/bezier_imp.h"
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

class PGFExporterImpVisitor : public ObjectImpVisitor
{
    QTextStream &mstream;
    ObjectHolder *mcurobj;
    const KigWidget &mw;
    Rect msr;

public:
    void visit(ObjectHolder *obj);

    PGFExporterImpVisitor(QTextStream &s, const KigWidget &w)
        : mstream(s)
        , mw(w)
        , msr(mw.showingRect())
    {
    }
    using ObjectImpVisitor::visit;
    void visit(const LineImp *imp) override;
    void visit(const PointImp *imp) override;
    void visit(const TextImp *imp) override;
    void visit(const AngleImp *imp) override;
    void visit(const VectorImp *imp) override;
    void visit(const LocusImp *imp) override;
    void visit(const CircleImp *imp) override;
    void visit(const ConicImp *imp) override;
    void visit(const CubicImp *imp) override;
    void visit(const SegmentImp *imp) override;
    void visit(const RayImp *imp) override;
    void visit(const ArcImp *imp) override;
    void visit(const FilledPolygonImp *imp) override;
    void visit(const ClosedPolygonalImp *imp) override;
    void visit(const OpenPolygonalImp *imp) override;
    void visit(const BezierImp *imp) override;
    void visit(const RationalBezierImp *imp) override;

    double unit;

private:
    /** Maximal line length in the output file. To avoid too long line which
     * can cause error in PGF/TikZ and latex parsing. (Note: this is not 100%
     * strict, the lines may have a few more characters.)
     */
    static const uint maxlinelength = 500;

    /**
     * Return the color definition as a string.
     */
    QString emitPenColor(const QColor &c);

    /**
     * Return the line style definition as a string.
     */
    QString emitPenStyle(const Qt::PenStyle &style);

    /**
     * Return the line width definition as a string.
     */
    QString emitPenSize(const int width);

    /**
     * Return the full pen definition including color, size and style as a string.
     */
    QString emitPen(const QColor &c, const int width, const Qt::PenStyle &style);

    /**
     * Return the full style definition including color, size and style as a string.
     */
    QString emitStyle(const ObjectDrawer *od);

    /**
     * Converts Kig coords to PGF coord system.
     */
    QString emitCoord(const Coordinate &c);

    /**
     * Draws a line (segment) or a vector if vector is true.
     */
    void emitLine(const Coordinate &a, const Coordinate &b, const ObjectDrawer *od, bool vector = false);

    /**
     * Sends a new line character ( \n ) to stream.
     */
    void newLine();

    /**
     * Use to convert a dimension "on the screen" to a dimension wrt.
     * Kig coordinate system.
     */
    double dimRealToCoord(int dim);

    /**
     * Plots a generic curve through its points calculated with getPoint.
     */
    void plotGenericCurve(const CurveImp *imp);
};
