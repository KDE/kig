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

#include <config.h>

#include "conic-common.h"

#include "common.h"
#include "kigtransform.h"

#include <cmath>
#include <algorithm>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

ConicCartesianData::ConicCartesianData(
  const ConicPolarData& polardata
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
}

ConicPolarData::ConicPolarData( const ConicCartesianData& cartdata )
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
}

const ConicCartesianData calcConicThroughPoints (
  const std::vector<Coordinate>& points,
  const LinearConstraints c1,
  const LinearConstraints c2,
  const LinearConstraints c3,
  const LinearConstraints c4,
  const LinearConstraints c5 )
{
  assert( 0 < points.size() && points.size() <= 5 );
  // points is a vector of up to 5 points through which the conic is
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
  double row0[6];
  double row1[6];
  double row2[6];
  double row3[6];
  double row4[6];
  double *matrix[5] = {row0, row1, row2, row3, row4};
  double solution[6];
  int scambio[6];
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

  if ( ! GaussianElimination( matrix, numpoints, 6, scambio ) )
    return ConicCartesianData::invalidData();
  // fine della fase di eliminazione
  BackwardSubstitution( matrix, numpoints, 6, scambio, solution );

  // now solution should contain the correct coefficients..
  return ConicCartesianData( solution );
}

const ConicPolarData calcConicBFFP(
  const std::vector<Coordinate>& args,
  int type )
{
  assert( args.size() >= 2 && args.size() <= 3 );
  assert( type == 1 || type == -1 );

  ConicPolarData ret;

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
  const ConicCartesianData& data,
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
  const ConicCartesianData& data,
  const LineData& polar )
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
    return Coordinate::invalidCoord();
  }

  x /= z;
  y /= z;
  return Coordinate (x, y);
}

const Coordinate calcConicLineIntersect( const ConicCartesianData& c,
                                         const LineData& l,
					 double knownparam,
                                         int which )
{
  assert( which == 1 || which == -1 || which == 0 );

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

  double t;
  if ( which == 0 )  /* i.e. we have a known intersection */
  {
    t = - bbb/aaa - knownparam;
    return l.a + t*(l.b - l.a);
  }

  double discrim = bbb*bbb - 4*aaa*ccc;
  if (discrim < 0.0)
  {
    return Coordinate::invalidCoord();
  }
  else
  {
    if ( which*bbb > 0 )
    {
      t = bbb + which*sqrt(discrim);
      t = - 2*ccc/t;
    } else {
      t = -bbb + which*sqrt(discrim);
      t /= 2*aaa;
    }

    return l.a + t*(l.b - l.a);
  }
}

ConicPolarData::ConicPolarData(
  const Coordinate& f, double d,
  double ec, double es )
  : focus1( f ), pdimen( d ), ecostheta0( ec ), esintheta0( es )
{
}

ConicPolarData::ConicPolarData()
  : focus1(), pdimen( 0 ), ecostheta0( 0 ), esintheta0( 0 )
{
}

const ConicPolarData calcConicBDFP(
  const LineData& directrix,
  const Coordinate& cfocus,
  const Coordinate& cpoint )
{
  ConicPolarData ret;

  Coordinate ba = directrix.dir();
  double bal = ba.length();
  ret.ecostheta0 = -ba.y / bal;
  ret.esintheta0 = ba.x / bal;

  Coordinate pa = cpoint - directrix.a;

  double distpf = (cpoint - cfocus).length();
  double distpd = ( pa.y*ba.x - pa.x*ba.y)/bal;

  double eccentricity = distpf/distpd;
  ret.ecostheta0 *= eccentricity;
  ret.esintheta0 *= eccentricity;

  Coordinate fa = cfocus - directrix.a;
  ret.pdimen = ( fa.y*ba.x - fa.x*ba.y )/bal;
  ret.pdimen *= eccentricity;
  ret.focus1 = cfocus;

  return ret;
}

ConicCartesianData::ConicCartesianData( const double incoeffs[6] )
{
  std::copy( incoeffs, incoeffs + 6, coeffs );
}

const LineData calcConicAsymptote(
  const ConicCartesianData data,
  int which, bool &valid )
{
  assert( which == -1 || which == 1 );

  LineData ret;
  double a=data.coeffs[0];
  double b=data.coeffs[1];
  double c=data.coeffs[2];
  double d=data.coeffs[3];
  double e=data.coeffs[4];

  double normabc = a*a + b*b + c*c;
  double delta = c*c - 4*a*b;
  if (fabs(delta) < 1e-6*normabc) { valid = false; return ret; }

  double yc = (2*a*e - c*d)/delta;
  double xc = (2*b*d - c*e)/delta;
  // let c be nonnegative; we no longer need d, e, f.
  if (c < 0)
  {
    c *= -1;
    a *= -1;
    b *= -1;
  }

  if ( delta < 0 )
  {
    valid = false;
    return ret;
  }

  double sqrtdelta = sqrt(delta);
  Coordinate displacement;
  if (which > 0)
    displacement = Coordinate(-2*b, c + sqrtdelta);
  else
    displacement = Coordinate(c + sqrtdelta, -2*a);
  ret.a = Coordinate(xc, yc);
  ret.b = ret.a + displacement;
  return ret;
}

const ConicCartesianData calcConicByAsymptotes(
  const LineData& line1,
  const LineData& line2,
  const Coordinate& p )
{
  Coordinate p1 = line1.a;
  Coordinate p2 = line1.b;
  double x = p.x;
  double y = p.y;

  double c1 = p1.x*p2.y - p2.x*p1.y;
  double b1 = p2.x - p1.x;
  double a1 = p1.y - p2.y;

  p1 = line2.a;
  p2 = line2.b;

  double c2 = p1.x*p2.y - p2.x*p1.y;
  double b2 = p2.x - p1.x;
  double a2 = p1.y - p2.y;

  double a = a1*a2;
  double b = b1*b2;
  double c = a1*b2 + a2*b1;
  double d = a1*c2 + a2*c1;
  double e = b1*c2 + c1*b2;

  double f = a*x*x + b*y*y + c*x*y + d*x + e*y;
  f = -f;

  return ConicCartesianData( a, b, c, d, e, f );
}

const LineData calcConicRadical( const ConicCartesianData& cequation1,
                                 const ConicCartesianData& cequation2,
                                 int which, int zeroindex, bool& valid )
{
  assert( which == 1 || which == -1 );
  assert( 0 < zeroindex && zeroindex < 4 );
  LineData ret;
  valid = true;

  double a = cequation1.coeffs[0];
  double b = cequation1.coeffs[1];
  double c = cequation1.coeffs[2];
  double d = cequation1.coeffs[3];
  double e = cequation1.coeffs[4];
  double f = cequation1.coeffs[5];

  double a2 = cequation2.coeffs[0];
  double b2 = cequation2.coeffs[1];
  double c2 = cequation2.coeffs[2];
  double d2 = cequation2.coeffs[3];
  double e2 = cequation2.coeffs[4];
  double f2 = cequation2.coeffs[5];

// background: the family of conics c + lambda*c2 has members that
// degenerate into a union of two lines. The values of lambda giving
// such degenerate conics is the solution of a third degree equation.
// The coefficients of such equation are given by:
// (Thanks to Dominique Devriese for the suggestion of this approach)
// domi: (And thanks to Maurizio for implementing it :)

  double df = 4*a*b*f - a*e*e - b*d*d - c*c*f + c*d*e;
  double cf = 4*a2*b*f + 4*a*b2*f + 4*a*b*f2
     - 2*a*e*e2 - 2*b*d*d2 - 2*f*c*c2
     - a2*e*e - b2*d*d - f2*c*c
     + c2*d*e + c*d2*e + c*d*e2;
  double bf = 4*a*b2*f2 + 4*a2*b*f2 + 4*a2*b2*f
     - 2*a2*e2*e - 2*b2*d2*d - 2*f2*c2*c
     - a*e2*e2 - b*d2*d2 - f*c2*c2
     + c*d2*e2 + c2*d*e2 + c2*d2*e;
  double af = 4*a2*b2*f2 - a2*e2*e2 - b2*d2*d2 - c2*c2*f2 + c2*d2*e2;

// assume both conics are nondegenerate, renormalize so that af = 1

  df /= af;
  cf /= af;
  bf /= af;
  af = 1.0;   // not needed, just for consistency

// computing the coefficients of the Sturm sequence

  double p1a = 2*bf*bf - 6*cf;
  double p1b = bf*cf - 9*df;
  double p0a = cf*p1a*p1a + p1b*(3*p1b - 2*bf*p1a);
  double fval, fpval, lambda;

  if (p0a < 0 && p1a < 0)
  {
    // -+--   ---+
    valid = false;
    return ret;
  }

  lambda = -bf/3.0;    //inflection point
  double displace = 1.0;
  if (p1a > 0)         // with two stationary points
  {
    displace += sqrt(p1a);  // should be enough.  The important
                            // thing is that it is larger than the
                            // semidistance between the stationary points
  }
  // compute the value at the inflection point using Horner scheme
  fval = bf + lambda;              // b + x
  fval = cf + lambda*fval;         // c + xb + xx
  fval = df + lambda*fval;         // d + xc + xxb + xxx

  if (fabs(p0a) < 1e-7)
  {   // this is the case if we intersect two vertical parabulas!
    p0a = 1e-7;  // fall back to the one zero case
  }
  if (p0a < 0)
  {
    // we have three roots..
    // we select the one we want ( defined by mzeroindex.. )
    lambda += ( 2 - zeroindex )* displace;
  }
  else
  {
    // we have just one root
    if( zeroindex > 1 )  // cannot find second and third root
    {
      valid = false;
      return ret;
    }

    if (fval > 0)      // zero on the left
    {
      lambda -= displace;
    } else {           // zero on the right
      lambda += displace;
    }

    // p0a = 0 means we have a root with multiplicity two
  }

//
// find a root of af*lambda^3 + bf*lambda^2 + cf*lambda + df = 0
// (use a Newton method starting from lambda = 0.  Hope...)
//

  double delta;

  int iterations = 0;
  const int maxiterations = 30;
  while (iterations++ < maxiterations)   // using Newton, iterations should be very few
  {
    // compute value of function and polinomial
    fval = fpval = af;
    fval = bf + lambda*fval;         // b + xa
    fpval = fval + lambda*fpval;     // b + 2xa
    fval = cf + lambda*fval;         // c + xb + xxa
    fpval = fval + lambda*fpval;     // c + 2xb + 3xxa
    fval = df + lambda*fval;         // d + xc + xxb + xxxa

    delta = fval/fpval;
    lambda -= delta;
    if (fabs(delta) < 1e-6) break;
  }
  if (iterations >= maxiterations) { valid = false; return ret; }

  // now we have the degenerate conic: a, b, c, d, e, f

  a += lambda*a2;
  b += lambda*b2;
  c += lambda*c2;
  d += lambda*d2;
  e += lambda*e2;
  f += lambda*f2;

  // domi:
  // this is the determinant of the matrix of the new conic.  It
  // should be zero, for the new conic to be degenerate.
  df = 4*a*b*f - a*e*e - b*d*d - c*c*f + c*d*e;

  //lets work in homogeneous coordinates...

  double dis1 = e*e - 4*b*f;
  double maxval = fabs(dis1);
  int maxind = 1;
  double dis2 = d*d - 4*a*f;
  if (fabs(dis2) > maxval)
  {
    maxval = fabs(dis2);
    maxind = 2;
  }
  double dis3 = c*c - 4*a*b;
  if (fabs(dis3) > maxval)
  {
    maxval = fabs(dis3);
    maxind = 3;
  }
  // one of these must be nonzero (otherwise the matrix is ...)
  // exchange elements so that the largest is the determinant of the
  // first 2x2 minor
  double temp;
  switch (maxind)
  {
    case 1:  // exchange 1 <-> 3
    temp = a; a = f; f = temp;
    temp = c; c = e; e = temp;
    temp = dis1; dis1 = dis3; dis3 = temp;
    break;

    case 2:  // exchange 2 <-> 3
    temp = b; b = f; f = temp;
    temp = c; c = d; d = temp;
    temp = dis2; dis2 = dis3; dis3 = temp;
    break;
  }

  // domi:
  // this is the negative of the determinant of the top left of the
  // matrix.  If it is 0, then the conic is a parabola, if it is < 0,
  // then the conic is an ellipse, if positive, the conic is a
  // hyperbola.  In this case, it should be positive, since we have a
  // degenerate conic, which is a degenerate case of a hyperbola..
  // note that it is negative if there is no valid conic to be
  // found ( e.g. not enough intersections.. )
  //  double discrim = c*c - 4*a*b;

  if (dis3 < 0)
  {
    // domi:
    // i would put an assertion here, but well, i guess it doesn't
    // really matter, and this prevents crashes if the math i still
    // recall from high school happens to be wrong :)
    valid = false;
    return ret;
  };

  double r[3];   // direction of the null space
  r[0] = 2*b*d - c*e;
  r[1] = 2*a*e - c*d;
  r[2] = dis3;

  // now remember the switch:
  switch (maxind)
  {
    case 1:  // exchange 1 <-> 3
    temp = a; a = f; f = temp;
    temp = c; c = e; e = temp;
    temp = dis1; dis1 = dis3; dis3 = temp;
    temp = r[0]; r[0] = r[2]; r[2] = temp;
    break;

    case 2:  // exchange 2 <-> 3
    temp = b; b = f; f = temp;
    temp = c; c = d; d = temp;
    temp = dis2; dis2 = dis3; dis3 = temp;
    temp = r[1]; r[1] = r[2]; r[2] = temp;
    break;
  }

  // Computing a Householder reflection transformation that
  // maps r onto [0, 0, k]

  double w[3];
  double rnormsq = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];
  double k = sqrt( rnormsq );
  if ( k*r[2] < 0) k = -k;
  double wnorm = sqrt( 2*rnormsq + 2*k*r[2] );
  w[0] = r[0]/wnorm;
  w[1] = r[1]/wnorm;
  w[2] = (r[2] + k)/wnorm;

  // matrix transformation using Householder matrix, the resulting
  // matrix is zero on third row and column
  // [q0,q1,q2]^t = A w
  // alpha = w^t A w
  double q0 = a*w[0] + c*w[1]/2 + d*w[2]/2;
  double q1 = b*w[1] + c*w[0]/2 + e*w[2]/2;
  double alpha = a*w[0]*w[0] + b*w[1]*w[1] + c*w[0]*w[1] +
                 d*w[0]*w[2] + e*w[1]*w[2] + f*w[2]*w[2];
  double a00 = a - 4*w[0]*q0 + 4*w[0]*w[0]*alpha;
  double a11 = b - 4*w[1]*q1 + 4*w[1]*w[1]*alpha;
  double a01 = c/2 - 2*w[1]*q0 - 2*w[0]*q1 + 4*w[0]*w[1]*alpha;

  double dis = a01*a01 - a00*a11;
  assert ( dis >= 0 );
  double sqrtdis = sqrt( dis );
  double px, py;
  if ( which*a01 > 0 )
  {
    px = a01 + which*sqrtdis;
    py = a11;
  } else {
    px = a00;
    py = a01 - which*sqrtdis;
  }
  double p[3];   // vector orthogonal to one of the two planes
  double pscalw = w[0]*px + w[1]*py;
  p[0] = px - 2*pscalw*w[0];
  p[1] = py - 2*pscalw*w[1];
  p[2] =    - 2*pscalw*w[2];

  // "r" is the solution of the equation A*(x,y,z) = (0,0,0) where
  // A is the matrix of the degenerate conic.  This is what we
  // called in the conic theory we saw in high school a "double
  // point".  It has the unique property that any line going through
  // it is a "polar line" of the conic at hand.  It only exists for
  // degenerate conics.  It has another unique property that if you
  // take any other point on the conic, then the line between it and
  // the double point is part of the conic.
  // this is what we use here: we find the double point ( ret.a
  // ), and then find another points on the conic.

  ret.a = -p[2]/(p[0]*p[0] + p[1]*p[1]) * Coordinate (p[0],p[1]);
  ret.b = ret.a + Coordinate (-p[1],p[0]);
  valid = true;

  return ret;
}

const ConicCartesianData calcConicTransformation (
  const ConicCartesianData& data, const Transformation& t, bool& valid )
{
  double a[3][3];
  double b[3][3];

  a[1][1] = data.coeffs[0];
  a[2][2] = data.coeffs[1];
  a[1][2] = a[2][1] = data.coeffs[2]/2;
  a[0][1] = a[1][0] = data.coeffs[3]/2;
  a[0][2] = a[2][0] = data.coeffs[4]/2;
  a[0][0] = data.coeffs[5];

  Transformation ti = t.inverse( valid );
  if ( ! valid ) return ConicCartesianData();

  double supnorm = 0.0;
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      b[i][j] = 0.;
      for (int ii = 0; ii < 3; ii++)
      {
        for (int jj = 0; jj < 3; jj++)
        {
	  b[i][j] += a[ii][jj]*ti.data( ii, i )*ti.data( jj, j );
	}
      }
      if ( std::fabs( b[i][j] ) > supnorm ) supnorm = std::fabs( b[i][j] );
    }
  }

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      b[i][j] /= supnorm;
    }
  }

  return ConicCartesianData ( b[1][1], b[2][2], b[1][2] + b[2][1],
                              b[0][1] + b[1][0], b[0][2] + b[2][0], b[0][0] );
}

ConicCartesianData::ConicCartesianData()
{
}

bool operator==( const ConicPolarData& lhs, const ConicPolarData& rhs )
{
  return lhs.focus1 == rhs.focus1 &&
         lhs.pdimen == rhs.pdimen &&
     lhs.ecostheta0 == rhs.ecostheta0 &&
     lhs.esintheta0 == rhs.esintheta0;
}

ConicCartesianData ConicCartesianData::invalidData()
{
  ConicCartesianData ret;
  ret.coeffs[0] = double_inf;
  return ret;
}

bool ConicCartesianData::valid() const
{
  return finite( coeffs[0] );
}

