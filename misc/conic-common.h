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
#include "common.h"

struct ConicPolarEquationData;

/**
 * This class represents an equation of a conic in the form
 * "ax^2 + by^2 + cxy + dx + ey + f = 0".  The coefficients are stored
 * in the order a - f.
 */
struct ConicCartesianEquationData
{
  double coeffs[6];
public:
  explicit ConicCartesianEquationData();
  explicit ConicCartesianEquationData( const ConicPolarEquationData& d );
  ConicCartesianEquationData( double a, double b, double c,
                              double d, double e, double f )
    {
      coeffs[0] = a;
      coeffs[1] = b;
      coeffs[2] = c;
      coeffs[3] = d;
      coeffs[4] = e;
      coeffs[5] = f;
    };
  ConicCartesianEquationData( const double incoeffs[6] )
    {
      std::copy( incoeffs, incoeffs + 6, coeffs );
    };
};

/**
 * This class represents an equation of a conic in the form
 * "\rho(\theta) = \frac{p}{1 - e \cos\theta}" ( run this through
 * LaTeX to get nice output ;)  focus and the ecostheta stuff
 * represent the coordinate system in which the equation yields the
 * good result..
 */
struct ConicPolarEquationData
{
  explicit ConicPolarEquationData( const ConicCartesianEquationData& data );
  explicit ConicPolarEquationData();
  ConicPolarEquationData( const Coordinate& focus1, double dimen,
                          double ecostheta0, double esintheta0 );

  Coordinate focus1;
  double pdimen;
  double ecostheta0;
  double esintheta0;
};

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

const ConicCartesianEquationData calcConicThroughPoints (
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
const ConicPolarEquationData calcConicBFFP(
  const std::vector<Coordinate>& args,
  int type );

const LineData calcConicPolarLine (
  const ConicCartesianEquationData& data,
  const Coordinate& cpole,
  bool& valid );

const Coordinate calcConicPolarPoint (
  const ConicCartesianEquationData& data,
  const LineData& polar,
  bool& valid );

const Coordinate calcConicLineIntersect( const ConicCartesianEquationData c,
                                         const LineData& l,
                                         int which, bool& valid );

#endif // KIG_MISC_CONIC_COMMON_H
