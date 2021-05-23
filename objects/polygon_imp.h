// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

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
  explicit AbstractPolygonImp( const std::vector<Coordinate>& points );
  AbstractPolygonImp( const uint nsides, const std::vector<Coordinate>& points, 
              const Coordinate& centerofmass );
  ~AbstractPolygonImp();
//  PolygonImp* copy() const;

  Coordinate attachPoint() const Q_DECL_OVERRIDE;
  std::vector<Coordinate> ptransform( const Transformation& ) const;

  bool inRect( const Rect& r, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool valid() const;
  Rect surroundingRect() const Q_DECL_OVERRIDE;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

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

  bool equals( const ObjectImp& rhs ) const Q_DECL_OVERRIDE;
  bool isInPolygon( const Coordinate& p ) const;
  bool isOnOPolygonBorder( const Coordinate& p, double dist, const KigDocument& doc ) const;
  bool isOnCPolygonBorder( const Coordinate& p, double dist, const KigDocument& doc ) const;
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
  explicit FilledPolygonImp( const std::vector<Coordinate>& points );
  static const ObjectImpType* stype();
  static const ObjectImpType* stype3();
  static const ObjectImpType* stype4();
  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;
  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  FilledPolygonImp* copy() const Q_DECL_OVERRIDE;
};

/**
 * An ObjectImp representing a closed polygonal.
 */
class ClosedPolygonalImp
  : public AbstractPolygonImp
{
public:
  typedef AbstractPolygonImp Parent;
  explicit ClosedPolygonalImp( const std::vector<Coordinate>& points );
  static const ObjectImpType* stype();
  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;
  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  ClosedPolygonalImp* copy() const Q_DECL_OVERRIDE;
};

/**
 * An ObjectImp representing an open polygonal.
 */
class OpenPolygonalImp
  : public AbstractPolygonImp
{
public:
  typedef AbstractPolygonImp Parent;
  explicit OpenPolygonalImp( const std::vector<Coordinate>& points );
  static const ObjectImpType* stype();
  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;
  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  OpenPolygonalImp* copy() const Q_DECL_OVERRIDE;
};

std::vector<Coordinate> computeConvexHull( const std::vector<Coordinate>& points );

#endif
