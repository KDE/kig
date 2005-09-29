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
   * Returns the ObjectImpType representing the CircleImp type..
   */
  static const ObjectImpType* stype();

  /**
   * Construct a Circle with a given center and radius.
   */
  CircleImp( const Coordinate& center, double radius );
  ~CircleImp();
  CircleImp* copy() const;

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;
  bool valid() const;
  Rect surroundingRect() const;

  double getParam( const Coordinate& point, const KigDocument& ) const;
  const Coordinate getPoint( double param, const KigDocument& ) const;

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
   * Return the center of this circle.
   */
  const Coordinate center() const;
  /**
   * Return the radius of this circle.
   */
  double radius() const;
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

  // trivial versions of the conic information functions..
  /**
   * Always returns 1, since a circle always is an ellipse.
   */
  int conicType() const;
  const ConicCartesianData cartesianData() const;
  const ConicPolarData polarData() const;
  /**
   * The first focus of a circle is simply its center.
   */
  Coordinate focus1() const;
  /**
   * The second focus of a circle is simply its center.
   */
  Coordinate focus2() const;

  /**
   * Return a string containing the cartesian equation of this circle.
   * This will be of the form "x^2 + y^2 + a x + b y + c = 0"
   */
  QString cartesianEquationString( const KigDocument& w ) const;
  /**
   * Return a string containing the cartesian equation of this circle.
   * This will be of the form "( x - x0 )^2 + ( y - y0 )^2 = r^2"
   */
  QString simplyCartesianEquationString( const KigDocument& w ) const;
  /**
   * Return a string containing the polar equation of this circle.
   * This will be of the form "rho = r  [centered at p]"
   */
  QString polarEquationString( const KigDocument& w ) const;

  bool equals( const ObjectImp& rhs ) const;
};

#endif
