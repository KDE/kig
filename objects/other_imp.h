// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_OTHER_IMP_H
#define KIG_OBJECTS_OTHER_IMP_H

#include "curve_imp.h"
#include "../misc/common.h"
#include "../misc/coordinate.h"

/**
 * An ObjectImp representing an angle.
 */
class AngleImp
  : public ObjectImp
{
  static const int radius;
  const Coordinate mpoint;
  const double mstartangle;
  const double mangle;
  bool mmarkRightAngle: 1;
public:
  typedef ObjectImp Parent;
  /**
   * Returns the ObjectImpType representing the AngleImp type.
   */
  static const ObjectImpType* stype();

  /**
   * Construct an Angle with a given center, start angle and
   * dimension (both in radians).
   */
  AngleImp( const Coordinate& pt, double start_angle_in_radials,
            double angle_in_radials, bool markRightAngle );
  ~AngleImp();

  ObjectImp* transform( const Transformation& ) const override;

  void draw( KigPainter& p ) const override;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const override;
  bool inRect( const Rect& r, int width, const KigWidget& ) const override;
  Rect surroundingRect() const override;

  Coordinate attachPoint() const override;
  int numberOfProperties() const override;
  const QList<KLazyLocalizedString> properties() const override;
  const QByteArrayList propertiesInternalNames() const override;
  ObjectImp* property( int which, const KigDocument& w ) const override;
  const char* iconForProperty( int which ) const override;
  const ObjectImpType* impRequirementForProperty( int which ) const override;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const override;

  ObjectImp* copy() const override;

  /**
   * Return the size in radians of this angle.
   */
  double size() const;
  const ObjectImpType* type() const override;
  void visit( ObjectImpVisitor* vtor ) const override;

  /**
   * Return the center of this angle.
   */
  const Coordinate point() const { return mpoint; }
  /**
   * Return the start angle in radians of this angle.
   */
  double startAngle() const { return mstartangle; }
  /**
   * Return the dimension in radians of this angle.
   */
  double angle() const { return mangle; }

  bool markRightAngle() const { return mmarkRightAngle; }

  void setMarkRightAngle( bool markRightAngle ) { mmarkRightAngle = markRightAngle; }

  bool equals( const ObjectImp& rhs ) const override;
};

/**
 * An ObjectImp representing a vector.
 */
class VectorImp
  : public CurveImp
{
  LineData mdata;
public:
  typedef CurveImp Parent;
  /**
   * Returns the ObjectImpType representing the VectorImp type.
   */
  static const ObjectImpType* stype();

  /**
   * Construct a Vector with a given start point and end point.
   */
  VectorImp( const Coordinate& a, const Coordinate& b );
  ~VectorImp();

  ObjectImp* transform( const Transformation& ) const override;

  const Coordinate getPoint( double param, const KigDocument& ) const override;
  double getParam( const Coordinate&, const KigDocument& ) const override;

  void draw( KigPainter& p ) const override;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const override;
  bool inRect( const Rect& r, int width, const KigWidget& ) const override;
  Rect surroundingRect() const override;

  int numberOfProperties() const override;
  const QList<KLazyLocalizedString> properties() const override;
  const QByteArrayList propertiesInternalNames() const override;
  ObjectImp* property( int which, const KigDocument& w ) const override;
  const char* iconForProperty( int which ) const override;
  const ObjectImpType* impRequirementForProperty( int which ) const override;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const override;

  VectorImp* copy() const override;

  /**
   * Return the direction of this vector.
   */
  const Coordinate dir() const;
  /**
   * Return the start point of this vector.
   */
  const Coordinate a() const;
  /**
   * Return the end point of this vector.
   */
  const Coordinate b() const;
  /**
   * Return the length of this vector.
   */
  double length() const;
  /**
   * Get the LineData for this vector.
   */
  LineData data() const;

  const ObjectImpType* type() const override;
  void visit( ObjectImpVisitor* vtor ) const override;

  bool equals( const ObjectImp& rhs ) const override;

  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const override;
  bool internalContainsPoint( const Coordinate& p, double threshold ) const;
};

/**
 * An ObjectImp representing an arc.
 */
class ArcImp
  : public CurveImp
{
  Coordinate mcenter;
  double mradius;
  double msa;
  double ma;
public:
  typedef CurveImp Parent;
  /**
   * Returns the ObjectImpType representing the ArcImp type.
   */
  static const ObjectImpType* stype();

  /**
   * Construct an Arc with a given center, radius, start angle and
   * dimension (both in radians).
   */
  ArcImp( const Coordinate& center, const double radius,
          const double startangle, const double angle );
  ~ArcImp();
  ArcImp* copy() const override;

  ObjectImp* transform( const Transformation& t ) const override;

  void draw( KigPainter& p ) const override;
  bool contains( const Coordinate& p, int width, const KigWidget& w ) const override;
  bool inRect( const Rect& r, int width, const KigWidget& si ) const override;
  Rect surroundingRect() const override;
  bool valid() const;

  int numberOfProperties() const override;
  const QList<KLazyLocalizedString> properties() const override;
  const QByteArrayList propertiesInternalNames() const override;
  ObjectImp* property( int which, const KigDocument& d ) const override;
  const char* iconForProperty( int which ) const override;
  const ObjectImpType* impRequirementForProperty( int which ) const override;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const override;

  const ObjectImpType* type() const override;
  void visit( ObjectImpVisitor* vtor ) const override;

  double getParam( const Coordinate& c, const KigDocument& d ) const override;
  const Coordinate getPoint( double p, const KigDocument& d ) const override;

  /**
   * Return the center of this arc.
   */
  const Coordinate center() const;
  /**
   * Return the radius of this arc.
   */
  double radius() const;
  /**
   * Return the orientation of this arc (usually > 0)
   */
  double orientation() const;
  /**
   * Return the start angle in radians of this arc.
   */
  double startAngle() const;
  /**
   * Return the dimension in radians of this arc.
   */
  double angle() const;
  /**
   * Return the start point of this arc.
   */
  Coordinate firstEndPoint() const;
  /**
   * Return the end point of this arc.
   */
  Coordinate secondEndPoint() const;
  /**
   * Return the size of the sector surface of this arc.
   */
  double sectorSurface() const;

  bool equals( const ObjectImp& rhs ) const override;

  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const override;
  bool internalContainsPoint( const Coordinate& p, double threshold ) const;
};

#endif
