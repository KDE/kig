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
#include "objects.h"
#include "screeninfo.h"

#include <qpainter.h>
#include <qcolor.h>

#include <vector>

class KigView;
class QPaintDevice;
class CoordinateSystem;
class Object;

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
  // don't blaim me for this mutable hack.  It's TT that hasn't got
  // its consts correctly...
  mutable QPainter mP;

  QColor color;
  PenStyle style;
  uint width;
  BrushStyle brushStyle;
  QColor brushColor;

  ScreenInfo msi;

  bool mNeedOverlay;
public:
  /**
   * construct a new KigPainter:
   * the ScreenInfo is used to map the document coordinates to the
   * widget coordinates.  This is done transparently to the objects.
   * needOverlay sets whether we try to remember the places we're
   * drawing on using the various overlay methods. @see overlay()
   */
  KigPainter( const ScreenInfo& r, QPaintDevice* device, bool needOverlay = true );
  ~KigPainter();

  // what rect are we drawing on ?
  Rect window();

  QPoint toScreen( const Coordinate p ) const;
  QRect toScreen( const Rect r ) const;
  Coordinate fromScreen( const QPoint& p ) const;
  Rect fromScreen( const QRect& r ) const;

  // colors and stuff...
  void setStyle( const PenStyle c );
  void setColor( const QColor& c );
  void setWidth( const uint c );
  void setPen( const QPen& p );
  void setBrushStyle( const BrushStyle c );
  void setBrush( const QBrush& b );
  void setBrushColor( const QColor& c );

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
   * draw an object
   */
  void drawObject( const Object* o, bool ss = true );
  void drawObjects( const Objects& os );

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
  void drawText( const Rect r, const QString s, int textFlags = 0,
                 int len = -1);
  void drawText( const Coordinate p, const QString s,
                 int textFlags = 0, int len = -1);

  void drawSimpleText( const Coordinate& c, const QString s );

  const Rect boundingRect( const Rect& r, const QString s,
                            int f = 0, int l = -1 ) const;

  const Rect boundingRect( const Coordinate& c, const QString s,
                            int f = 0, int l = -1 ) const;

  const Rect simpleBoundingRect( const Coordinate& c, const QString s );

  void drawGrid( const CoordinateSystem* c );

  const std::vector<QRect>& overlay() { return mOverlay; };

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

  std::vector<QRect> mOverlay;
};

#endif
