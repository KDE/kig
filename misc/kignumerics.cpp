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

#include "kignumerics.h"
#include "common.h"

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
      double b, double c, double d, double tol, bool &valid )
{
  double fval, fpval, fppval;

  assert ( xmax > xmin );
  while ( xmax - xmin > tol )
  {
    // compute the values of function, derivative and second derivative:
    double fval1, fval2, fpval1, fpval2, fppval1, fppval2;
    calcCubicDerivatives ( xmin, a, b, c, d, fval1, fpval1, fppval1 );
    calcCubicDerivatives ( xmax, a, b, c, d, fval2, fpval2, fppval2 );
    assert ( fval1 * fval2 <= 0 );
    if ( fppval1 * fppval2 < 0 || fpval1 * fpval2 < 0 )
    {
      double xmiddle = (xmin + xmax)/2;
      calcCubicDerivatives ( xmiddle, a, b, c, d, fval, fpval, fppval );
      if ( fval1*fval < 0 )
      {
        xmax = xmiddle;
      } else {
        xmin = xmiddle;
      }
      continue;
    }
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
    assert (iterations < 100);
    return x;
  }

  assert (false);
  return (xmin + xmax)/2;
}

/*
 * This function computes the LU factorization of a mxn matrix, with
 * m typically less then n.  This is done with complete pivoting; the
 * exchanges in columns are recorded in the integer vector "exchange"
 */
void GaussianElimination( double *matrix[], int numrows, int numcols,
        int exchange[] )
{
  // start gaussian elimination
  for ( int k = 0; k < numrows; ++k )
  {
    // ricerca elemento di modulo massimo
    double maxval = -1.0;
    int imax = -1;
    int jmax = -1;
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
    for( int j = k; j < numcols; ++j )
    {
      double t = matrix[k][j];
      matrix[k][j] = matrix[imax][j];
      matrix[imax][j] = t;
    }
    // column exchange
    for( int i = 0; i < numrows; ++i )
    {
      double t = matrix[i][k];
      matrix[i][k] = matrix[i][jmax];
      matrix[i][jmax] = t;
    }
    // remember this column exchange at step k
    exchange[k] = jmax;

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
