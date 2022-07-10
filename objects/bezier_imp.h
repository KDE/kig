// SPDX-FileCopyrightText: 2009 Petr Gajdos <pgajdos@suse.cz> and
// Maurizio Paolini <paolini@dmf.unicatt.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_BEZIER_IMP_H
#define KIG_OBJECTS_BEZIER_IMP_H

#include "curve_imp.h"

#include "object_imp.h"
#include "../misc/coordinate.h"
#include <vector>

/**
 * An ObjectImp representing polynomial Bézier Curve.
 */
class BezierImp
  : public CurveImp
{
  uint mnpoints;
  std::vector<Coordinate> mpoints;
  Coordinate mcenterofmass;
  
  Coordinate deCasteljau( unsigned int m, unsigned int k, double p ) const;

public:
  typedef CurveImp Parent;
  /**
   * Returns the ObjectImpType representing the BezierImp type.
   */
  static const ObjectImpType* stype();
  static const ObjectImpType* stype2();
  static const ObjectImpType* stype3();

  /**
   * Constructs a Bézier curve.
   */
  explicit BezierImp( const std::vector<Coordinate>& points );
  ~BezierImp();
  BezierImp* copy() const override;

  Coordinate attachPoint() const override;
  ObjectImp* transform( const Transformation& ) const override;

  void draw( KigPainter& p ) const override;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const override;
  bool inRect( const Rect& r, int width, const KigWidget& ) const override;
  bool valid() const;
  Rect surroundingRect() const override;

  const Coordinate getPoint( double param, const KigDocument& ) const override;
  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const override;
  bool internalContainsPoint( const Coordinate& p, double threshold, 
                              const KigDocument& doc ) const;

  int numberOfProperties() const override;
  const QList<KLazyLocalizedString> properties() const override;
  const QByteArrayList propertiesInternalNames() const override;
  ObjectImp* property( int which, const KigDocument& w ) const override;
  const char* iconForProperty( int which ) const override;
  const ObjectImpType* impRequirementForProperty( int which ) const override;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const override;

  const ObjectImpType* type() const override;
  void visit( ObjectImpVisitor* vtor ) const override;

  /**
   * Returns the vector with control points.
   */
  const std::vector<Coordinate> points() const; 
  /**
   * Returns the center of mass of the control polygon.
   */
  const Coordinate centerOfMass() const; 
  /**
   * Returns the number of control points.
   */
  uint npoints() const;

  bool equals( const ObjectImp& rhs ) const override;
};

/**
 * An ObjectImp representing a rational Bézier curve.
 */
class RationalBezierImp
  : public CurveImp
{
  uint mnpoints;
  std::vector<Coordinate> mpoints;
  std::vector<double> mweights;
  Coordinate mcenterofmass;
  
  Coordinate deCasteljauPoints( unsigned int m, unsigned int k, double p ) const;
  double deCasteljauWeights( unsigned int m, unsigned int k, double p ) const;

public:
  typedef CurveImp Parent;
  /**
   * Returns the ObjectImpType representing the RationalBezierImp type.
   */
  static const ObjectImpType* stype();
  static const ObjectImpType* stype2();
  static const ObjectImpType* stype3();

  /**
   * Constructs a rational Bézier curve.
   */
  RationalBezierImp( const std::vector<Coordinate>& points, const std::vector<double>& weights );
  ~RationalBezierImp();
  RationalBezierImp* copy() const override;

  Coordinate attachPoint() const override;
  ObjectImp* transform( const Transformation& ) const override;

  void draw( KigPainter& p ) const override;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const override;
  bool inRect( const Rect& r, int width, const KigWidget& ) const override;
  bool valid() const;
  Rect surroundingRect() const override;

  const Coordinate getPoint( double param, const KigDocument& ) const override;
  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const override;
  bool internalContainsPoint( const Coordinate& p, double threshold, 
                              const KigDocument& doc ) const;

  int numberOfProperties() const override;
  const QList<KLazyLocalizedString> properties() const override;
  const QByteArrayList propertiesInternalNames() const override;
  ObjectImp* property( int which, const KigDocument& w ) const override;
  const char* iconForProperty( int which ) const override;
  const ObjectImpType* impRequirementForProperty( int which ) const override;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const override;

  const ObjectImpType* type() const override;
  void visit( ObjectImpVisitor* vtor ) const override;

  /**
   * Returns the vector with control points.
   */
  const std::vector<Coordinate> points() const; 
  /**
   * Returns the center of mass of the control polygon.
   */
  const Coordinate centerOfMass() const; 
  /**
   * Returns the number of control points.
   */
  uint npoints() const;

  bool equals( const ObjectImp& rhs ) const override;
};


#endif
