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
  const Coordinate mpoint;
  const double mstartangle;
  const double mangle;
public:
  typedef ObjectImp Parent;
  /**
   * Returns the ObjectImpType representing the AngleImp type..
   */
  static const ObjectImpType* stype();

  /**
   * Construct an Angle with a given center, start angle and
   * dimension (both in radians).
   */
  AngleImp( const Coordinate& pt, double start_angle_in_radials,
            double angle_in_radials );
  ~AngleImp();

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;
  Rect surroundingRect() const;

  Coordinate attachPoint() const;
  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( uint which ) const;

  ObjectImp* copy() const;

  /**
   * Return the size in radians of this angle.
   */
  const double size() const;
  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  /**
   * Return the center of this angle.
   */
  const Coordinate point() const { return mpoint; }
  /**
   * Return the start angle in radians of this angle.
   */
  const double startAngle() const { return mstartangle; }
  /**
   * Return the dimension in radians of this angle.
   */
  const double angle() const { return mangle; }

  bool equals( const ObjectImp& rhs ) const;
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
   * Returns the ObjectImpType representing the VectorImp type..
   */
  static const ObjectImpType* stype();

  /**
   * Construct a Vector with a given start point and end point.
   */
  VectorImp( const Coordinate& a, const Coordinate& b );
  ~VectorImp();

  ObjectImp* transform( const Transformation& ) const;

  const Coordinate getPoint( double param, const KigDocument& ) const;
  double getParam( const Coordinate&, const KigDocument& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;
  Rect surroundingRect() const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( uint which ) const;

  VectorImp* copy() const;

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
  const double length() const;
  /**
   * Get the LineData for this vector.
   */
  LineData data() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool equals( const ObjectImp& rhs ) const;

  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const;
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
   * Returns the ObjectImpType representing the ArcImp type..
   */
  static const ObjectImpType* stype();

  /**
   * Construct an Arc with a given center, radius, start angle and
   * dimension (both in radians).
   */
  ArcImp( const Coordinate& center, const double radius,
          const double startangle, const double angle );
  ~ArcImp();
  ArcImp* copy() const;

  ObjectImp* transform( const Transformation& t ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& w ) const;
  bool inRect( const Rect& r, int width, const KigWidget& si ) const;
  Rect surroundingRect() const;
  bool valid() const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& d ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( uint which ) const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  double getParam( const Coordinate& c, const KigDocument& d ) const;
  const Coordinate getPoint( double p, const KigDocument& d ) const;

  /**
   * Return the center of this arc.
   */
  const Coordinate center() const;
  /**
   * Return the radius of this arc.
   */
  double radius() const;
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
  const double sectorSurface() const;

  bool equals( const ObjectImp& rhs ) const;

  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const;
  bool internalContainsPoint( const Coordinate& p, double threshold ) const;
};

#endif
