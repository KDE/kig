/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002-2003  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/


#ifndef KIGPAINTER_H
#define KIGPAINTER_H

#include "coordinate.h"
#include "rect.h"
#include "screeninfo.h"

#include <qpainter.h>
#include <qcolor.h>

#include <vector>

class KigWidget;
class QPaintDevice;
class CoordinateSystem;
class ObjectHierarchy;
class ConicPolarData;
class CubicCartesianData;
class LineData;
class CurveImp;
class KigDocument;
class ObjectHolder;

/**
 * KigPainter is an extended QPainter.
 *
 * Currently the only difference is that it translates coordinates
 * from and to the internal coordinates/ the widget coordinates...
 *
 * It calls KigWidget::appendOverlay() for all of the places it draws in...
 */
class KigPainter
  : public Qt
{
protected:
  // don't blaim me for this mutable hack.  It's TT that hasn't got
  // its consts correctly...
  mutable QPainter mP;

  QColor color;
  PenStyle style;
  int pointstyle;
  int width;
  BrushStyle brushStyle;
  QColor brushColor;

  const KigDocument& mdoc;
  ScreenInfo msi;

  bool mNeedOverlay;
  int overlayenlarge;
public:
  /**
   * construct a new KigPainter:
   * the ScreenInfo is used to map the document coordinates to the
   * widget coordinates.  This is done transparently to the objects.
   * needOverlay sets whether we try to remember the places we're
   * drawing on using the various overlay methods. @see overlay()
   */
  KigPainter( const ScreenInfo& r, QPaintDevice* device, const KigDocument& doc,
              bool needOverlay = true );
  ~KigPainter();

  /**
   * what rect are we drawing on ?
   */
  Rect window();

  QPoint toScreen( const Coordinate p ) const;
  QRect toScreen( const Rect r ) const;
  QRect toScreenEnlarge( const Rect r ) const;
  Coordinate fromScreen( const QPoint& p ) const;
  Rect fromScreen( const QRect& r ) const;

  // colors and stuff...
  void setStyle( const PenStyle c );
  void setColor( const QColor& c );
  /**
   * setting this to -1 means to use the default width for the object
   * being drawn..  a point -> 5, other objects -> 1
   */
  void setWidth( const int c );
  void setPointStyle( const int p );
  void setPen( const QPen& p );
  void setBrushStyle( const BrushStyle c );
  void setBrush( const QBrush& b );
  void setBrushColor( const QColor& c );

  QColor getColor() const;
  bool getNightVision( ) const;

  double pixelWidth();

  /**
   * this is called by some drawing functions that modify the 'entire'
   * screen, i.e. they do so many changes that it's easier to just
   * update the entire screen, or else i have been to lazy to
   * implement an appropriate overlay function ;)
   * it clears mOverlay, and sets it to the entire widget...
   */
  void setWholeWinOverlay();

  /**
   * draw an object ( by calling its draw function.. )
   */
  void drawObject( const ObjectHolder* o, bool sel );
  void drawObjects( const std::vector<ObjectHolder*>& os, bool sel );
  template<typename iter>
  void drawObjects( iter begin, iter end, bool sel )
    {
      for ( ; begin != end; ++begin )
        drawObject( *begin, sel );
    }

  /**
   * draw a generic curve...
   */
  void drawCurve( const CurveImp* curve );

  /**
   * draws text in a standard manner, convenience function...
   */
  void drawTextStd( const QPoint& p, const QString& s );

  /**
   * draws a rect filled up with a pattern of cyan lines...
   */
  void drawFilledRect( const QRect& );

  /**
   * draw a rect..
   */
  void drawRect( const Rect& r );

  /**
   * overload, mainly for drawing the selection rectangle by
   * KigWidget...
   */
  void drawRect( const QRect& r );

  /**
   * draw a circle...
   */
  void drawCircle( const Coordinate& center, const double radius );

  /**
   * draw a segment...
   */
  void drawSegment ( const Coordinate& from, const Coordinate& to );
  void drawSegment( const LineData& d );

  /**
   * draw a ray...
   */
  void drawRay( const Coordinate& a, const Coordinate& b );
  void drawRay( const LineData& d );

  /**
   * draw a line...
   */
  void drawLine ( const Coordinate& p1, const Coordinate& p2 );
  void drawLine( const LineData& d );

  /**
   * draw a point...  This means a single point, as in
   * QPainter::drawPoint(), unlike drawFatPoint()...
   */
  void drawPoint( const Coordinate& p );

  /**
   * draw a thick point..  This is what the user sees when he draws a
   * point.  In fact it isn't a point, but a filled circle of a
   * certain radius...
   */
  void drawFatPoint( const Coordinate& p );

  /**
   * draw a polygon defined by the points in pts...
   */
  void drawPolygon( const std::vector<QPoint>& pts, bool winding = false, int index = 0, int npoints = -1 );
  void drawPolygon( const std::vector<Coordinate>& pts, bool winding = false, int index = 0, int npoints = -1 );

  /**
   * draw an area defined by the points in pts filled with the set
   * color...
   */
  void drawArea( const std::vector<Coordinate>& pts, bool border = true );

  /**
   * draw the angle with center point, with size angle, starting
   * at the angle startAngle..  Angles should be in radians.
   */
  void drawAngle( const Coordinate& point, const double startangle,
                  const double angle );

  /**
   * draw the arc ( a part of a circle ), of the circle with center
   * center, radius radius, with size angle, starting at the angle
   * startAngle..  Angles should be in radians..
   */
  void drawArc( const Coordinate& center, const double radius,
                const double startangle, const double angle );

  /**
   * draw a vector ( with an arrow etc. )
   */

  void drawVector( const Coordinate& a, const Coordinate& b );

  /**
   * draw text...
   * \see QPainter::drawText()
   */
  void drawText( const Rect r, const QString s, int textFlags = 0,
                 int len = -1);
  void drawText( const Coordinate p, const QString s,
                 int textFlags = 0, int len = -1);

  void drawSimpleText( const Coordinate& c, const QString s );
  void drawTextFrame( const Rect& frame, const QString& s, bool needframe );

  const Rect boundingRect( const Rect& r, const QString s,
                            int f = 0, int l = -1 ) const;

  const Rect boundingRect( const Coordinate& c, const QString s,
                            int f = 0, int l = -1 ) const;

  const Rect simpleBoundingRect( const Coordinate& c, const QString s );

  void drawGrid( const CoordinateSystem& c, bool showGrid = true, bool showAxes = true );

  const std::vector<QRect>& overlay() { return mOverlay; }

protected:
  /**
   * adds a number of rects to mOverlay so that the rects entirely
   * contain the circle...
   * \see mOverlay
   */
  void circleOverlay( const Coordinate& centre, double radius );
  // this works recursively...
  void circleOverlayRecurse( const Coordinate& centre, double radius, const Rect& currentRect );

  /**
   * adds some rects to mOverlay, so that they cover the segment p1p2
   * completely...
   * \see Object::getOverlay()
   */
  void segmentOverlay( const Coordinate& p1, const Coordinate& p2 );

  /**
   * ...
   */
  void pointOverlay( const Coordinate& p1 );

  /**
   * ...
   * \see drawText(), QPainter::boundingRect()
   */
  void textOverlay( const QRect& r, const QString s, int textFlags, int len );

  /**
   * the size we want the overlay rects to be...
   */
  double overlayRectSize();

  std::vector<QRect> mOverlay;
};

#endif
