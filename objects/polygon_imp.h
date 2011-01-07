// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>

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

#ifndef KIG_OBJECTS_POLYGON_IMP_H
#define KIG_OBJECTS_POLYGON_IMP_H

#include "object_imp.h"
#include "../misc/coordinate.h"
#include <vector>

/**
 * An ObjectImp representing a polygon.
 */
class PolygonImp
  : public ObjectImp
{
  uint mnpoints;
  std::vector<Coordinate> mpoints;
  Coordinate mcenterofmass;
public:
  typedef ObjectImp Parent;
  /**
   * Returns the ObjectImpType representing the PolygonImp type..
   */
  static const ObjectImpType* stype();
  static const ObjectImpType* stype3();
  static const ObjectImpType* stype4();

  /**
   * Constructs a polygon.
   */
  PolygonImp( const std::vector<Coordinate>& points );
  PolygonImp( const uint nsides, const std::vector<Coordinate>& points, 
              const Coordinate& centerofmass );
  ~PolygonImp();
  PolygonImp* copy() const;

  Coordinate attachPoint() const;
  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;
  bool valid() const;
  Rect surroundingRect() const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( uint which ) const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  /**
   * Returns the vector with polygon points.
   */
  const std::vector<Coordinate> points() const; 
  /**
   * Returns the center of mass of the polygon.
   */
  const Coordinate centerOfMass() const; 
  /**
   * Returns the number of points of this polygon.
   */
  const uint npoints() const;

  bool equals( const ObjectImp& rhs ) const;
  bool isInPolygon( const Coordinate& p ) const;
  int windingNumber() const;
  bool isMonotoneSteering() const;
  bool isConvex() const;
};

std::vector<Coordinate> computeConvexHull( const std::vector<Coordinate>& points );

#endif
