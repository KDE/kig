// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MISC_CUBIC_COMMON_H
#define KIG_MISC_CUBIC_COMMON_H

#include "common.h"

class Transformation;

/**
 * This class represents an equation of a cubic in the form
 * \f$ a_{ijk} x_i x_j x_k = 0 \f$ (in homogeneous coordinates,
 * \f$ i,j,k = 0,1,2 \f$), \f$ i <= j <= k \f$.
 * The coefficients are stored in lessicografic order.
 */
class CubicCartesianData
{
public:
  double coeffs[10];
  /**
   * \ifnot creating-python-scripting-doc
   * \brief Default Constructor
   *
   * Constructs a new CubicCartesianData, with all the coeffs
   * initialized to 0.
   * \endif
   */
  explicit CubicCartesianData();
  /**
   * Constructor. Construct a new CubicCartesianData, with the given
   * values as coeffs.
   */
  CubicCartesianData( double a000, double a001, double a002,
                      double a011, double a012, double a022,
                      double a111, double a112, double a122,
                      double a222 )
    {
      coeffs[0] = a000;
      coeffs[1] = a001;
      coeffs[2] = a002;
      coeffs[3] = a011;
      coeffs[4] = a012;
      coeffs[5] = a022;
      coeffs[6] = a111;
      coeffs[7] = a112;
      coeffs[8] = a122;
      coeffs[9] = a222;
    }
  explicit CubicCartesianData( const double incoeffs[10] );

  /**
   * Create an invalid CubicCartesianData. This is a special state of a
   * CubicCartesianData that signals that something went wrong..
   *
   * \see CubicCartesianData::valid
   *
   * \internal We represent an invalid CubicCartesianData by setting all
   * the coeffs to positive or negative infinity. This is handy, since
   * it doesn't require us to adapt most of the functions, it doesn't
   * need extra space, and most of the times that we should get an
   * invalid CubicCartesianData, we get one automatically..
   */
  static CubicCartesianData invalidData();
  /**
   * Return whether this is a valid CubicCartesianData.
   *
   * \see CubicCartesianData::invalidData
   */
  bool valid() const;
  /*
   * normalize mdata with infinity norm of the coefficients = 1
   */
  void normalize();
};

bool operator==( const CubicCartesianData& lhs, const CubicCartesianData& rhs );

/**
 * This function calcs a cartesian cubic equation such that the
 * given points are on the cubic.  There can be at most 9 and at
 * least 2 point.  If there are less than 9, than the coefficients
 * will be chosen to 1.0 if possible
 */
const CubicCartesianData calcCubicThroughPoints (
    const std::vector<Coordinate>& points );

const CubicCartesianData calcCubicCuspThroughPoints (
    const std::vector<Coordinate>& points );

const CubicCartesianData calcCubicNodeThroughPoints (
    const std::vector<Coordinate>& points );

double calcCubicYvalue ( double x, double ymin, double ymax,
                         int root, const CubicCartesianData &data,
                         bool& valid, int& numroots );

const Coordinate calcCubicLineIntersect( const CubicCartesianData& c,
                                         const LineData& l,
                                         int root, bool& valid );

void calcCubicLineRestriction ( const CubicCartesianData &data,
         const Coordinate &p1, const Coordinate &dir,
         double& a, double& b, double& c, double& d );

const CubicCartesianData calcCubicTransformation (
  const CubicCartesianData& data,
  const Transformation& t, bool& valid );

#endif
