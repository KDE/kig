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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "kignumerics.h"
#include "common.h"

/*
 * compute one of the roots of a cubic polynomial
 * if xmin << 0 or xmax >> 0 then autocompute a bound for all the
 * roots
 */

double calcCubicRoot ( double xmin, double xmax, double a,
      double b, double c, double d, int root, bool& valid, int& numroots )
{
  // renormalize: positive a and infinity norm = 1

  double infnorm = fabs(a);
  if ( infnorm < fabs(b) ) infnorm = fabs(b);
  if ( infnorm < fabs(c) ) infnorm = fabs(c);
  if ( infnorm < fabs(d) ) infnorm = fabs(d);
  if ( a < 0 ) infnorm = -infnorm;
  a /= infnorm;
  b /= infnorm;
  c /= infnorm;
  d /= infnorm;

  const double small = 1e-7;
  valid = false;
  if ( fabs(a) < small )
  {
    if ( fabs(b) < small )
    {
      if ( fabs(c) < small )
      { // degree = 0;
	numroots = 0;
	return 0.0;
      }
      // degree = 1
      double rootval = -d/c;
      numroots = 1;
      if ( rootval < xmin || xmax < rootval ) numroots--;
      if ( root > numroots ) return 0.0;
      valid = true;
      return rootval;
    }
    // degree = 2
    if ( b < 0 ) { b = -b; c = -c; d = -d; }
    double discrim = c*c - 4*b*d;
    numroots = 2;
    if ( discrim < 0 )
    {
      numroots = 0;
      return 0.0;
    }
    discrim = sqrt(discrim)/(2*fabs(b));
    double rootmiddle = -c/(2*b);
    if ( rootmiddle - discrim < xmin ) numroots--;
    if ( rootmiddle + discrim > xmax ) numroots--;
    if ( rootmiddle + discrim < xmin ) numroots--;
    if ( rootmiddle - discrim > xmax ) numroots--;
    if ( root > numroots ) return 0.0;
    valid = true;
    if ( root == 2 || rootmiddle - discrim < xmin ) return rootmiddle + discrim;
    return rootmiddle - discrim;
  }

  if ( xmin < -1e8 || xmax > 1e8 )
  {

    // compute a bound for all the real roots:

    xmax = fabs(d/a);
    if ( fabs(c/a) + 1 > xmax ) xmax = fabs(c/a) + 1;
    if ( fabs(b/a) + 1 > xmax ) xmax = fabs(b/a) + 1;
    xmin = -xmax;
  }

  // computing the coefficients of the Sturm sequence
  double p1a = 2*b*b - 6*a*c;
  double p1b = b*c - 9*a*d;
  double p0a = c*p1a*p1a + p1b*(3*a*p1b - 2*b*p1a);

  int varbottom = calcCubicVariations (xmin, a, b, c, d, p1a, p1b, p0a);
  int vartop = calcCubicVariations (xmax, a, b, c, d, p1a, p1b, p0a);
  numroots = vartop - varbottom;
  valid = false;
  if (root <= varbottom || root > vartop ) return 0.0;

  valid = true;

  // now use bisection to separate the required root
  double dx = (xmax - xmin)/2;
  while ( vartop - varbottom > 1 )
  {
    if ( fabs( dx ) < 1e-8 ) return (xmin + xmax)/2;
    double xmiddle = xmin + dx;
    int varmiddle = calcCubicVariations (xmiddle, a, b, c, d, p1a, p1b, p0a);
    if ( varmiddle < root )   // I am below
    {
      xmin = xmiddle;
      varbottom = varmiddle;
    } else {
      xmax = xmiddle;
      vartop = varmiddle;
    }
    dx /= 2;
  }

  /*
   * now [xmin, xmax] enclose a single root, try using Newton
   */
  if ( vartop - varbottom == 1 )
  {
    double fval1 = a;     // double check...
    double fval2 = a;
    fval1 = b + xmin*fval1;
    fval2 = b + xmax*fval2;
    fval1 = c + xmin*fval1;
    fval2 = c + xmax*fval2;
    fval1 = d + xmin*fval1;
    fval2 = d + xmax*fval2;
    assert ( fval1 * fval2 <= 0 );
    return calcCubicRootwithNewton ( xmin, xmax, a, b, c, d, 1e-8 );
  }
  else   // probably a double root here!
    return ( xmin + xmax )/2;
}

/*
 * computation of the number of sign changes in the sturm sequence for
 * a third degree polynomial at x.  This number counts the number of
 * roots of the polynomial on the left of point x.
 *
 * a, b, c, d: coefficients of the third degree polynomial (a*x^3 + ...)
 *
 * the second degree polynomial in the sturm sequence is just minus the
 * derivative, so we don't need to compute it.
 *
 * p1a*x + p1b: is the third (first degree) polynomial in the sturm sequence.
 *
 * p0a: is the (constant) fourth polynomial of the sturm sequence.
 */

int calcCubicVariations (double x, double a, double b, double c,
      double d, double p1a, double p1b, double p0a)
{
  double fval, fpval;
  fval = fpval = a;
  fval = b + x*fval;
  fpval = fval + x*fpval;
  fval = c + x*fval;
  fpval = fval + x*fpval;
  fval = d + x*fval;

  double f1val = p1a*x + p1b;

  bool f3pos = fval >= 0;
  bool f2pos = fpval <= 0;
  bool f1pos = f1val >= 0;
  bool f0pos = p0a >= 0;

  int variations = 0;
  if ( f3pos != f2pos ) variations++;
  if ( f2pos != f1pos ) variations++;
  if ( f1pos != f0pos ) variations++;
  return variations;
}

/*
 * use newton to solve a third degree equation with already isolated
 * root
 */

inline void calcCubicDerivatives ( double x, double a, double b, double c,
      double d, double& fval, double& fpval, double& fppval )
{
  fval = fpval = fppval = a;
  fval = b + x*fval;
  fpval = fval + x*fpval;
  fppval = fpval + x*fppval;   // this is really half the second derivative
  fval = c + x*fval;
  fpval = fval + x*fpval;
  fval = d + x*fval;
}

double calcCubicRootwithNewton ( double xmin, double xmax, double a,
      double b, double c, double d, double tol )
{
  double fval, fpval, fppval;

  double fval1, fval2, fpval1, fpval2, fppval1, fppval2;
  calcCubicDerivatives ( xmin, a, b, c, d, fval1, fpval1, fppval1 );
  calcCubicDerivatives ( xmax, a, b, c, d, fval2, fpval2, fppval2 );
  assert ( fval1 * fval2 <= 0 );

  assert ( xmax > xmin );
  while ( xmax - xmin > tol )
  {
    // compute the values of function, derivative and second derivative:
    assert ( fval1 * fval2 <= 0 );
    if ( fppval1 * fppval2 < 0 || fpval1 * fpval2 < 0 )
    {
      double xmiddle = (xmin + xmax)/2;
      calcCubicDerivatives ( xmiddle, a, b, c, d, fval, fpval, fppval );
      if ( fval1*fval <= 0 )
      {
        xmax = xmiddle;
	fval2 = fval;
	fpval2 = fpval;
	fppval2 = fppval;
      } else {
        xmin = xmiddle;
	fval1 = fval;
	fpval1 = fpval;
	fppval1 = fppval;
      }
    } else
    {
      // now we have first and second derivative of constant sign, we
      // can start with Newton from the Fourier point.
      double x = xmin;
      if ( fval2*fppval2 > 0 ) x = xmax;
      double p = 1.0;
      int iterations = 0;
      while ( fabs(p) > tol && iterations++ < 100 )
      {
        calcCubicDerivatives ( x, a, b, c, d, fval, fpval, fppval );
        p = fval/fpval;
        x -= p;
      }
      if( iterations >= 100 )
      {
        // Newton scheme did not converge..
        // we should end up with an invalid Coordinate
        return double_inf;
      };
      return x;
    }
  }

  // we cannot apply Newton, (perhaps we are at an inflection point)

  return (xmin + xmax)/2;
}

/*
 * This function computes the LU factorization of a mxn matrix, with
 * m typically less then n.  This is done with complete pivoting; the
 * exchanges in columns are recorded in the integer vector "exchange"
 */
bool GaussianElimination( double *matrix[], int numrows,
                          int numcols, int exchange[] )
{
  // start gaussian elimination
  for ( int k = 0; k < numrows; ++k )
  {
    // ricerca elemento di modulo massimo
    double maxval = -double_inf;
    int imax = k;
    int jmax = k;
    for( int i = k; i < numrows; ++i )
    {
      for( int j = k; j < numcols; ++j )
      {
        if (fabs(matrix[i][j]) > maxval)
        {
          maxval = fabs(matrix[i][j]);
          imax = i;
          jmax = j;
        }
      }
    }

    // row exchange
    if ( imax != k )
      for( int j = k; j < numcols; ++j )
      {
        double t = matrix[k][j];
        matrix[k][j] = matrix[imax][j];
        matrix[imax][j] = t;
      }

    // column exchange
    if ( jmax != k )
      for( int i = 0; i < numrows; ++i )
      {
        double t = matrix[i][k];
        matrix[i][k] = matrix[i][jmax];
        matrix[i][jmax] = t;
      }

    // remember this column exchange at step k
    exchange[k] = jmax;

    // we can't usefully eliminate a singular matrix..
    if ( maxval == 0. ) return false;

    // ciclo sulle righe
    for( int i = k+1; i < numrows; ++i)
    {
      double mik = matrix[i][k]/matrix[k][k];
      matrix[i][k] = mik;    //ricorda il moltiplicatore... (not necessary)
      // ciclo sulle colonne
      for( int j = k+1; j < numcols; ++j )
      {
        matrix[i][j] -= mik*matrix[k][j];
      }
    }
  }
  return true;
}

/*
 * solve an undetermined homogeneous triangular system. the matrix is nonzero
 * on its diagonal. The last unknown(s) are chosen to be 1. The
 * vector "exchange" contains exchanges to be performed on the
 * final solution components.
 */

void BackwardSubstitution( double *matrix[], int numrows, int numcols,
        int exchange[], double solution[] )
{
  // the system is homogeneous and underdetermined, the last unknown(s)
  // are chosen = 1
  for ( int j = numrows; j < numcols; ++j )
  {
    solution[j] = 1.0;          // other choices are possible here
  };

  for( int k = numrows - 1; k >= 0; --k )
  {
    // backward substitution
    solution[k] = 0.0;
    for ( int j = k+1; j < numcols; ++j)
    {
      solution[k] -= matrix[k][j]*solution[j];
    }
    solution[k] /= matrix[k][k];
  }

  // ultima fase: riordinamento incognite

  for( int k = numrows - 1; k >= 0; --k )
  {
    int jmax = exchange[k];
    double t = solution[k];
    solution[k] = solution[jmax];
    solution[jmax] = t;
  }
}

bool Invert3by3matrix ( const double m[3][3], double inv[3][3] )
{
  double det = m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1]) -
               m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0]) +
               m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);
  if (det == 0) return false;

  for (int i=0; i < 3; i++)
  {
    for (int j=0; j < 3; j++)
    {
      int i1 = (i+1)%3;
      int i2 = (i+2)%3;
      int j1 = (j+1)%3;
      int j2 = (j+2)%3;
      inv[j][i] = (m[i1][j1]*m[i2][j2] - m[i1][j2]*m[i2][j1])/det;
    }
  }
  return true;
}
