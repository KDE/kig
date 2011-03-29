// Copyright (C)  2010,2011 Raoul Bourquin

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

#ifndef KIG_FILTERS_ASYEXPORTERIMPVISITOR_H
#define KIG_FILTERS_ASYEXPORTERIMPVISITOR_H

// #include <qcheckbox.h>
// #include <qcolor.h>
// #include <qfile.h>
// #include <qtextstream.h>

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

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


class AsyExporterImpVisitor
  : public ObjectImpVisitor
{
  QTextStream& mstream;
  ObjectHolder* mcurobj;
  const KigWidget& mw;
  Rect msr;
public:
  void visit( ObjectHolder* obj );

  AsyExporterImpVisitor( QTextStream& s, const KigWidget& w )
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
  void visit( const FilledPolygonImp* imp );
  void visit( const ClosedPolygonalImp* imp );
  void visit( const OpenPolygonalImp* imp );
  void visit ( const BezierImp* imp );

  double unit;

private:
  /** Maximal line length in the output file. To avoid too long line which
    * can cause error in asy and latex parsing. (Note: this is not 100% strict,
    * the lines may have a few more characters.)
    */
  static const uint maxlinelength = 500;
  
  /**
   * Converts Kig coords to pstrick coord system and sends them to stream
   * using the format: (xcoord,ycoord)
   */
  QString emitCoord( const Coordinate& c );
  
  /**
   * Draws a line (segment) or a vector if vector is true.
   */
  void emitLine( const Coordinate& a, const Coordinate& b, const int width,
                 const Qt::PenStyle s, bool vector = false );
  /**
   * Writes the color to the stream as rgb(r,g,b)
   */
  QString emitColor( const QColor& c );
  
  /**
   * Writes a style definition to the stream.
   */
  QString emitStyle( Qt::PenStyle style );
  
  /**
   * Sends a new line character ( \n ) to stream.
   */
  void newLine();

  /**
   * Use to convert a dimension "on the screen" to a dimension wrt.
   * Kig coordinate system.
   */
  double dimRealToCoord( int dim );
  
  /**
   * Plots a generic curve though its points calc'ed with getPoint.
   */
  void plotGenericCurve( const CurveImp* imp );
};

#endif
