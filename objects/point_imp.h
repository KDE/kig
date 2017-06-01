// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_OBJECTS_POINT_IMP_H
#define KIG_OBJECTS_POINT_IMP_H

#include "object_imp.h"
#include "../misc/coordinate.h"

/**
 * An ObjectImp representing a point.
 */
class PointImp
  : public ObjectImp
{
  Coordinate mc;
public:
  typedef ObjectImp Parent;
  /**
   * Returns the ObjectImpType representing PointImp's.
   */
  static const ObjectImpType* stype();

  /**
   * Construct a PointImp with coordinate c.
   */
  PointImp( const Coordinate& c );
  ~PointImp();

  Rect surroundingRect() const Q_DECL_OVERRIDE;
  Coordinate attachPoint() const Q_DECL_OVERRIDE;

  /**
   * Get the coordinate of this PointImp.
   */
  const Coordinate& coordinate() const { return mc; }
  /**
   * Set the coordinate of this PointImp.
   */
  void setCoordinate( const Coordinate& c );

  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool inRect( const Rect& r, int width, const KigWidget& ) const Q_DECL_OVERRIDE;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& d ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;

  PointImp* copy() const Q_DECL_OVERRIDE;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  void fillInNextEscape( QString& s, const KigDocument& ) const Q_DECL_OVERRIDE;
  bool canFillInNextEscape() const Q_DECL_OVERRIDE;

  bool equals( const ObjectImp& rhs ) const Q_DECL_OVERRIDE;
};

class BogusPointImp
  : public PointImp
{
public:
  BogusPointImp( const Coordinate& c );
  ~BogusPointImp();
  static const ObjectImpType* stype();
  const ObjectImpType* type() const Q_DECL_OVERRIDE;
};

#endif
