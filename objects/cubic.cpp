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

#include "cubic.h"
#include "conic.h"

#include "point.h"
//#include "abstractline.h"

#include "../misc/common.h"
#include "../misc/kigpainter.h"
//#include "../misc/kigtransform.h"
#include "../misc/coordinate_system.h"
#include "../misc/i18n.h"
#include "../kig/kig_view.h"
#include "../kig/kig_part.h"

#include <kdebug.h>
#include <qpen.h>
#include <math.h>

Cubic::Cubic()
{
};

Cubic::~Cubic()
{
};

bool Cubic::isa( int type ) const
{
  return type == CubicT ? true : Parent::isa( type );
}

bool Cubic::contains (const Coordinate& o, const ScreenInfo& si ) const
{
  double a000 = cequation.coeffs[0];
  double a001 = cequation.coeffs[1];
  double a002 = cequation.coeffs[2];
  double a011 = cequation.coeffs[3];
  double a012 = cequation.coeffs[4];
  double a022 = cequation.coeffs[5];
  double a111 = cequation.coeffs[6];
  double a112 = cequation.coeffs[7];
  double a122 = cequation.coeffs[8];
  double a222 = cequation.coeffs[9];

  double x = o.x;
  double y = o.y;

  double f = a000 + a001*x + a002*y + a011*x*x + a012*x*y + a022*y*y +
             a111*x*x*x + a112*x*x*y + a122*x*y*y + a222*y*y*y;
  double fx = a001 + 2*a011*x + a012*y + 3*a111*x*x + 2*a112*x*y + a122*y*y;
  double fy = a002 + a012*x + 2*a022*y + a112*x*x + 2*a122*x*y + 3*a222*y*y;

  double dist = fabs(f)/(fabs(fx) + fabs(fy));

  return dist <= si.normalMiss();
  return false;
};

void Cubic::draw (KigPainter& p, bool ss) const
{
  p.setPen(QPen ( selected && ss ? Qt::red : mColor, 1));
  p.setBrush( Qt::NoBrush );

  p.drawCubic ( cequation );
};

bool Cubic::inRect ( const Rect& ) const
{
  // not implemented yet, i'm thinking: take the diagonals of the
  // rect, their intersections with the circle, and check their
  // positions...
  return false;
};

Coordinate Cubic::getPoint (double p) const
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
  double a000 = cequation.coeffs[0];
  double a001 = cequation.coeffs[1];
  double a002 = cequation.coeffs[2];
  double a011 = cequation.coeffs[3];
  double a012 = cequation.coeffs[4];
  double a022 = cequation.coeffs[5];
  double a111 = cequation.coeffs[6];
  double a112 = cequation.coeffs[7];
  double a122 = cequation.coeffs[8];
  double a222 = cequation.coeffs[9];

  // first the y^3 coefficient, it coming only from a222:
  double a = a222;
  // next the y^2 coefficient (from a122 and a022):
  double b = a122*x + a022;
  // next the y coefficient (from a112, a012 and a002):
  double c = a112*x*x + a012*x + a002;
  // finally the constant coefficient (from a111, a011, a001 and a000):
  double d = a111*x*x*x + a011*x*x + a001*x + a000;

  // renormalize: positive a
  if ( a < 0 )
  {
    a *= -1;
    b *= -1;
    c *= -1;
    d *= -1;
  }

  const double small = 1e-7;
  int degree = 3;
  if ( fabs(a) < small*fabs(b) ||
       fabs(a) < small*fabs(c) ||
       fabs(a) < small*fabs(d) )
  {
    degree = 2;
    if ( fabs(b) < small*fabs(c) ||
         fabs(b) < small*fabs(d) )
    {
      degree = 1;
    }
  }

// and a bound for all the real roots:

  double bound;
  switch (degree)
  {
    case 3:
    bound = fabs(d/a);
    if ( fabs(c/a) + 1 > bound ) bound = fabs(c/a) + 1;
    if ( fabs(b/a) + 1 > bound ) bound = fabs(b/a) + 1;
    break;

    case 2:
    bound = fabs(d/b);
    if ( fabs(c/b) + 1 > bound ) bound = fabs(c/b) + 1;
    break;

    case 1:
    default:
    bound = fabs(d/c) + 1;
    break;
  }

  bool valid;
  int numroots;
  double y = calcCubicYvalue ( x, -bound, bound, root, cequation, valid,
                               numroots );
  if ( valid ) return Coordinate(x,y);
  root--; if ( root <= 0) root += 3;
  y = calcCubicYvalue ( x, -bound, bound, root, cequation, valid,
                               numroots );
  if ( valid ) return Coordinate(x,y);
  root--; if ( root <= 0) root += 3;
  y = calcCubicYvalue ( x, -bound, bound, root, cequation, valid,
                               numroots );
  assert ( valid );
  return Coordinate(x,y);
};

double Cubic::getParam (const Coordinate& p) const
{
  double x = p.x;
  double y = p.y;
  double t;

  double a000 = cequation.coeffs[0];
  double a001 = cequation.coeffs[1];
  double a002 = cequation.coeffs[2];
  double a011 = cequation.coeffs[3];
  double a012 = cequation.coeffs[4];
  double a022 = cequation.coeffs[5];
  double a111 = cequation.coeffs[6];
  double a112 = cequation.coeffs[7];
  double a122 = cequation.coeffs[8];
  double a222 = cequation.coeffs[9];

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
    calcCubicLineRestriction ( cequation, p, v, a, b, c, d );
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

  Coordinate p1 = getPoint ( t );
  Coordinate p2 = getPoint ( t + 1.0/3.0 );
  Coordinate p3 = getPoint ( t + 2.0/3.0 );

  double mint = t;
  double mindist = fabs ( y - p1.y );
  if ( fabs ( y - p2.y ) < mindist )
  {
    mint = t + 1.0/3.0;
    mindist = fabs ( y - p2.y );
  }
  if ( fabs ( y - p3.y ) < mindist )
  {
    mint = t + 2.0/3.0;
  }

  return mint;
};

const QCString Cubic::sBaseTypeName()
{
  return I18N_NOOP("cubic");
}

Cubic::Cubic( const Cubic& c )
  : Curve( c ), cequation( c.cequation )
{
}

const CubicCartesianEquationData Cubic::cartesianEquationData() const
{
  return cequation;
}

/*
 * cubic through 9 given points
 */

void CubicB9P::calc()
{
  std::vector<Coordinate> points;

  mvalid = true;
  for ( Point** ipt = pts; ipt < pts + 9; ++ipt )
    mvalid &= (*ipt)->valid();
  if ( mvalid )
  {
    std::transform( pts, pts + 9, std::back_inserter( points ),
                    std::mem_fun( &Point::getCoord ) );
    cequation = calcCubicThroughPoints( points );
  }
}

const char* CubicB9P::sActionName()
{
  return "objects_new_cubicb9p";
}

Objects CubicB9P::getParents() const
{
  Objects objs ( pts, pts+9 );
  return objs;
}

CubicB9P::CubicB9P(const CubicB9P& c)
  : Cubic( c )
{
  for ( int i = 0; i < 9; i++ )
  {
    pts[i]=c.pts[i];
    pts[i]->addChild(this);
  }
}

const QString CubicB9P::sDescriptiveName()
{
  return i18n("Cubic by nine points");
}

const QString CubicB9P::sDescription()
{
  return i18n( "A cubic constructed through nine points" );
}

Object::WantArgsResult CubicB9P::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 9 || size < 1 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toPoint() ) return NotGood;
  return size == 9 ? Complete : NotComplete;
}

QString CubicB9P::sUseText( const Objects&, const Object* )
{
  return i18n("Through point");
}

void CubicB9P::sDrawPrelim( KigPainter& p, const Objects& os )
{
  std::vector<Coordinate> points;

  uint size = os.size();
  assert( size > 0 && size <= 9 );
  if ( size < 2 ) return;  // don't drawprelim if too few points
  for ( uint i = 0; i < size; ++i )
  {
    assert( os[i]->toPoint() );
    points.push_back( os[i]->toPoint()->getCoord() );
  };

//  if ( size <= 5 )
//  {
//    p.setPen(QPen (Qt::green, 1));
//    p.drawConic(
//      ConicPolarEquationData(
//        calcConicThroughPoints( points, zerotilt, parabolaifzt, ysymmetry )
//        )
//      );
//    return;
//  }

  p.setPen(QPen (Qt::red, 1));
  p.drawCubic(
      calcCubicThroughPoints( points )
      );

  return;
}

CubicB9P::CubicB9P( const Objects& os )
  : Cubic()
{
  assert( os.size() == 9 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( (*i)->toPoint() );
    (*i)->addChild( this );
    pts[i-os.begin()] = static_cast<Point*>( *i );
  };
}

/*
 * cubic with cusp at the origin (with horizontal tangent) through 4 points
 */

void CubicCuspB4P::calc()
{
  std::vector<Coordinate> points;

  mvalid = true;
  for ( Point** ipt = pts; ipt < pts + 4; ++ipt )
    mvalid &= (*ipt)->valid();
  if ( mvalid )
  {
    std::transform( pts, pts + 4, std::back_inserter( points ),
                    std::mem_fun( &Point::getCoord ) );
    cequation = calcCubicCuspThroughPoints( points );
  }
}

const char* CubicCuspB4P::sActionName()
{
  return "objects_new_cubiccuspb4p";
}

Objects CubicCuspB4P::getParents() const
{
  Objects objs ( pts, pts+4 );
  return objs;
}

CubicCuspB4P::CubicCuspB4P(const CubicCuspB4P& c)
  : Cubic( c )
{
  for ( int i = 0; i < 4; i++ )
  {
    pts[i]=c.pts[i];
    pts[i]->addChild(this);
  }
}

const QString CubicCuspB4P::sDescriptiveName()
{
  return i18n("Cubic with cusp by four points");
}

const QString CubicCuspB4P::sDescription()
{
  return i18n( "A cubic with a horizontal cusp at the origin through four points" );
}

Object::WantArgsResult CubicCuspB4P::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 4 || size < 1 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toPoint() ) return NotGood;
  return size == 4 ? Complete : NotComplete;
}

QString CubicCuspB4P::sUseText( const Objects&, const Object* )
{
  return i18n("Through point");
}

void CubicCuspB4P::sDrawPrelim( KigPainter& p, const Objects& os )
{
  std::vector<Coordinate> points;

  uint size = os.size();
  assert( size > 0 && size <= 4 );
  if ( size < 2 ) return;  // don't drawprelim if too few points
  for ( uint i = 0; i < size; ++i )
  {
    assert( os[i]->toPoint() );
    points.push_back( os[i]->toPoint()->getCoord() );
  };

  p.setPen(QPen (Qt::red, 1));
  p.drawCubic(
      calcCubicCuspThroughPoints( points )
      );

  return;
}

CubicCuspB4P::CubicCuspB4P( const Objects& os )
  : Cubic()
{
  assert( os.size() == 4 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( (*i)->toPoint() );
    (*i)->addChild( this );
    pts[i-os.begin()] = static_cast<Point*>( *i );
  };
}

/*
 * cubic with node at the origin through 6 points
 */

void CubicNodeB6P::calc()
{
  std::vector<Coordinate> points;

  mvalid = true;
  for ( Point** ipt = pts; ipt < pts + 6; ++ipt )
    mvalid &= (*ipt)->valid();
  if ( mvalid )
  {
    std::transform( pts, pts + 6, std::back_inserter( points ),
                    std::mem_fun( &Point::getCoord ) );
    cequation = calcCubicNodeThroughPoints( points );
  }
}

const char* CubicNodeB6P::sActionName()
{
  return "objects_new_cubicnodeb6p";
}

Objects CubicNodeB6P::getParents() const
{
  Objects objs ( pts, pts+6 );
  return objs;
}

CubicNodeB6P::CubicNodeB6P(const CubicNodeB6P& c)
  : Cubic( c )
{
  for ( int i = 0; i < 6; i++ )
  {
    pts[i]=c.pts[i];
    pts[i]->addChild(this);
  }
}

const QString CubicNodeB6P::sDescriptiveName()
{
  return i18n("Cubic with node by six points");
}

const QString CubicNodeB6P::sDescription()
{
  return i18n( "A cubic with a nodal point at the origin through six points" );
}

Object::WantArgsResult CubicNodeB6P::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 6 || size < 1 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toPoint() ) return NotGood;
  return size == 6 ? Complete : NotComplete;
}

QString CubicNodeB6P::sUseText( const Objects&, const Object* )
{
  return i18n("Through point");
}

void CubicNodeB6P::sDrawPrelim( KigPainter& p, const Objects& os )
{
  std::vector<Coordinate> points;

  uint size = os.size();
  assert( size > 0 && size <= 6 );
  if ( size < 2 ) return;  // don't drawprelim if too few points
  for ( uint i = 0; i < size; ++i )
  {
    assert( os[i]->toPoint() );
    points.push_back( os[i]->toPoint()->getCoord() );
  };

  p.setPen(QPen (Qt::red, 1));
  p.drawCubic(
      calcCubicNodeThroughPoints( points )
      );

  return;
}

CubicNodeB6P::CubicNodeB6P( const Objects& os )
  : Cubic()
{
  assert( os.size() == 6 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( (*i)->toPoint() );
    (*i)->addChild( this );
    pts[i-os.begin()] = static_cast<Point*>( *i );
  };
}

/*
 * coefficients of the cartesian equation for cubics
 */

CubicCartesianEquationData::CubicCartesianEquationData()
{
  std::fill( coeffs, coeffs + 10, 0 );
};

CubicCartesianEquationData::CubicCartesianEquationData(
            const double incoeffs[10] )
{
  std::copy( incoeffs, incoeffs + 10, coeffs );
};

const CubicCartesianEquationData calcCubicThroughPoints (
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

  GaussianElimination( matrix, numpoints, 10, scambio );
  // fine della fase di eliminazione
  BackwardSubstitution( matrix, numpoints, 10, scambio, solution );

  // now solution should contain the correct coefficients..
  return CubicCartesianEquationData( solution );
}

const CubicCartesianEquationData calcCubicCuspThroughPoints (
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

  GaussianElimination( matrix, numpoints, 10, scambio );
  // fine della fase di eliminazione
  BackwardSubstitution( matrix, numpoints, 10, scambio, solution );

  // now solution should contain the correct coefficients..
  return CubicCartesianEquationData( solution );
}

const CubicCartesianEquationData calcCubicNodeThroughPoints (
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

  GaussianElimination( matrix, numpoints, 10, scambio );
  // fine della fase di eliminazione
  BackwardSubstitution( matrix, numpoints, 10, scambio, solution );

  // now solution should contain the correct coefficients..
  return CubicCartesianEquationData( solution );
}

/*
 * computation of the y value corresponding to some x value
 */

double calcCubicYvalue ( double x, double ymin, double ymax, int root,
                         CubicCartesianEquationData data, bool& valid,
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

/*
 * intersection of a cubic and a line.  There are in general 3
 * intersection points, so we need a parameter (mroot) to
 * distinguish among them
 */

CubicLineIntersectionPoint::CubicLineIntersectionPoint( const Objects& os )
  : Point(), mcubic( 0 ), mline( 0 ), mroot( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! mcubic ) mcubic = (*i)->toCubic();
    if ( ! mline ) mline = (*i)->toAbstractLine();
  };
  assert( mcubic && mline );
  mcubic->addChild( this );
  mline->addChild( this );
}

CubicLineIntersectionPoint::CubicLineIntersectionPoint( const CubicLineIntersectionPoint& p )
  : Point( p ), mcubic( p.mcubic ), mline( p.mline ), mroot( p.mroot )
{
  assert( mcubic && mline );
  mcubic->addChild( this );
  mline->addChild( this );
}

CubicLineIntersectionPoint::~CubicLineIntersectionPoint()
{
}

const QCString CubicLineIntersectionPoint::vFullTypeName() const
{
  return sFullTypeName();
}

const QCString CubicLineIntersectionPoint::sFullTypeName()
{
  return "CubicLineIntersectionPoint";
}

const QString CubicLineIntersectionPoint::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString CubicLineIntersectionPoint::sDescriptiveName()
{
  return i18n("The intersections of a line and a cubic");
}

const QString CubicLineIntersectionPoint::vDescription() const
{
  return sDescription();
}

const QCString CubicLineIntersectionPoint::vIconFileName() const
{
  return sIconFileName();
}

const QCString CubicLineIntersectionPoint::sIconFileName()
{
  return "cubiclineintersection";
}

const int CubicLineIntersectionPoint::vShortCut() const
{
  return sShortCut();
}

const int CubicLineIntersectionPoint::sShortCut()
{
  return 0;
}

const char* CubicLineIntersectionPoint::sActionName()
{
  return "objects_new_cubiclineintersect";
}

void CubicLineIntersectionPoint::sDrawPrelim( KigPainter& p, const Objects& os )
{
  assert( os.size() <= 2 );
  if ( os.size() != 2 ) return;
  Cubic* c = 0;
  AbstractLine* l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! c ) c = (*i)->toCubic();
    if ( ! l ) l = (*i)->toAbstractLine();
  };
  assert( c && l );

  CubicCartesianEquationData ccd = c->cartesianEquationData();
  const LineData ld = l->lineData();

  bool valid = true;

  Coordinate cc = calcCubicLineIntersect( ccd.coeffs, ld, 1, valid );
  if ( valid ) sDrawPrelimPoint( p, cc );
  cc = calcCubicLineIntersect( ccd.coeffs, ld, 2, valid );
  if ( valid ) sDrawPrelimPoint( p, cc );
  cc = calcCubicLineIntersect( ccd.coeffs, ld, 3, valid );
  if ( valid ) sDrawPrelimPoint( p, cc );

  return;
}

Object::WantArgsResult CubicLineIntersectionPoint::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  bool gotc = false;
  bool gotl = false;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! gotc ) gotc = (*i)->toCubic();
    if ( ! gotl ) gotl = (*i)->toAbstractLine();
  };
  if ( size == 1 ) return ( gotc || gotl ) ? NotComplete : NotGood;
  if ( size == 2 ) return ( gotc && gotl ) ? Complete : NotGood;
  assert( false );
}

QString CubicLineIntersectionPoint::sUseText( const Objects&, const Object* o )
{
  if ( o->toCubic() ) return i18n( "Intersections of a line and this cubic" );
  if ( o->toLine() ) return i18n( "Intersections of a cubic and this line" );
  if ( o->toSegment() ) return i18n( "Intersections of a cubic and this segment" );
  if ( o->toRay() ) return i18n( "Intersections of a cubic and this ray" );
  assert( false );
}

Objects CubicLineIntersectionPoint::getParents() const
{
  Objects o;
  o.push_back( mcubic );
  o.push_back( mline );
  return o;
}

void CubicLineIntersectionPoint::calc()
{
  assert( mcubic && mline );
  Coordinate t;
  CubicCartesianEquationData cequation = mcubic->cartesianEquationData();
  t = calcCubicLineIntersect( cequation.coeffs,
                              mline->lineData(),
                              mroot, mvalid );
  if ( mvalid ) mC = t;
}

Object::prop_map CubicLineIntersectionPoint::getParams()
{
  prop_map map = Point::getParams();
  map["cubiclineintersect-root"] = QString::number( mroot );
  return map;
}

void CubicLineIntersectionPoint::setParams( const prop_map& map )
{
  Point::setParams( map );
  prop_map::const_iterator p = map.find("cubiclineintersect-root");
  if( p == map.end() ) mroot = 1;  // this is an error in the kig
                                   // file, but we ignore it..
  else
  {
    bool ok = true;
    mroot = p->second.toInt( &ok );
    assert( ok && ( mroot == 1 || mroot == 2 || mroot == 3 ) );
  };
}

Objects CubicLineIntersectionPoint::sMultiBuild( const Objects& args )
{
  CubicLineIntersectionPoint* a;
  CubicLineIntersectionPoint* b;
  CubicLineIntersectionPoint* c;
  a = new CubicLineIntersectionPoint( args );
  b = new CubicLineIntersectionPoint( args );
  c = new CubicLineIntersectionPoint( args );
  a->mroot = 1;
  b->mroot = 2;
  c->mroot = 3;
  Objects o;
  o.push_back( a );
  o.push_back( b );
  o.push_back( c );
  return o;
}

KigMode*
CubicLineIntersectionPoint::sConstructMode( MultiConstructibleType* ourtype,
                                            KigDocument* theDoc,
                                            NormalMode* previousMode )
{
  return new MultiConstructionMode( ourtype, previousMode, theDoc );
}

const QString CubicLineIntersectionPoint::sDescription()
{
  return i18n("Construct the intersections of a line and a cubic...");
}

const Coordinate calcCubicLineIntersect( const CubicCartesianEquationData& cu,
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

void calcCubicLineRestriction ( CubicCartesianEquationData data,
                                Coordinate p, Coordinate v,
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
