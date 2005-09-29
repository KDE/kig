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

#ifndef KIG_OBJECTS_CONIC_IMP_H
#define KIG_OBJECTS_CONIC_IMP_H

#include "curve_imp.h"

#include "../misc/conic-common.h"

/**
 * An ObjectImp representing a conic.
 *
 * A conic is a general second degree curve, and some beautiful theory
 * has been developed about it..  See a math book for more
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

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;
  bool valid() const;
  Rect surroundingRect() const;

  const uint numberOfProperties() const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( uint which ) const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  const char* iconForProperty( uint which ) const;
  ObjectImp* property( uint which, const KigDocument& w ) const;

  double getParam( const Coordinate& point, const KigDocument& ) const;
  const Coordinate getPoint( double param, const KigDocument& ) const;

  // information about ourselves..  These are all virtual, because a
  // trivial subclass like CircleImp can override these with trivial
  // versions..

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
   * Return the first focus of this conic.
   */
  virtual Coordinate focus1() const;
  /**
   * Return the second focus of this conic.
   */
  virtual Coordinate focus2() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool equals( const ObjectImp& rhs ) const;

  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const;
  bool internalContainsPoint( const Coordinate& p, double threshold ) const;
};

/**
 * An implementation of ConicImp to be used when only the cartesian
 * equation of the conic is known.
 */
class ConicImpCart
  : public ConicImp
{
  ConicCartesianData mcartdata;
  ConicPolarData mpolardata;
public:
  ConicImpCart( const ConicCartesianData& data );
  ~ConicImpCart();
  ConicImpCart* copy() const;

  const ConicCartesianData cartesianData() const;
  const ConicPolarData polarData() const;
};

/**
 * An implementation of ConicImp to be used when only the cartesian
 * equation of the conic is known.
 */
class ConicImpPolar
  : public ConicImp
{
  ConicPolarData mdata;
public:
  ConicImpPolar( const ConicPolarData& data );
  ~ConicImpPolar();
  ConicImpPolar* copy() const;

  const ConicPolarData polarData() const;
};

#endif
