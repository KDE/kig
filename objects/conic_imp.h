// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_CONIC_IMP_H
#define KIG_OBJECTS_CONIC_IMP_H

#include "curve_imp.h"

#include "../misc/conic-common.h"

/**
 * An ObjectImp representing a conic.
 *
 * A conic is a general second degree curve, and some beautiful theory
 * has been developed about it.  See a math book for more
 * information.  This class is in fact an abstract base class hiding
 * the fact that a ConicImp can be constructed in two ways.  If only
 * its Cartesian equation is known, then you should use ConicImpCart,
 * otherwise, you should use ConicImpPolar.  If the other
 * representation is needed, it will be calculated, but a cartesian
 * representation is rarely needed, and not calculating saves some CPU
 * cycles.
 */
class ConicImp
  : public CurveImp
{
protected:
  ConicImp();
  ~ConicImp();
public:
  typedef CurveImp Parent;
  /**
   * Returns the ObjectImpType representing the ConicImp type.
   */
  static const ObjectImpType* stype();

  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;

  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool inRect( const Rect& r, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool valid() const;
  Rect surroundingRect() const Q_DECL_OVERRIDE;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;

  double getParam( const Coordinate& point, const KigDocument& ) const Q_DECL_OVERRIDE;
  const Coordinate getPoint( double param, const KigDocument& ) const Q_DECL_OVERRIDE;

  // getPoint and getParam do not really need the KigDocument arg...

  double getParam( const Coordinate& point ) const;
  const Coordinate getPoint( double param ) const;

  // information about ourselves.  These are all virtual, because a
  // trivial subclass like CircleImp can override these with trivial
  // versions.

  /**
   * Type of conic.
   * Return what type of conic this is:
   * -1 for a hyperbola
   * 0 for a parabola
   * 1 for an ellipse
   */
  virtual int conicType() const;
  /**
   * A string containing "Hyperbola", "Parabola" or "Ellipse".
   */
  virtual QString conicTypeString() const;
  /**
   * A string containing the cartesian equation of the conic.  This
   * will be of the form "a x^2 + b y^2 + c xy + d x + e y + f = 0".
   */
  virtual QString cartesianEquationString( const KigDocument& w ) const;
  /**
   * A string containing the polar equation of the conic.  This will
   * be of the form "rho = pdimen/(1 + ect cos( theta ) + est sin(
   * theta ) )\n    [centered at p]"
   */
  virtual QString polarEquationString( const KigDocument& w ) const;
  /**
   * Return the cartesian representation of this conic.
   */
  virtual const ConicCartesianData cartesianData() const;
  /**
   * Return the polar representation of this conic.
   */
  virtual const ConicPolarData polarData() const = 0;
  /**
   * Return the center of this conic.
   */
  virtual Coordinate coniccenter() const;
  /**
   * Return the first focus of this conic.
   */
  virtual Coordinate focus1() const;
  /**
   * Return the second focus of this conic.
   */
  virtual Coordinate focus2() const;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  bool equals( const ObjectImp& rhs ) const Q_DECL_OVERRIDE;

  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const Q_DECL_OVERRIDE;
  bool internalContainsPoint( const Coordinate& p, double threshold ) const;
  bool isVerticalParabola( ConicCartesianData& data ) const;
};

/**
 * An implementation of ConicImp to be used when only the cartesian
 * equation of the conic is known.
 */
class ConicImpCart
  : public ConicImp
{
protected:
  ConicCartesianData mcartdata;
  ConicPolarData mpolardata;
public:
  explicit ConicImpCart( const ConicCartesianData& data );
  ~ConicImpCart();
  ConicImpCart* copy() const Q_DECL_OVERRIDE;

  const ConicCartesianData cartesianData() const Q_DECL_OVERRIDE;
  const ConicPolarData polarData() const Q_DECL_OVERRIDE;
};

/**
 * An implementation of ConicImp to be used when only the polar
 * equation of the conic is known.
 */
class ConicImpPolar
  : public ConicImp
{
  ConicPolarData mdata;
public:
  explicit ConicImpPolar( const ConicPolarData& data );
  ~ConicImpPolar();
  ConicImpPolar* copy() const Q_DECL_OVERRIDE;

  const ConicPolarData polarData() const Q_DECL_OVERRIDE;
};

/**
 * A conic arc, which is given by the cartesian equation and two angles
 */
class ConicArcImp
  : public ConicImpCart
{
  double msa;
  double ma;
public:
  typedef CurveImp Parent;
  /**
   * Returns the ObjectImpType representing the ConicImp type.
   */
  static const ObjectImpType* stype();
  /**
   * Construct a Conic Arc with given cartesian equation, start angle and
   * dimension (both in radians).
   */
  ConicArcImp( const ConicCartesianData& data,
               const double startangle, const double angle );
  ~ConicArcImp();

  ConicArcImp* copy() const Q_DECL_OVERRIDE;

  ObjectImp* transform( const Transformation& t ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const Q_DECL_OVERRIDE;
  bool internalContainsPoint( const Coordinate& p, double threshold,
     const KigDocument& doc ) const;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  double getParam( const Coordinate& point, const KigDocument& ) const Q_DECL_OVERRIDE;
  const Coordinate getPoint( double param, const KigDocument& ) const Q_DECL_OVERRIDE;

  double getParam( const Coordinate& point ) const;
  const Coordinate getPoint( double param ) const;

  /**
   * Set the start angle in radians of this arc.
   */
  void setStartAngle( double sa ) { msa = sa; }
  /**
   * Set the dimension in radians of this arc.
   */
  void setAngle( double a ) { ma = a; }
  /**
   * Return the start point of this arc.
   */
  Coordinate firstEndPoint() const;
  /**
   * Return the end point of this arc.
   */
  Coordinate secondEndPoint() const;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
};

#endif
