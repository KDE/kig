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

#ifndef KIG_OBJECTS_OBJECT_DRAWER_H
#define KIG_OBJECTS_OBJECT_DRAWER_H

#include <qcolor.h>
#include <qnamespace.h>

class ObjectImp;
class KigPainter;
class Coordinate;
class KigWidget;
class Rect;

/**
 * A class holding some information about how a certain object is
 * drawn on the window.
 *
 * An ObjectDrawer is used by an ObjectHolder to keep information
 * about how to draw an ObjectImp on the window.  It is really nothing
 * more than a struct with some convenience methods.  It does not have
 * any virtual methods, or have any complex semantics.  It keeps
 * information like the thickness of an object, its color, and whether
 * or not it is hidden.
 *
 * \note The default width of an object depends on its type.  E.g. A
 * point is by default drawn at width 5, a line at width 1.
 * Therefore, there is a special width -1, which means "the default
 * width for this object".
 */
class ObjectDrawer
{
  QColor mcolor;
  bool mshown;
  int mwidth;
  Qt::PenStyle mstyle;
  int mpointstyle;
public:
  /**
   * Construct a new ObjectDrawer with a default color ( Qt::blue ),
   * width ( -1 ), shown state ( true ), PenStyle ( Qt::SolidLine ),
   * and pointstyle ( 0 )
   */
  ObjectDrawer();
  ObjectDrawer( const QColor& color, int width = -1, bool shown = true, Qt::PenStyle = Qt::SolidLine, int pointStyle = 0 );
  /**
   * Draw the object \p imp on kigpainter \p p .  If \p selected is true, it is
   * drawn in red, otherwise in its normal color.
   */
  void draw( const ObjectImp& imp, KigPainter& p, bool selected ) const;
  /**
   * returns whether the object \p imp contains coordinate \p p . This is
   * dependent on whether it is shown ( when it will never contain
   * anything ), and on its width..
   */
  bool contains( const ObjectImp& imp, const Coordinate& pt, const KigWidget& w, bool nv = false ) const;
  /**
   * returns whether the object \p imp is in the rectangle \p r . This is
   * dependent on whether it is shown and on its width..
   */
  bool inRect( const ObjectImp& imp, const Rect& r, const KigWidget& w ) const;

  /**
   * returns whether the object this ObjectDrawer is responsible for
   * will be drawn or not..
   */
  bool shown() const;
  /**
   * returns the color that the object will be drawn in
   */
  QColor color() const;
  /**
   * return the width of the object
   */
  int width() const;
  /**
   * return PenStyle for all objects except points
   */
  Qt::PenStyle style() const;
  /**
   * return pointStyle for points
   */
  int pointStyle() const;
  /**
   * return pointStyle trasnformed in a string
   */
  QString pointStyleToString() const;
  /**
   * return style trasnformed in a string
   */
  QString styleToString() const;
  /**
   * returns a new ObjectDrawer that is identical to this one.. except
   * that the shown state is set to \p s ..
   */
  ObjectDrawer* getCopyShown( bool s ) const;
  /**
   * returns a new ObjectDrawer that is identical to this one.. except
   * that the color is set to \p c ..
   */
  ObjectDrawer* getCopyColor( const QColor& c ) const;
  /**
   * returns a new ObjectDrawer that is identical to this one.. except
   * that the width is set to \p w ..
   */
  ObjectDrawer* getCopyWidth( int w ) const;
  /**
   * returns a new ObjectDrawer that is identical to this one.. except
   * that the PenStyle state is set to \p s ..
   */
  ObjectDrawer* getCopyStyle( Qt::PenStyle s ) const;
  /**
   * returns a new ObjectDrawer that is identical to this one.. except
   * that the pointStyle state is set to \p p ..
   */
  ObjectDrawer* getCopyPointStyle( int p ) const;
  /**
   * Note that this returns a valid point style in every case, even if
   * the given \p style string is unknown. In that case it returns a
   * default value.
   */
  static int pointStyleFromString( const QString& style );
  /**
   * Note that this returns a valid style in every case, even if the
   * given \p style string is unknown. In that case it returns a default
   * value.
   */
  static Qt::PenStyle styleFromString( const QString& style );
};

#endif
