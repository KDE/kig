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

#include "conic-common.h"

#include <cmath>

ConicCartesianEquationData::ConicCartesianEquationData(
  const ConicPolarEquationData& polardata
  )
{
  double ec = polardata.ecostheta0;
  double es = polardata.esintheta0;
  double p = polardata.pdimen;
  double fx = polardata.focus1.x;
  double fy = polardata.focus1.y;

  double a = 1 - ec*ec;
  double b = 1 - es*es;
  double c = - 2*ec*es;
  double d = - 2*p*ec;
  double e = - 2*p*es;
  double f = - p*p;

  f += a*fx*fx + b*fy*fy + c*fx*fy - d*fx - e*fy;
  d -= 2*a*fx + c*fy;
  e -= 2*b*fy + c*fx;

  coeffs[0] = a;
  coeffs[1] = b;
  coeffs[2] = c;
  coeffs[3] = d;
  coeffs[4] = e;
  coeffs[5] = f;
};

ConicPolarEquationData::ConicPolarEquationData( const ConicCartesianEquationData& cartdata )
{
  double a = cartdata.coeffs[0];
  double b = cartdata.coeffs[1];
  double c = cartdata.coeffs[2];
  double d = cartdata.coeffs[3];
  double e = cartdata.coeffs[4];
  double f = cartdata.coeffs[5];

  // 1. Compute theta (tilt of conic)
  double theta = std::atan2(c, b - a)/2;

  // now I should possibly add pi/2...
  double costheta = std::cos(theta);
  double sintheta = std::sin(theta);
  // compute new coefficients (c should now be zero)
  double aa = a*costheta*costheta + b*sintheta*sintheta - c*sintheta*costheta;
  double bb = a*sintheta*sintheta + b*costheta*costheta + c*sintheta*costheta;
  if (aa*bb < 0)
  {   // we have a hyperbola we need to check the correct orientation
    double dd = d*costheta - e*sintheta;
    double ee = d*sintheta + e*costheta;
    double xc = - dd / ( 2*aa );
    double yc = - ee / ( 2*bb );
    double ff = f + aa*xc*xc + bb*yc*yc + dd*xc + ee*yc;
    if (ff*aa > 0)    // wrong orientation
    {
      if (theta > 0) theta -= M_PI/2;
      else theta += M_PI/2;
      costheta = cos(theta);
      sintheta = sin(theta);
      aa = a*costheta*costheta + b*sintheta*sintheta - c*sintheta*costheta;
      bb = a*sintheta*sintheta + b*costheta*costheta + c*sintheta*costheta;
    }
  }
  else
  {
    if ( std::fabs (bb) < std::fabs (aa) )
    {
      if (theta > 0) theta -= M_PI/2;
      else theta += M_PI/2;
      costheta = cos(theta);
      sintheta = sin(theta);
      aa = a*costheta*costheta + b*sintheta*sintheta - c*sintheta*costheta;
      bb = a*sintheta*sintheta + b*costheta*costheta + c*sintheta*costheta;
    }
  }

  double cc = 2*(a - b)*sintheta*costheta +
              c*(costheta*costheta - sintheta*sintheta);
  //  cc should be zero!!!   cout << "cc = " << cc << "\n";
  double dd = d*costheta - e*sintheta;
  double ee = d*sintheta + e*costheta;

  a = aa;
  b = bb;
  c = cc;
  d = dd;
  e = ee;

  // now b cannot be zero (otherwise conic is degenerate)
  a /= b;
  c /= b;
  d /= b;
  e /= b;
  f /= b;
  b = 1.0;

  // 2. compute y coordinate of focuses

  double yf = - e/2;

  // new values:
  f += yf*yf + e*yf;
  e += 2*yf;   // this should be zero!

  // now: a > 0 -> ellipse
  //      a = 0 -> parabula
  //      a < 0 -> hyperbola

  double eccentricity = sqrt(1.0 - a);

  double sqrtdiscrim = sqrt(d*d - 4*a*f);
  if (d < 0.0) sqrtdiscrim = -sqrtdiscrim;
  double xf = (4*a*f - 4*f - d*d)/(d + eccentricity*sqrtdiscrim) / 2;

  // 3. the focus needs to be rotated back into position
  focus1.x = xf*costheta + yf*sintheta;
  focus1.y = -xf*sintheta + yf*costheta;

  // 4. final touch: the pdimen
  pdimen = -sqrtdiscrim/2;

  ecostheta0 = eccentricity*costheta;
  esintheta0 = -eccentricity*sintheta;
  if ( pdimen < 0)
  {
    pdimen = -pdimen;
    ecostheta0 = -ecostheta0;
    esintheta0 = -esintheta0;
  }
};

const ConicCartesianEquationData calcConicThroughPoints (
  const std::vector<Coordinate>& points,
  const LinearConstraints c1,
  const LinearConstraints c2,
  const LinearConstraints c3,
  const LinearConstraints c4,
  const LinearConstraints c5 )
{
  // points is a vector of at least 5 points through which the conic is
  // constrained.
  // this routine should compute the coefficients in the cartesian equation
  //    a x^2 + b y^2 + c xy + d x + e y + f = 0
  // they are defined up to a multiplicative factor.
  // since we don't know (in advance) which one of them is nonzero, we
  // simply keep all 6 parameters, obtaining a 5x6 linear system which
  // we solve using gaussian elimination with complete pivoting
  // If there are too few, then we choose some cool way to fill in the
  // empty parts in the matrix according to the LinearConstraints
  // given..

  // 5 rows, 6 columns..
  double matrix[5][6];
  double solution[6];
  int scambio[5];
  LinearConstraints constraints[] = {c1, c2, c3, c4, c5};

  int numpoints = points.size();
  int numconstraints = 5;

  // fill in the matrix elements
  for ( int i = 0; i < numpoints; ++i )
  {
    double xi = points[i].x;
    double yi = points[i].y;
    matrix[i][0] = xi*xi;
    matrix[i][1] = yi*yi;
    matrix[i][2] = xi*yi;
    matrix[i][3] = xi;
    matrix[i][4] = yi;
    matrix[i][5] = 1.0;
  }

  for ( int i = 0; i < numconstraints; i++ )
  {
    if (numpoints >= 5) break;    // don't add constraints if we have enough
    for (int j = 0; j < 6; ++j) matrix[numpoints][j] = 0.0;
    // force the symmetry axes to be
    // parallel to the coordinate system (zero tilt): c = 0
    if (constraints[i] == zerotilt) matrix[numpoints][2] = 1.0;
    // force a parabula (if zerotilt): b = 0
    if (constraints[i] == parabolaifzt) matrix[numpoints][1] = 1.0;
    // force a circle (if zerotilt): a = b
    if (constraints[i] == circleifzt) {
      matrix[numpoints][0] = 1.0;
      matrix[numpoints][1] = -1.0; }
    // force an equilateral hyperbola: a + b = 0
    if (constraints[i] == equilateral) {
      matrix[numpoints][0] = 1.0;
      matrix[numpoints][1] = 1.0; }
    // force symmetry about y-axis: d = 0
    if (constraints[i] == ysymmetry) matrix[numpoints][3] = 1.0;
    // force symmetry about x-axis: e = 0
    if (constraints[i] == xsymmetry) matrix[numpoints][4] = 1.0;

    if (constraints[i] != noconstraint) ++numpoints;
  }

  // start gaussian elimination
  for ( int k = 0; k < numpoints; ++k )
  {
    // ricerca elemento di modulo massimo
    double maxval = -1.0;
    int imax = -1;
    int jmax = -1;
    for( int i = k; i < numpoints; ++i )
    {
      for( int j = k; j < 6; ++j )
      {
        if (fabs(matrix[i][j]) > maxval)
        {
          maxval = fabs(matrix[i][j]);
          imax = i;
          jmax = j;
        }
      }
    }
    // scambio di riga
    for( int j = k; j < 6; ++j )
    {
      double t = matrix[k][j];
      matrix[k][j] = matrix[imax][j];
      matrix[imax][j] = t;
    }
    // scambio di colonna
    for( int i = 0; i < numpoints; ++i )
    {
      double t = matrix[i][k];
      matrix[i][k] = matrix[i][jmax];
      matrix[i][jmax] = t;
    }
    // ricorda lo scambio effettuato al passo k
    scambio[k] = jmax;

    // ciclo sulle righe
    for( int i = k+1; i < numpoints; ++i)
    {
      double mik = matrix[i][k]/matrix[k][k];
      matrix[i][k] = mik;    //ricorda il moltiplicatore... (not necessary)
      // ciclo sulle colonne
      for( int j = k+1; j < 6; ++j )
      {
        matrix[i][j] -= mik*matrix[k][j];
      }
    }
  }

  // fine della fase di eliminazione
  // il sistema e' sottodeterminato, fisso l'ultima incognita = 1
  for ( int j = numpoints; j < 6; ++j )
  {
    solution[j] = 1.0;          // other choices are possible here
  };

  for( int k = numpoints - 1; k >= 0; --k )
  {
    // sostituzioni all'indietro
    solution[k] = 0.0;
    for ( int j = k+1; j < 6; ++j)
    {
      solution[k] -= matrix[k][j]*solution[j];
    }
    solution[k] /= matrix[k][k];
  }

  // ultima fase: riordinamento incognite

  for( int k = numpoints - 1; k >= 0; --k )
  {
    int jmax = scambio[k];
    double t = solution[k];
    solution[k] = solution[jmax];
    solution[jmax] = t;
  }

  // now solution should contain the correct coefficients..
  return ConicCartesianEquationData( solution );
}

const ConicPolarEquationData calcConicBFFP(
  const std::vector<Coordinate>& args,
  int type )
{
  assert( args.size() > 1 && args.size() < 4 );
  assert( type == 1 || type == -1 );

  ConicPolarEquationData ret;

  Coordinate f1 = args[0];
  Coordinate f2 = args[1];
  Coordinate d;
  double eccentricity, d1, d2, dl;

  Coordinate f2f1 = f2 - f1;
  double f2f1l = f2f1.length();
  ret.ecostheta0 = f2f1.x / f2f1l;
  ret.esintheta0 = f2f1.y / f2f1l;

  if ( args.size() == 3 )
  {
    d = args[2];
    d1 = ( d - f1 ).length();
    d2 = ( d - f2 ).length();
    dl = fabs( d1 + type * d2 );
    eccentricity = f2f1l/dl;
  }
  else
  {
    if ( type > 0 ) eccentricity = 0.7; else eccentricity = 2.0;
    dl = f2f1l/eccentricity;
  }

  double rhomax = (dl + f2f1l) /2.0;

  ret.ecostheta0 *= eccentricity;
  ret.esintheta0 *= eccentricity;
  ret.pdimen = type*(1 - eccentricity)*rhomax;
  ret.focus1 = type == 1 ? f1 : f2;
  return ret;
}

const LineData calcConicPolarLine (
  const ConicCartesianEquationData& data,
  const Coordinate& cpole,
  bool& valid )
{
  double x = cpole.x;
  double y = cpole.y;
  double a = data.coeffs[0];
  double b = data.coeffs[1];
  double c = data.coeffs[2];
  double d = data.coeffs[3];
  double e = data.coeffs[4];
  double f = data.coeffs[5];

  double alpha = 2*a*x + c*y + d;
  double beta = c*x + 2*b*y + e;
  double gamma = d*x + e*y + 2*f;

  double normsq = alpha*alpha + beta*beta;

  if (normsq < 1e-10)          // line at infinity
  {
    valid = false;
    return LineData();
  }
  valid = true;

  Coordinate reta = -gamma/normsq * Coordinate (alpha, beta);
  Coordinate retb = reta + Coordinate (-beta, alpha);
  return LineData( reta, retb );
}

const Coordinate calcConicPolarPoint (
  const ConicCartesianEquationData& data,
  const LineData& polar,
  bool& valid )
{
  Coordinate p1 = polar.a;
  Coordinate p2 = polar.b;

  double alpha = p2.y - p1.y;
  double beta = p1.x - p2.x;
  double gamma = p1.y*p2.x - p1.x*p2.y;

  double a11 = data.coeffs[0];
  double a22 = data.coeffs[1];
  double a12 = data.coeffs[2]/2.0;
  double a13 = data.coeffs[3]/2.0;
  double a23 = data.coeffs[4]/2.0;
  double a33 = data.coeffs[5];

//  double detA = a11*a22*a33 - a11*a23*a23 - a22*a13*a13 - a33*a12*a12 +
//                2*a12*a23*a13;

  double a11inv = a22*a33 - a23*a23;
  double a22inv = a11*a33 - a13*a13;
  double a33inv = a11*a22 - a12*a12;
  double a12inv = a23*a13 - a12*a33;
  double a23inv = a12*a13 - a23*a11;
  double a13inv = a12*a23 - a13*a22;

  double x = a11inv*alpha + a12inv*beta + a13inv*gamma;
  double y = a12inv*alpha + a22inv*beta + a23inv*gamma;
  double z = a13inv*alpha + a23inv*beta + a33inv*gamma;

  if (fabs(z) < 1e-10)          // point at infinity
  {
    valid = false;
    return Coordinate (0,0);
  }
  valid = true;

  x /= z;
  y /= z;
  return Coordinate (x, y);
}

const Coordinate calcConicLineIntersect( const ConicCartesianEquationData c,
                                         const LineData& l,
                                         int which, bool& valid )
{
  assert( which == 1 || which == -1 );

  double aa = c.coeffs[0];
  double bb = c.coeffs[1];
  double cc = c.coeffs[2];
  double dd = c.coeffs[3];
  double ee = c.coeffs[4];
  double ff = c.coeffs[5];

  double x = l.a.x;
  double y = l.a.y;
  double dx = l.b.x - l.a.x;
  double dy = l.b.y - l.a.y;

  double aaa = aa*dx*dx + bb*dy*dy + cc*dx*dy;
  double bbb = 2*aa*x*dx + 2*bb*y*dy + cc*x*dy + cc*y*dx + dd*dx + ee*dy;
  double ccc = aa*x*x + bb*y*y + cc*x*y + dd*x + ee*y + ff;

  double discrim = bbb*bbb - 4*aaa*ccc;
  if (discrim < 0.0)
  {
    valid = false;
    return Coordinate();
  }
  else
  {
    valid = true;
    double t = -bbb + which*sqrt(discrim);
    t /= 2*aaa;

    return l.a + t*(l.b - l.a);
  }
}

ConicPolarEquationData::ConicPolarEquationData(
  const Coordinate& f, double d,
  double ec, double es )
  : focus1( f ), pdimen( d ), ecostheta0( ec ), esintheta0( es )
{
};

ConicPolarEquationData::ConicPolarEquationData()
  : focus1(), pdimen( 0 ), ecostheta0( 0 ), esintheta0( 0 )
{
};
