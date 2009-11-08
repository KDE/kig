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
class AbstractPolygonImp
  : public ObjectImp
{
protected:
  uint mnpoints;
  std::vector<Coordinate> mpoints;
//  bool minside;   // true: filled polygon, false: polygon boundary
//  bool mopen;     // true: polygonal curve (minside must be false)
  Coordinate mcenterofmass;
public:
  typedef ObjectImp Parent;
  /**
   * Returns the ObjectImpType representing the PolygonImp type.
   */
  static const ObjectImpType* stype();

//  PolygonImp( const std::vector<Coordinate>& points, bool inside = true, bool open = false );
  AbstractPolygonImp( const std::vector<Coordinate>& points );
  AbstractPolygonImp( const uint nsides, const std::vector<Coordinate>& points, 
              const Coordinate& centerofmass );
  ~AbstractPolygonImp();
//  PolygonImp* copy() const;

  Coordinate attachPoint() const;
  std::vector<Coordinate> ptransform( const Transformation& ) const;

  bool inRect( const Rect& r, int width, const KigWidget& ) const;
  bool valid() const;
  Rect surroundingRect() const;

  int numberOfProperties() const;
  const QByteArrayList properties() const;
  const QByteArrayList propertiesInternalNames() const;
  ObjectImp* property( int which, const KigDocument& w ) const;
  const char* iconForProperty( int which ) const;
  const ObjectImpType* impRequirementForProperty( int which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const;

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
  uint npoints() const;
  /**
   * Returns the perimeter of this polygon.
   */
  double operimeter() const;
  double cperimeter() const;
  /**
   * Returns the area of this polygon.
   */
  double area() const;

  bool equals( const ObjectImp& rhs ) const;
  bool isInPolygon( const Coordinate& p ) const;
  bool isOnOPolygonBorder( const Coordinate& p, int width, const KigWidget& w ) const;
  bool isOnCPolygonBorder( const Coordinate& p, int width, const KigWidget& w ) const;
  int windingNumber() const;
  bool isTwisted() const;
  bool isMonotoneSteering() const;
  bool isConvex() const;
};

/**
 * An ObjectImp representing a filled polygon.
 */
class FilledPolygonImp
  : public AbstractPolygonImp
{
public:
  typedef AbstractPolygonImp Parent;
  FilledPolygonImp( const std::vector<Coordinate>& points );
  static const ObjectImpType* stype();
  static const ObjectImpType* stype3();
  static const ObjectImpType* stype4();
  ObjectImp* transform( const Transformation& ) const;
  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  int numberOfProperties() const;
  const QByteArrayList properties() const;
  const QByteArrayList propertiesInternalNames() const;
  ObjectImp* property( int which, const KigDocument& w ) const;
  const char* iconForProperty( int which ) const;
  const ObjectImpType* impRequirementForProperty( int which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  FilledPolygonImp* copy() const;
};

/**
 * An ObjectImp representing a closed polygonal.
 */
class ClosedPolygonalImp
  : public AbstractPolygonImp
{
public:
  typedef AbstractPolygonImp Parent;
  ClosedPolygonalImp( const std::vector<Coordinate>& points );
  static const ObjectImpType* stype();
  ObjectImp* transform( const Transformation& ) const;
  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  int numberOfProperties() const;
  const QByteArrayList properties() const;
  const QByteArrayList propertiesInternalNames() const;
  ObjectImp* property( int which, const KigDocument& w ) const;
  const char* iconForProperty( int which ) const;
  const ObjectImpType* impRequirementForProperty( int which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  ClosedPolygonalImp* copy() const;
};

/**
 * An ObjectImp representing an open polygonal.
 */
class OpenPolygonalImp
  : public AbstractPolygonImp
{
public:
  typedef AbstractPolygonImp Parent;
  OpenPolygonalImp( const std::vector<Coordinate>& points );
  static const ObjectImpType* stype();
  ObjectImp* transform( const Transformation& ) const;
  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  int numberOfProperties() const;
  const QByteArrayList properties() const;
  const QByteArrayList propertiesInternalNames() const;
  ObjectImp* property( int which, const KigDocument& w ) const;
  const char* iconForProperty( int which ) const;
  const ObjectImpType* impRequirementForProperty( int which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  OpenPolygonalImp* copy() const;
};

std::vector<Coordinate> computeConvexHull( const std::vector<Coordinate>& points );

#endif
