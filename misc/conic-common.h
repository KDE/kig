/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Maurizio Paolini <paolini@dmf.unicatt.it>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef KIG_MISC_CONIC_COMMON_H
#define KIG_MISC_CONIC_COMMON_H

#include "coordinate.h"
#include <vector>
#include "kignumerics.h"

class ConicPolarData;
class Transformation;
class LineData;

/**
 * Cartesian Conic Data. This class represents an equation of a conic
 * in the form "ax^2 + by^2 + cxy + dx + ey + f = 0".
 * \internal The coefficients are stored in the order a - f.
 */
class ConicCartesianData
{
public:
  double coeffs[6];
  ConicCartesianData();
  /**
   * Construct a ConicCartesianData from a ConicPolarData.
   * Construct a ConicCartesianData that is the cartesian
   * representation of the conic represented by d.
   */
  explicit ConicCartesianData( const ConicPolarData& d );
  /**
   * Construct a ConicCartesianData from its coefficients
   * Construct a ConicCartesianData using the coefficients a through f
   * from the equation "ax^2 + by^2 + cxy + dx + ey + f = 0"
   */
  ConicCartesianData( double a, double b, double c,
                      double d, double e, double f )
    {
      coeffs[0] = a;
      coeffs[1] = b;
      coeffs[2] = c;
      coeffs[3] = d;
      coeffs[4] = e;
      coeffs[5] = f;
    };
  ConicCartesianData( const double incoeffs[6] );

  /**
   * Invalid conic.
   * Return a ConicCartesianData representing an invalid conic.
   * \see valid()
   */
  static ConicCartesianData invalidData();
  /**
   * Test validity.
   * Return whether this is a valid conic.
   * \see invalidData()
   */
  bool valid() const;
};

/**
 * This class represents an equation of a conic in the form
 * "\rho(\theta) = \frac{p}{1 - e \cos\theta}" ( run this through
 * LaTeX to get nice output ;)  focus and the ecostheta stuff
 * represent the coordinate system in which the equation yields the
 * good result..
 */
class ConicPolarData
{
public:
  /**
   * Construct a ConicPolarData from a ConicCartesianData.
   *
   * Construct a ConicPolarData that is the polar
   * representation of the conic represented by d.
   */
  explicit ConicPolarData( const ConicCartesianData& data );
  explicit ConicPolarData();
  /**
   * Construct a ConicPolarData using the parameters from the equation
   * "\rho(\theta) = \frac{p}{1 - e \cos\theta}"
   */
  ConicPolarData( const Coordinate& focus1, double dimen,
                  double ecostheta0, double esintheta0 );

  /**
   * The first focus of this conic.
   */
  Coordinate focus1;
  /**
   * The pdimen value from the polar equation.
   */
  double pdimen;
  /**
   * The ecostheta0 value from the polar equation.
   */
  double ecostheta0;
  /**
   * The esintheta0 value from the polar equation.
   */
  double esintheta0;
};

bool operator==( const ConicPolarData& lhs, const ConicPolarData& rhs );

/**
 * This function calcs a cartesian conic equation such that the
 * given points are on the conic..  There can be at most 5 and at
 * least 1 point.  If there are less than 5, than the coefficients
 * will be chosen to 1.0 if possible
 */

enum LinearConstraints {
  noconstraint, zerotilt, parabolaifzt, circleifzt,
  equilateral, ysymmetry, xsymmetry
};

const ConicCartesianData calcConicThroughPoints (
    const std::vector<Coordinate>& points,
    const LinearConstraints c1 = noconstraint,
    const LinearConstraints c2 = noconstraint,
    const LinearConstraints c3 = noconstraint,
    const LinearConstraints c4 = noconstraint,
    const LinearConstraints c5 = noconstraint);

/**
 * This function is used by ConicBFFP.  It calcs the polar equation
 * for a hyperbola ( type == -1 ) or ellipse ( type == 1 ) with
 * focuses args[0] and args[1], and with args[2] on the edge of the
 * conic.  args.size() should be two or three.  If it is two, the two
 * points are taken to be the focuses, and a third point is chosen in
 * a sensible way..
 */
const ConicPolarData calcConicBFFP(
  const std::vector<Coordinate>& args,
  int type );

/**
 * function used by ConicBDFP.  It calcs the conic with directrix d,
 * focus f, and point p on the conic..
 */
const ConicPolarData calcConicBDFP(
  const LineData& d, const Coordinate& f, const Coordinate& p );

/**
 * This calcs the hyperbola defined by its two asymptotes line1 and
 * line2, and a point p on the edge.
 */
const ConicCartesianData calcConicByAsymptotes(
  const LineData& line1,
  const LineData& line2,
  const Coordinate& p );

const LineData calcConicPolarLine (
  const ConicCartesianData& data,
  const Coordinate& cpole,
  bool& valid );

const Coordinate calcConicPolarPoint (
  const ConicCartesianData& data,
  const LineData& polar,
  bool& valid );

const Coordinate calcConicLineIntersect( const ConicCartesianData& c,
                                         const LineData& l,
					 double knownparam,
                                         int which, bool& valid );

const LineData calcConicAsymptote(
  const ConicCartesianData data,
  int which, bool &valid );

const LineData calcConicRadical( const ConicCartesianData& cequation1,
                                 const ConicCartesianData& cequation2,
                                 int which, int zeroindex, bool& valid );

const ConicCartesianData calcConicTransformation (
  const ConicCartesianData& data,
  const Transformation& t, bool& valid );

#endif // KIG_MISC_CONIC_COMMON_H
