// cubic_imp.cc
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "cubic_imp.h"

#include "bogus_imp.h"

#include "../misc/kigpainter.h"
#include "../misc/screeninfo.h"
#include "../misc/kignumerics.h"
#include "../misc/common.h"
#include "../kig/kig_view.h"

#include <math.h>
#include <klocale.h>

CubicImp::CubicImp( const CubicCartesianData& data )
  : CurveImp(), mdata( data )
{
}

CubicImp::~CubicImp()
{
}

ObjectImp* CubicImp::transform( const Transformation& t ) const
{
  bool valid = true;
  CubicCartesianData d = calcCubicTransformation( data(), t, valid );
  if ( valid ) return new CubicImp( d );
  else return new InvalidImp;
}

void CubicImp::draw( KigPainter& p ) const
{
//  p.drawCubic( mdata );
  p.drawCurve( this );
}

bool CubicImp::contains( const Coordinate& o, int width, const KigWidget& w ) const
{
  double a000 = mdata.coeffs[0];
  double a001 = mdata.coeffs[1];
  double a002 = mdata.coeffs[2];
  double a011 = mdata.coeffs[3];
  double a012 = mdata.coeffs[4];
  double a022 = mdata.coeffs[5];
  double a111 = mdata.coeffs[6];
  double a112 = mdata.coeffs[7];
  double a122 = mdata.coeffs[8];
  double a222 = mdata.coeffs[9];

  double x = o.x;
  double y = o.y;

  double f = a000 + a001*x + a002*y + a011*x*x + a012*x*y + a022*y*y +
             a111*x*x*x + a112*x*x*y + a122*x*y*y + a222*y*y*y;
  double fx = a001 + 2*a011*x + a012*y + 3*a111*x*x + 2*a112*x*y + a122*y*y;
  double fy = a002 + a012*x + 2*a022*y + a112*x*x + 2*a122*x*y + 3*a222*y*y;

  double dist = fabs(f)/(fabs(fx) + fabs(fy));

  return dist <= w.screenInfo().normalMiss( width );
  return false;
}

bool CubicImp::inRect( const Rect&, int, const KigWidget& ) const
{
  // TODO ?
  return false;
}

bool CubicImp::inherits( int type ) const
{
  return type == ID_CubicImp ? true : Parent::inherits( type );
}

CubicImp* CubicImp::copy() const
{
  return new CubicImp( mdata );
}

double CubicImp::getParam( const Coordinate& p, const KigDocument& ) const
{
  double x = p.x;
  double y = p.y;
  double t;

  double a000 = mdata.coeffs[0];
  double a001 = mdata.coeffs[1];
  double a002 = mdata.coeffs[2];
  double a011 = mdata.coeffs[3];
  double a012 = mdata.coeffs[4];
  double a022 = mdata.coeffs[5];
  double a111 = mdata.coeffs[6];
  double a112 = mdata.coeffs[7];
  double a122 = mdata.coeffs[8];
  double a222 = mdata.coeffs[9];

  /*
   * first project p onto the cubic. This is done by computing the
   * line through p in the direction of the gradient
   */

  double f = a000 + a001*x + a002*y + a011*x*x + a012*x*y + a022*y*y +
             a111*x*x*x + a112*x*x*y + a122*x*y*y + a222*y*y*y;
  if ( f != 0 )
  {
    double fx = a001 + 2*a011*x + a012*y + 3*a111*x*x + 2*a112*x*y + a122*y*y;
    double fy = a002 + 2*a022*y + a012*x + 3*a222*y*y + 2*a122*x*y + a112*x*x;
    Coordinate v = Coordinate (fx, fy);
    if ( f < 0 ) v = -v;   // the line points away from the intersection
    double a, b, c, d;
    calcCubicLineRestriction ( mdata, p, v, a, b, c, d );
    if ( a < 0 )
    {
       a *= -1;
       b *= -1;
       c *= -1;
       d *= -1;
    }

    // computing the coefficients of the Sturm sequence
    double p1a = 2*b*b - 6*a*c;
    double p1b = b*c - 9*a*d;
    double p0a = c*p1a*p1a + p1b*(3*a*p1b - 2*b*p1a);
    // compute the number of roots for negative lambda
    int variations = calcCubicVariations ( 0, a, b, c, d, p1a, p1b, p0a );
    bool valid;
    int numroots;
    double lambda = calcCubicRoot ( -1e10, 1e10, a, b, c, d, variations, valid,
                               numroots );
    if ( valid )
    {
      Coordinate pnew = p + lambda*v;
      x = pnew.x;
      y = pnew.y;
    }
  }

  if (x > 0) t = x/(1+x);
    else t = x/(1-x);
  t = 0.5*(t + 1);
  t /= 3;

  bool valid1, valid2, valid3;
  Coordinate p1 = internalGetPoint ( t, valid1 );
  Coordinate p2 = internalGetPoint ( t + 1.0/3.0, valid2 );
  Coordinate p3 = internalGetPoint ( t + 2.0/3.0, valid3 );

  double mint = t;
  double mindist = valid1 ? fabs ( y - p1.y ) : double_inf;
  if ( valid2 && fabs ( y - p2.y ) < mindist )
  {
    mint = t + 1.0/3.0;
    mindist = fabs ( y - p2.y );
  }
  if ( valid3 && fabs ( y - p3.y ) < mindist )
  {
    mint = t + 2.0/3.0;
  }

  return mint;
}

const Coordinate CubicImp::getPoint( double p, bool& valid, const KigDocument& ) const
{
  return internalGetPoint( p, valid );
}

const Coordinate CubicImp::internalGetPoint( double p, bool& valid ) const
{
  /*
   * this isn't really elegant...
   * the magnitude of p tells which one of the maximum 3 intersections
   * of the vertical line with the cubic to take.
   */

  p *= 3;
  int root = (int) p;
  assert ( root >= 0 );
  assert ( root <= 3 );
  if ( root == 3 ) root = 2;

  p -= root;

  assert ( 0 <= p && p <= 1 );
  if ( p <= 0. ) p = 1e-6;
  if ( p >= 1. ) p = 1 - 1e-6;
  root++;
  p = 2*p - 1;
  double x;
  if (p > 0) x = p/(1 - p);
  else x = p/(1 + p);

  // calc the third degree polynomial:
  // compute the third degree polinomial:
//  double a000 = mdata.coeffs[0];
//  double a001 = mdata.coeffs[1];
//  double a002 = mdata.coeffs[2];
//  double a011 = mdata.coeffs[3];
//  double a012 = mdata.coeffs[4];
//  double a022 = mdata.coeffs[5];
//  double a111 = mdata.coeffs[6];
//  double a112 = mdata.coeffs[7];
//  double a122 = mdata.coeffs[8];
//  double a222 = mdata.coeffs[9];
//
//  // first the y^3 coefficient, it coming only from a222:
//  double a = a222;
//  // next the y^2 coefficient (from a122 and a022):
//  double b = a122*x + a022;
//  // next the y coefficient (from a112, a012 and a002):
//  double c = a112*x*x + a012*x + a002;
//  // finally the constant coefficient (from a111, a011, a001 and a000):
//  double d = a111*x*x*x + a011*x*x + a001*x + a000;

// commented out, since the bound is already computed when passing a huge
// interval; the normalization is not needed also

  // renormalize: positive a
//  if ( a < 0 )
//  {
//    a *= -1;
//    b *= -1;
//    c *= -1;
//    d *= -1;
//  }
//
//  const double small = 1e-7;
//  int degree = 3;
//  if ( fabs(a) < small*fabs(b) ||
//       fabs(a) < small*fabs(c) ||
//       fabs(a) < small*fabs(d) )
//  {
//    degree = 2;
//    if ( fabs(b) < small*fabs(c) ||
//         fabs(b) < small*fabs(d) )
//    {
//      degree = 1;
//    }
//  }

// and a bound for all the real roots:

//  double bound;
//  switch (degree)
//  {
//    case 3:
//    bound = fabs(d/a);
//    if ( fabs(c/a) + 1 > bound ) bound = fabs(c/a) + 1;
//    if ( fabs(b/a) + 1 > bound ) bound = fabs(b/a) + 1;
//    break;
//
//    case 2:
//    bound = fabs(d/b);
//    if ( fabs(c/b) + 1 > bound ) bound = fabs(c/b) + 1;
//    break;
//
//    case 1:
//    default:
//    bound = fabs(d/c) + 1;
//    break;
//  }

  int numroots;
  double y = calcCubicYvalue ( x, -HUGE_VAL, HUGE_VAL, root, mdata, valid,
                               numroots );
  return Coordinate(x,y);
//  if ( valid ) return Coordinate(x,y);
//  root--; if ( root <= 0) root += 3;
//  y = calcCubicYvalue ( x, -bound, bound, root, mdata, valid,
//                        numroots );
//  if ( valid ) return Coordinate(x,y);
//  root--; if ( root <= 0) root += 3;
//  y = calcCubicYvalue ( x, -bound, bound, root, mdata, valid,
//                        numroots );
//  assert ( valid );
//  return Coordinate(x,y);
}

const uint CubicImp::numberOfProperties() const
{
  // TODO ?
  return Parent::numberOfProperties();
}

const QCStringList CubicImp::propertiesInternalNames() const
{
  return Parent::propertiesInternalNames();
}

const QCStringList CubicImp::properties() const
{
  // TODO ?
  return Parent::properties();
}

int CubicImp::impRequirementForProperty( uint which ) const
{
  return Parent::impRequirementForProperty( which );
}

const char* CubicImp::iconForProperty( uint which ) const
{
  return Parent::iconForProperty( which );
}

ObjectImp* CubicImp::property( uint which, const KigDocument& w ) const
{
  // TODO ?
  return Parent::property( which, w );
}

const CubicCartesianData CubicImp::data() const
{
  return mdata;
}

const char* CubicImp::baseName() const
{
  return I18N_NOOP( "cubic" );
}

int CubicImp::id() const
{
  return ID_CubicImp;
}

void CubicImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool CubicImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( ID_CubicImp ) &&
    static_cast<const CubicImp&>( rhs ).data() == data();
}

