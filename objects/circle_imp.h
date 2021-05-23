// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_CIRCLE_IMP_H
#define KIG_OBJECTS_CIRCLE_IMP_H

#include "conic_imp.h"

/**
 * An ObjectImp representing a circle.  This class is a subclass of
 * ConicImp, ensuring that a circle can be used as a conic.
 */
class CircleImp
  : public ConicImp
{
  Coordinate mcenter;
  double mradius;
public:
  typedef ConicImp Parent;
  /**
   * Returns the ObjectImpType representing the CircleImp type.
   */
  static const ObjectImpType* stype();

  /**
   * Construct a Circle with a given center and radius.
   */
  CircleImp( const Coordinate& center, double radius );
  ~CircleImp();
  CircleImp* copy() const Q_DECL_OVERRIDE;

  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;

  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool inRect( const Rect& r, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool valid() const;
  Rect surroundingRect() const Q_DECL_OVERRIDE;

  double getParam( const Coordinate& point, const KigDocument& ) const Q_DECL_OVERRIDE;
  const Coordinate getPoint( double param, const KigDocument& ) const Q_DECL_OVERRIDE;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  /**
   * Return the center of this circle.
   */
  const Coordinate center() const;
  /**
   * Return the radius of this circle.
   */
  double radius() const;
  /**
   * Return the orientation of this circle.
   */
  double orientation() const;
  /**
   * Return the square radius of this circle.  Use this in preference
   * to sqr( radius() ).
   */
  double squareRadius() const;
  /**
   * Return the surface of this circle.
   */
  double surface() const;
  /**
   * Return the circumference of this circle.
   */
  double circumference() const;

  // trivial versions of the conic information functions.
  /**
   * Always returns 1, since a circle always is an ellipse.
   */
  int conicType() const Q_DECL_OVERRIDE;
  const ConicCartesianData cartesianData() const Q_DECL_OVERRIDE;
  const ConicPolarData polarData() const Q_DECL_OVERRIDE;
  /**
   * The first focus of a circle is simply its center.
   */
  Coordinate focus1() const Q_DECL_OVERRIDE;
  /**
   * The second focus of a circle is simply its center.
   */
  Coordinate focus2() const Q_DECL_OVERRIDE;

  /**
   * Return a string containing the cartesian equation of this circle.
   * This will be of the form "x^2 + y^2 + a x + b y + c = 0"
   */
  QString cartesianEquationString( const KigDocument& w ) const Q_DECL_OVERRIDE;
  /**
   * Return a string containing the cartesian equation of this circle.
   * This will be of the form "( x - x0 )^2 + ( y - y0 )^2 = r^2"
   */
  QString simplyCartesianEquationString( const KigDocument& w ) const;
  /**
   * Return a string containing the polar equation of this circle.
   * This will be of the form "rho = r  [centered at p]"
   */
  QString polarEquationString( const KigDocument& w ) const Q_DECL_OVERRIDE;

  bool equals( const ObjectImp& rhs ) const Q_DECL_OVERRIDE;
};

#endif
