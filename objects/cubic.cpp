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

bool Cubic::contains (const Coordinate& o, const ScreenInfo& si ) const
{
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
  return Coordinate(0,0);
};

double Cubic::getParam (const Coordinate& p) const
{
    return 0; 
};

const QCString Cubic::sBaseTypeName()
{
  return I18N_NOOP("cubic");
}

Cubic::Cubic( const Cubic& c )
  : Curve( c ), cequation( c.cequation )
{
}

Cubic* Cubic::toCubic()
{
  return this;
}

const Cubic* Cubic::toCubic() const
{
  return this;
}

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

  if ( size <= 5 )
  {
    p.setPen(QPen (Qt::green, 1));
    p.drawConic(
      ConicPolarEquationData(
        calcConicThroughPoints( points, zerotilt, parabolaifzt, ysymmetry )
        )
      );
    return;
  }

  CubicCartesianEquationData equation = calcCubicThroughPoints( points );
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

  // renormalize:
  b /= a;
  c /= a;
  d /= a;
  a = 1.0;

// computing the coefficients of the Sturm sequence

  double p1a = 2*b*b - 6*c;
  double p1b = b*c - 9*d;
  double p0a = c*p1a*p1a + p1b*(3*p1b - 2*b*p1a);

  int varbottom = calcCubicVariations (ymin, a, b, c, d, p1a, p1b, p0a);
  int vartop = calcCubicVariations (ymax, a, b, c, d, p1a, p1b, p0a);
  numroots = vartop - varbottom;
  valid = false;
  if (root <= varbottom || root > vartop ) return 0.0;

  valid = true;

  // now use bisection to separate the required root
  double dy = (ymax - ymin)/2;
  double sigma = dy/1000;
  while ( dy > sigma && vartop - varbottom > 1 )
  {
    double ymiddle = ymin + dy;
    int varmiddle = calcCubicVariations (ymiddle, a, b, c, d, p1a, p1b, p0a);
    if ( varmiddle < root )   // I am below
    {
      ymin = ymiddle;
      varbottom = varmiddle;
    } else {
      ymax = ymiddle;
      vartop = varmiddle;
    }
    dy /= 2;
  }

  /*
   * now [ymin, ymax] enclose a single root, try using Newton
   */
  if ( vartop - varbottom == 1 )
  {
    double fval1 = a;     // double check...
    double fval2 = a;
    fval1 = b + ymin*fval1;
    fval2 = b + ymax*fval2;
    fval1 = c + ymin*fval1;
    fval2 = c + ymax*fval2;
    fval1 = d + ymin*fval1;
    fval2 = d + ymax*fval2;
    assert ( fval1 * fval2 <= 0 );
    return calcCubicRootwithNewton ( ymin, ymax, a, b, c, d, 1e-8, valid );
  }
  else   // probably a double root here!
    return ( ymin + ymax )/2;

}
