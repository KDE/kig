// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "cubic-common.h"
#include "kignumerics.h"
#include "kigtransform.h"

#include <config-kig.h>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

/*
 * coefficients of the cartesian equation for cubics
 */

CubicCartesianData::CubicCartesianData()
{
  std::fill( coeffs, coeffs + 10, 0 );
}

CubicCartesianData::CubicCartesianData(
            const double incoeffs[10] )
{
  std::copy( incoeffs, incoeffs + 10, coeffs );
  normalize();
}

void CubicCartesianData::normalize()
{
  double norm = 0.0;
  for ( int i = 0; i < 10; ++i )
  {
    if ( std::fabs( coeffs[i] ) > norm ) norm = std::fabs( coeffs[i] );
  }
  if ( norm < 1e-8 ) return;
  for ( int i = 0; i < 10; ++i )
  {
    coeffs[i] /= norm;
  }
}

const CubicCartesianData calcCubicThroughPoints (
    const std::vector<Coordinate>& points )
{
  // points is a vector of at most 9 points through which the cubic is
  // constrained.
  // this routine should compute the coefficients in the cartesian equation
  // they are defined up to a multiplicative factor.
  // since we don't know (in advance) which one of them is nonzero, we
  // simply keep all 10 parameters, obtaining a 9x10 linear system which
  // we solve using gaussian elimination with complete pivoting
  // If there are too few, then we choose some cool way to fill in the
  // empty parts in the matrix according to the LinearConstraints
  // given..

  // 9 rows, 10 columns..
  double row0[10];
  double row1[10];
  double row2[10];
  double row3[10];
  double row4[10];
  double row5[10];
  double row6[10];
  double row7[10];
  double row8[10];
  double *matrix[9] = {row0, row1, row2, row3, row4, row5, row6, row7, row8};
  double solution[10];
  int scambio[10];

  int numpoints = points.size();
  int numconstraints = 9;

  // fill in the matrix elements
  for ( int i = 0; i < numpoints; ++i )
  {
    double xi = points[i].x;
    double yi = points[i].y;
    matrix[i][0] = 1.0;
    matrix[i][1] = xi;
    matrix[i][2] = yi;
    matrix[i][3] = xi*xi;
    matrix[i][4] = xi*yi;
    matrix[i][5] = yi*yi;
    matrix[i][6] = xi*xi*xi;
    matrix[i][7] = xi*xi*yi;
    matrix[i][8] = xi*yi*yi;
    matrix[i][9] = yi*yi*yi;
  }

  for ( int i = 0; i < numconstraints; i++ )
  {
    if (numpoints >= 9) break;    // don't add constraints if we have enough
    for (int j = 0; j < 10; ++j) matrix[numpoints][j] = 0.0;
    bool addedconstraint = true;
    switch (i)
    {
      case 0:
        matrix[numpoints][7] = 1.0;
        matrix[numpoints][8] = -1.0;
        break;
      case 1:
        matrix[numpoints][7] = 1.0;
	break;
      case 2:
        matrix[numpoints][9] = 1.0;
	break;
      case 3:
        matrix[numpoints][4] = 1.0;
	break;
      case 4:
        matrix[numpoints][5] = 1.0;
	break;
      case 5:
        matrix[numpoints][3] = 1.0;
	break;
      case 6:
        matrix[numpoints][1] = 1.0;
	break;

      default:
        addedconstraint = false;
        break;
    }

    if (addedconstraint) ++numpoints;
  }

  if ( ! GaussianElimination( matrix, numpoints, 10, scambio ) )
    return CubicCartesianData::invalidData();
  // fine della fase di eliminazione
  BackwardSubstitution( matrix, numpoints, 10, scambio, solution );

  // now solution should contain the correct coefficients..
  return CubicCartesianData( solution );
}

const CubicCartesianData calcCubicCuspThroughPoints (
    const std::vector<Coordinate>& points )
{
  // points is a vector of at most 4 points through which the cubic is
  // constrained. Moreover the cubic is required to have a cusp at the
  // origin.

  // 9 rows, 10 columns..
  double row0[10];
  double row1[10];
  double row2[10];
  double row3[10];
  double row4[10];
  double row5[10];
  double row6[10];
  double row7[10];
  double row8[10];
  double *matrix[9] = {row0, row1, row2, row3, row4, row5, row6, row7, row8};
  double solution[10];
  int scambio[10];

  int numpoints = points.size();
  int numconstraints = 9;

  // fill in the matrix elements
  for ( int i = 0; i < numpoints; ++i )
  {
    double xi = points[i].x;
    double yi = points[i].y;
    matrix[i][0] = 1.0;
    matrix[i][1] = xi;
    matrix[i][2] = yi;
    matrix[i][3] = xi*xi;
    matrix[i][4] = xi*yi;
    matrix[i][5] = yi*yi;
    matrix[i][6] = xi*xi*xi;
    matrix[i][7] = xi*xi*yi;
    matrix[i][8] = xi*yi*yi;
    matrix[i][9] = yi*yi*yi;
  }

  for ( int i = 0; i < numconstraints; i++ )
  {
    if (numpoints >= 9) break;    // don't add constraints if we have enough
    for (int j = 0; j < 10; ++j) matrix[numpoints][j] = 0.0;
    bool addedconstraint = true;
    switch (i)
    {
      case 0:
	matrix[numpoints][0] = 1.0;   // through the origin
	break;
      case 1:
	matrix[numpoints][1] = 1.0;
	break;
      case 2:
	matrix[numpoints][2] = 1.0;   // no first degree term
	break;
      case 3:
        matrix[numpoints][3] = 1.0;   // a011 (x^2 coeff) = 0
	break;
      case 4:
        matrix[numpoints][4] = 1.0;   // a012 (xy coeff) = 0
	break;
      case 5:
        matrix[numpoints][7] = 1.0;
        matrix[numpoints][8] = -1.0;
        break;
      case 6:
        matrix[numpoints][7] = 1.0;
	break;
      case 7:
        matrix[numpoints][9] = 1.0;
	break;
      case 8:
        matrix[numpoints][6] = 1.0;
	break;

      default:
        addedconstraint = false;
        break;
    }

    if (addedconstraint) ++numpoints;
  }

  if ( ! GaussianElimination( matrix, numpoints, 10, scambio ) )
    return CubicCartesianData::invalidData();
  // fine della fase di eliminazione
  BackwardSubstitution( matrix, numpoints, 10, scambio, solution );

  // now solution should contain the correct coefficients..
  return CubicCartesianData( solution );
}

const CubicCartesianData calcCubicNodeThroughPoints (
    const std::vector<Coordinate>& points )
{
  // points is a vector of at most 6 points through which the cubic is
  // constrained. Moreover the cubic is required to have a node at the
  // origin.

  // 9 rows, 10 columns..
  double row0[10];
  double row1[10];
  double row2[10];
  double row3[10];
  double row4[10];
  double row5[10];
  double row6[10];
  double row7[10];
  double row8[10];
  double *matrix[9] = {row0, row1, row2, row3, row4, row5, row6, row7, row8};
  double solution[10];
  int scambio[10];

  int numpoints = points.size();
  int numconstraints = 9;

  // fill in the matrix elements
  for ( int i = 0; i < numpoints; ++i )
  {
    double xi = points[i].x;
    double yi = points[i].y;
    matrix[i][0] = 1.0;
    matrix[i][1] = xi;
    matrix[i][2] = yi;
    matrix[i][3] = xi*xi;
    matrix[i][4] = xi*yi;
    matrix[i][5] = yi*yi;
    matrix[i][6] = xi*xi*xi;
    matrix[i][7] = xi*xi*yi;
    matrix[i][8] = xi*yi*yi;
    matrix[i][9] = yi*yi*yi;
  }

  for ( int i = 0; i < numconstraints; i++ )
  {
    if (numpoints >= 9) break;    // don't add constraints if we have enough
    for (int j = 0; j < 10; ++j) matrix[numpoints][j] = 0.0;
    bool addedconstraint = true;
    switch (i)
    {
      case 0:
	matrix[numpoints][0] = 1.0;
	break;
      case 1:
	matrix[numpoints][1] = 1.0;
	break;
      case 2:
	matrix[numpoints][2] = 1.0;
	break;
      case 3:
        matrix[numpoints][7] = 1.0;
        matrix[numpoints][8] = -1.0;
        break;
      case 4:
        matrix[numpoints][7] = 1.0;
	break;
      case 5:
        matrix[numpoints][9] = 1.0;
	break;
      case 6:
        matrix[numpoints][4] = 1.0;
	break;
      case 7:
        matrix[numpoints][5] = 1.0;
	break;
      case 8:
        matrix[numpoints][3] = 1.0;
	break;

      default:
        addedconstraint = false;
        break;
    }

    if (addedconstraint) ++numpoints;
  }

  if ( ! GaussianElimination( matrix, numpoints, 10, scambio ) )
    return CubicCartesianData::invalidData();
  // fine della fase di eliminazione
  BackwardSubstitution( matrix, numpoints, 10, scambio, solution );

  // now solution should contain the correct coefficients..
  return CubicCartesianData( solution );
}

/*
 * computation of the y value corresponding to some x value
 */

double calcCubicYvalue ( double x, double ymin, double ymax, int root,
                         const CubicCartesianData &data, bool& valid,
                         int &numroots )
{
  valid = true;

  // compute the third degree polinomial:
  double a000 = data.coeffs[0];
  double a001 = data.coeffs[1];
  double a002 = data.coeffs[2];
  double a011 = data.coeffs[3];
  double a012 = data.coeffs[4];
  double a022 = data.coeffs[5];
  double a111 = data.coeffs[6];
  double a112 = data.coeffs[7];
  double a122 = data.coeffs[8];
  double a222 = data.coeffs[9];

  // first the y^3 coefficient, it coming only from a222:
  double a = a222;
  // next the y^2 coefficient (from a122 and a022):
  double b = a122*x + a022;
  // next the y coefficient (from a112, a012 and a002):
  double c = a112*x*x + a012*x + a002;
  // finally the constant coefficient (from a111, a011, a001 and a000):
  double d = a111*x*x*x + a011*x*x + a001*x + a000;

  return calcCubicRoot ( ymin, ymax, a, b, c, d, root, valid, numroots );
}

const Coordinate calcCubicLineIntersect( const CubicCartesianData& cu,
                                         const LineData& l,
                                         int root, bool& valid )
{
  assert( root == 1 || root == 2 || root == 3 );

  double a, b, c, d;
  calcCubicLineRestriction ( cu, l.a, l.b-l.a, a, b, c, d );
  int numroots;
  double param =
    calcCubicRoot ( -1e10, 1e10, a, b, c, d, root, valid, numroots );
  return l.a + param*(l.b - l.a);
}

/*
 * calculate the cubic polynomial resulting from the restriction
 * of a cubic to a line (defined by two "Coordinates": a point and a
 * direction)
 */

void calcCubicLineRestriction ( const CubicCartesianData &data,
                                const Coordinate &p, const Coordinate &v,
                                double& a, double& b, double& c, double& d )
{
  a = b = c = d = 0;

  double a000 = data.coeffs[0];
  double a001 = data.coeffs[1];
  double a002 = data.coeffs[2];
  double a011 = data.coeffs[3];
  double a012 = data.coeffs[4];
  double a022 = data.coeffs[5];
  double a111 = data.coeffs[6];
  double a112 = data.coeffs[7];
  double a122 = data.coeffs[8];
  double a222 = data.coeffs[9];

  // zero degree term
  d += a000;

  // first degree terms
  d += a001*p.x + a002*p.y;
  c += a001*v.x + a002*v.y;

  // second degree terms
  d +=   a011*p.x*p.x + a012*p.x*p.y             +   a022*p.y*p.y;
  c += 2*a011*p.x*v.x + a012*(p.x*v.y + v.x*p.y) + 2*a022*p.y*v.y;
  b +=   a011*v.x*v.x + a012*v.x*v.y             +   a022*v.y*v.y;

  // third degree terms: a111 x^3 + a222 y^3
  d +=    a111*p.x*p.x*p.x + a222*p.y*p.y*p.y;
  c += 3*(a111*p.x*p.x*v.x + a222*p.y*p.y*v.y);
  b += 3*(a111*p.x*v.x*v.x + a222*p.y*v.y*v.y);
  a +=    a111*v.x*v.x*v.x + a222*v.y*v.y*v.y;

  // third degree terms: a112 x^2 y + a122 x y^2
  d += a112*p.x*p.x*p.y + a122*p.x*p.y*p.y;
  c += a112*(p.x*p.x*v.y + 2*p.x*v.x*p.y) + a122*(v.x*p.y*p.y + 2*p.x*p.y*v.y);
  b += a112*(v.x*v.x*p.y + 2*v.x*p.x*v.y) + a122*(p.x*v.y*v.y + 2*v.x*v.y*p.y);
  a += a112*v.x*v.x*v.y + a122*v.x*v.y*v.y;
}


const CubicCartesianData calcCubicTransformation (
  const CubicCartesianData& data,
  const Transformation& t, bool& valid )
{
  double a[3][3][3];
  double b[3][3][3];
  CubicCartesianData dataout;

  int icount = 0;
  for (int i=0; i < 3; i++)
  {
    for (int j=i; j < 3; j++)
    {
      for (int k=j; k < 3; k++)
      {
	a[i][j][k] = data.coeffs[icount++];
	if ( i < k )
	{
	  if ( i == j )    // case aiik
	  {
	    a[i][i][k] /= 3.;
	    a[i][k][i] = a[k][i][i] = a[i][i][k];
	  }
	  else if ( j == k )  // case aijj
	  {
	    a[i][j][j] /= 3.;
	    a[j][i][j] = a[j][j][i] = a[i][j][j];
	  }
	  else             // case aijk  (i<j<k)
	  {
	    a[i][j][k] /= 6.;
	    a[i][k][j] = a[j][i][k] = a[j][k][i] =
                         a[k][i][j] = a[k][j][i] = a[i][j][k];
	  }
	}
      }
    }
  }

  Transformation ti = t.inverse( valid );
  if ( ! valid ) return dataout;

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      for (int k = 0; k < 3; k++)
      {
	b[i][j][k] = 0.;
        for (int ii = 0; ii < 3; ii++)
        {
          for (int jj = 0; jj < 3; jj++)
          {
            for (int kk = 0; kk < 3; kk++)
            {
	      b[i][j][k] += a[ii][jj][kk]*ti.data( ii, i )*ti.data( jj, j )*ti.data( kk, k );
	    }
	  }
	}
      }
    }
  }

//   assert (fabs(b[0][1][2] - b[1][2][0]) < 1e-8);  // test a couple of cases
//   assert (fabs(b[0][1][1] - b[1][1][0]) < 1e-8);

  // apparently, the above assertions are wrong ( due to rounding
  // errors, Maurizio and I hope :) ), so since the symmetry is not
  // present, we just take the sum of the parts of the matrix elements
  // that should be symmetric, instead of taking one of them, and
  // multiplying it..

  dataout.coeffs[0] = b[0][0][0];
  dataout.coeffs[1] = b[0][0][1] + b[0][1][0] + b[1][0][0];
  dataout.coeffs[2] = b[0][0][2] + b[0][2][0] + b[2][0][0];
  dataout.coeffs[3] = b[0][1][1] + b[1][0][1] + b[1][1][0];
  dataout.coeffs[4] = b[0][1][2] + b[0][2][1] + b[1][2][0] + b[1][0][2] + b[2][1][0] + b[2][0][1];
  dataout.coeffs[5] = b[0][2][2] + b[2][0][2] + b[2][2][0];
  dataout.coeffs[6] = b[1][1][1];
  dataout.coeffs[7] = b[1][1][2] + b[1][2][1] + b[2][1][1];
  dataout.coeffs[8] = b[1][2][2] + b[2][1][2] + b[2][2][1];
  dataout.coeffs[9] = b[2][2][2];

  return dataout;
}

bool operator==( const CubicCartesianData& lhs, const CubicCartesianData& rhs )
{
  for ( int i = 0; i < 10; ++i )
    if ( lhs.coeffs[i] != rhs.coeffs[i] )
      return false;
  return true;
}

CubicCartesianData CubicCartesianData::invalidData()
{
  CubicCartesianData ret;
  ret.coeffs[0] = double_inf;
  return ret;
}

bool CubicCartesianData::valid() const
{
  return std::isfinite( coeffs[0] );
}
