/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Maurizio Paolini <paolini@dmf.unicatt.it>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
    }
  explicit ConicCartesianData( const double incoeffs[6] );

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
 * \f$ \rho(\theta) = \frac{p}{1 - e \cos\theta}\f$. focus and the
 * ecostheta stuff represent the coordinate system in which the
 * equation yields the good result..
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
   * \f$ \rho(\theta) = \frac{p}{1 - e \cos\theta}\f$
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
 * These are the constraint values that can be passed to the
 * calcConicThroughPoints function.  Their meaning is as follows:
 * noconstraint: no additional points will be calculated.
 * zerotilt: force the symmetry axes to be parallel to the coordinate
 *           system ( zero tilt ).
 * parabolaifzt: the returned conic should be a parabola ( if used in
 *               combination with zerotilt )
 * circleifzt: the returned conic should be a circle ( if used in
 *             combination with zerotilt )
 * equilateral: the returned conic should be equilateral
 * ysymmetry: the returned conic should be symmetric over the Y-axis.
 * xsymmetry: the returned conic should be symmetric over the X-axis.
 */
enum LinearConstraints {
  noconstraint, zerotilt, parabolaifzt, circleifzt,
  equilateral, ysymmetry, xsymmetry
};

/**
 * Calculate a conic through a given set of points.  points should
 * contain at least one, and at most five points.  If there are five
 * points, then the conic is completely defined.  If there are less,
 * then additional points will be calculated according to the
 * constraints given.  See above for the various constraints.
 *
 * An invalid ConicCartesianData is returned if there is no conic
 * through the given set of points, or if not enough constraints are
 * given for a conic to be calculated.
 */
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

/**
 * This function calculates the polar line of the point cpole with
 * respect to the given conic data.  As the last argument, you should
 * pass a reference to a boolean.  This boolean will be set to true if
 * the returned LineData is valid, and to false if the returned line
 * is not valid.  The latter condition only occurs if a "line at
 * infinity" would have had to be returned.
 */
const LineData calcConicPolarLine (
  const ConicCartesianData& data,
  const Coordinate& cpole,
  bool& valid );

/**
 * This function calculates the polar point of the line polar with
 * respect to the given conic data.  As the last argument, you should
 * pass a reference to a boolean.  This boolean will be set to true if
 * the returned LineData is valid, and to false if the returned line
 * is not valid.  The latter condition only occurs if a "point at
 * infinity" would have had to be returned.
 */
const Coordinate calcConicPolarPoint (
  const ConicCartesianData& data,
  const LineData& polar );

/**
 * This function calculates the intersection of a given line ( l ) and
 * a given conic ( c ).  A line and a conic have two intersections in
 * general, and as such, which should be set to -1 or 1 depending on
 * which intersection you want.  As the last argument, you should pass
 * a reference to a boolean.  This boolean will be set to true if the
 * returned point is valid, and to false if the returned point is not
 * valid.  The latter condition only occurs if the given conic and
 * line do not have the specified intersection.
 *
 * knownparam is something special:  If you already know one
 * intersection of the line and the conic, and you want the other one,
 * then you should set which to 0, knownparam to the curve parameter
 * of the point you already know ( i.e. the value returned by
 * conicimp->getParam( otherpoint ) ).
 */
const Coordinate calcConicLineIntersect( const ConicCartesianData& c,
                                         const LineData& l,
					 double knownparam,
                                         int which );

/**
 * This function calculates the asymptote of the given conic ( data ).
 * A conic has two asymptotes in general, so which should be set to +1
 * or -1 depending on which asymptote you want.  As the last argument,
 * you should pass a reference to a boolean.  This boolean will be set
 * to true if the returned line is valid, and to false if the returned
 * line is not valid.  The latter condition only occurs if the given
 * conic does not have the specified asymptote.
 */
const LineData calcConicAsymptote(
  const ConicCartesianData &data,
  int which, bool &valid );

/**
 * This function calculates the radical line of two conics.  A radical
 * line is the line that goes through two of the intersections of two
 * conics.  Since two conics have up to four intersections in general,
 * there are three sets of two radical lines.  zeroindex specifies
 * which set of radical lines you want ( set it to 1, 2 or 3 ), and
 * which is set to -1 or +1 depending on which of the two radical
 * lines in the set you want.  As the last argument, you should pass a
 * reference to a boolean.  This boolean will be set to true if the
 * returned line is valid, and to false if the returned line is not
 * valid.  The latter condition only occurs if the given conics do not
 * have the specified radical line.
 */
const LineData calcConicRadical( const ConicCartesianData& cequation1,
                                 const ConicCartesianData& cequation2,
                                 int which, int zeroindex, bool& valid );

/**
 * This calculates the image of the given conic ( data ) through the
 * given transformation ( t ).  As the last argument, you should pass
 * a reference to a boolean.  This boolean will be set to true if the
 * returned line is valid, and to false if the returned line is not
 * valid.  The latter condition only occurs if the given
 * transformation is singular, and as such, the transformation of the
 * conic cannot be calculated.
 */
const ConicCartesianData calcConicTransformation (
  const ConicCartesianData& data,
  const Transformation& t, bool& valid );

#endif // KIG_MISC_CONIC_COMMON_H
