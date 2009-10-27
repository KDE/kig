// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "curve_imp.h"
#include "../misc/common.h"
#include "../misc/coordinate.h"

#include <cmath>

const ObjectImpType* CurveImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "curve",
    I18N_NOOP( "curve" ),
    I18N_NOOP( "Select this curve" ),
    I18N_NOOP( "Select curve %1" ),
    I18N_NOOP( "Remove a Curve" ),
    I18N_NOOP( "Add a Curve" ),
    I18N_NOOP( "Move a Curve" ),
    I18N_NOOP( "Attach to this curve" ),
    I18N_NOOP( "Show a Curve" ),
    I18N_NOOP( "Hide a Curve" )
    );
  return &t;
}

Coordinate CurveImp::attachPoint() const
{
  return Coordinate::invalidCoord();
}

/*
 *  Generic algorithm for getParam()
 */

/**
 * This function calculates the parameter of the point that realizes the
 * minimum in [a,b] of the distance between the points of the locus and
 * the point of coordinate p, using the golden ration method.
 */
double CurveImp::getParamofmin( double a, double b,
                                const Coordinate& p,
                                const KigDocument& doc ) const
{
  double epsilons = 1.e-08;
  double epsilonl = 2.e-02;

  //assert( a < b && a >= 0. && b <= 1.0);
  assert( a < b && a >= 0.);

  double r2 = ( sqrt( 5. ) - 1 ) / 2.; // golden ratio
  double r1 = 1. - r2;

  double t2 = a + r2 * ( b - a );
  double t1 = a + r1 * ( b - a );
  Coordinate p1 = getPoint( fmod( t1, 1. ), doc);
  double f1 = (p1 - p).length();
  Coordinate p2 = getPoint( fmod( t2, 1. ), doc);
  double f2 = (p2 - p).length();

  double fmin, tmin;
  if (f1 < f2)
  {
    b = t2;
    fmin = f1;
    tmin = t1;
  }
  else
  {
    a = t1;
    fmin = f2;
    tmin = t2;
  }

  while ( ( b - a ) > epsilons &&
          ( (p1 - p2).length() > 0.4 * fmin
            || (b - a) > epsilonl) &&
          fmin > 1.e-8 )
  {
    if ( f1 < f2 )
    {
      t2 = t1;
      t1 = a + r1*(b - a);
      f2 = f1;
      p1 = getPoint( fmod( t1, 1. ), doc);
      f1 = (p1 - p).length();
    }
    else
    {
      t1 = t2;
      t2 = a + r2*(b - a);
      f1 = f2;
      p2 = getPoint( fmod( t2, 1. ), doc);
      f2 = (p2 - p).length();
    }
    if ( f1 < f2 )
    {
      b = t2;
      fmin = f1;
      tmin = t1;
    }
    else
    {
      a = t1;
      fmin = f2;
      tmin = t2;
    }
  }

  return(tmin);
}

/**
 * This function returns the distance between the point with parameter
 * param and point p.  param is allowed to not be between 0 and 1, in
 * which case we consider only the decimal part.
 */
double CurveImp::getDist(double param, const Coordinate& p, const KigDocument& doc) const
{
  param = fmod( param, 1 );
  if( param < 0 ) param += 1.;
  Coordinate p1 = getPoint( param, doc );
  // i don't think the p1.valid() switch is really necessary, but I
  // prefer to not take any chances :)
  return p1.valid() ? ( p1 - p ).length() : +double_inf;
}

double CurveImp::getParam( const Coordinate& p, const KigDocument& doc ) const
{
  // this function ( and related functions like getInterval etc. ) is
  // written by Franco Pasquarelli <pasqui@dmf.bs.unicatt.it>.
  // I ( domi ) have adapted and documented it a bit.

  // consider the function that returns the distance for a point at
  // parameter x to the locus for a given parameter x.  What we do
  // here is look for the global minimum of this function.  We do that
  // by dividing the range ( [0,1] ) into N parts,  and start looking
  // for a local minimum from there on.  If we find one, we keep it if
  // it is the lowest of all the ones we've already found..

  const int N = 100;
  const double incr = 1. / (double) N;

  // xm is the best parameter we've found so far, fxm is the distance
  // to the locus from that point.  We start with some
  // pseudo-values.
  // (mp) note that if the distance is actually increasing in the
  // whole interval [0,1] this value will be returned in the end.
  double xm = 0.;
  double fxm = getDist( xm, p, doc );

  int j = 0;
  double mm = fxm;

  while( j < N - 1 )
  {
    // [x1,x2] is the range we're currently considering..
    double x1 = j * incr;
    double x2 = x1 + incr;

    // check the range x1,x2 for the first local maximum..
    double mm1 = getDist( x2, p, doc);
    double mm2;
    j++;
    if( mm  < mm1 )
       mm = mm1;

    else
    {
      if ( mm > mm1 )
      {
        double x3 = x2 + incr;
        mm2 = getDist (x3, p, doc);
        j++;
        while( mm1 > mm2 && j <= N )
        {
          x1 = x2;
          x2 = x3;
          x3 = x2 + incr;
          mm = mm1;
          mm1 = mm2;
          mm2 = getDist (x3, p, doc);
          j++;
        }
        x2 = x3;
      }
      else
        mm2 = mm1;

      if ( mm1 <= mm2 )
      {
        mm = mm2;

        double xm1 = getParamofmin( x1, x2, p, doc);
        double fxm1 = getDist( xm1, p, doc );
        if( fxm1 < fxm )
        {
          // we found a new minimum, save it..
          xm=xm1;
          fxm=fxm1;
        }
      }
    }
  }
  return xm;
}

