/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/


#ifndef KIGPAINTER_H
#define KIGPAINTER_H

#include "coordinate.h"
#include "rect.h"

#include <qpainter.h>
#include <qcolor.h>

#include <vector>

class KigView;
class QPaintDevice;

/**
 * KigPainter is an extended qpainter...
 * currently the only difference is that it translates coordinates
 * from and to the internal coordinates/ the widget coordinates...
 * it calls KigView::appendOverlay() for all of the places it draws in...
 * i'm planning to do more advanced stuff here, @see the TODO file...
 */
class KigPainter
  : public Qt
{
protected:
  QPainter mP;

  QColor color;
  PenStyle style;
  uint width;
  BrushStyle brushStyle;
  QColor brushColor;

  KigView* mView;
public:
  /**
   * construct a new KigPainter:
   * the view pointer is stored, and used for its toScreen and
   * fromScreen methods, and for calling appendOverlay() after we
   * painted something...
   */
  KigPainter ( KigView* view, QPaintDevice* device );
  ~KigPainter();

  // what rect are we drawing on ?
  Rect window();

  // colors and stuff...
  void setStyle( const PenStyle c );
  void setColor( const QColor& c );
  void setWidth( const uint c );
  void setPen( const QPen& p );
  void setBrushStyle( const BrushStyle c );
  void setBrush( const QBrush& b );
  void setBrushColor( const QColor& c );

  /**
   * draw a rect..
   */
  void drawRect( const Rect& r );

  /**
   * overload, mainly for drawing the selection rectangle by
   * KigView... 
   */
  void drawRect( const QRect& r );

  /*
   * draw a circle...
   */
  void drawCircle( const Coordinate& center, const double radius );

  /*
   * draw a segment...
   */
  void drawSegment ( const Coordinate& from, const Coordinate& to );

  /**
   * draw a line...
   */
  void drawLine ( const Coordinate& p1, const Coordinate& p2 );

  /**
   * draw a point...
   * small == true means only draw a single dot, otherwise an ellipse
   * will be drawn as for Point objects
   */
  void drawPoint( const Coordinate& p, bool small = true );

  /**
   * draw a polygon defined by the points in pts...
   */
  void drawPolygon( const std::vector<Coordinate>& pts, bool winding = false, int index = 0, int npoints = -1 );

  /*
   * draw text...
   * @see QPainter::drawText()
   */
  void drawText( const Rect r, const QString s, int textFlags = 0, int len = -1);
  inline void drawText( const Coordinate p, const QString s,  int textFlags = 0, int len = -1)
  {
    drawText( Rect( p, mP.window().right(), mP.window().top() ), s, textFlags, len );
  };

protected:
  /**
   * adds a number of rects to mOverlay so that the rects entirely
   * contain the circle...
   * @see mOverlay
   */
  void circleOverlay( const Coordinate& centre, double radius );
  // this works recursively...
  void circleOverlayRecurse( const Coordinate& centre, double radius, const Rect& currentRect );  

  /**
   * adds some rects to mOverlay, so that they cover the segment p1p2 
   * completely...
   * @see Object::getOverlay()
   */
  void segmentOverlay( const Coordinate& p1, const Coordinate& p2 );

  /**
   * ...
   */
  void pointOverlay( const Coordinate& p1 );
  
  /**
   * ...
   * @see drawText(), QPainter::boundingRect()
   */
  void textOverlay( const QRect& r, const QString s, int textFlags, int len );

  // the size we want the overlay rects to be...
  double overlayRectSize();
};

#endif
