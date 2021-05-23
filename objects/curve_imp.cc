// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "curve_imp.h"
#include "../misc/common.h"
#include "../misc/coordinate.h"
#include "../misc/kignumerics.h"
#include "../misc/equationstring.h"
#include "../kig/kig_document.h"

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
  Coordinate p1 = getPoint( t1, doc);
  double f1 = (p1 - p).length();
  Coordinate p2 = getPoint( t2, doc);
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
      //p1 = getPoint( fmod( t1, 1. ), doc);
      p1 = getPoint( t1, doc);
      f1 = (p1 - p).length();
    }
    else
    {
      t1 = t2;
      t2 = a + r2*(b - a);
      f1 = f2;
      // p2 = getPoint( fmod( t2, 1. ), doc);
      p2 = getPoint( t2, doc);
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
//  param = fmod( param, 1 );
//  if( param < 0 ) param += 1.;
  if( param < 0 ) param = 0.;
  if( param > 1 ) param = 1.;
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

  // mp: the following two lines are especially useful in conjunction to
  // differential geometry constructions like tangent, center of curvature,...
  // such constructions need to recover the param associated to a (constrained)
  // PointImp, but do not have direct access to it since it is a parent of the
  // calcer accociated to the ConstrainedPointType, whereas we only have the
  // ObjectImps of the Curve and of the Point; in such case the only possibility
  // consists in a call to getParam, which is unnecessarily heavy since the PointImp
  // was itself computed previously using getPoint.  So the param used in getPoint
  // is cached in LocusImp, BezierImp, ... and then checked for validity here.

  if ( doc.mcachedparam >= 0. && doc.mcachedparam <= 1. &&
       getPoint ( doc.mcachedparam, doc ) == p )
    return doc.mcachedparam;

  // consider the function that returns the distance for a point at
  // parameter x to the locus for a given parameter x.  What we do
  // here is look for the global minimum of this function.  We do that
  // by dividing the range ( [0,1] ) into N parts,  and start looking
  // for a local minimum from there on.  If we find one, we keep it if
  // it is the lowest of all the ones we've already found..

  const int N = 64;
  const double incr = 1. / (double) N;

  // xm is the best parameter we've found so far, fxm is the distance
  // to the locus from that point.  We start with some
  // pseudo-values.
  // (mp) note that if the distance is actually increasing in the
  // whole interval [0,1] this value will be returned in the end.
  double xm = 0.;
  double fxm = getDist( xm, p, doc );
  double x1,x2;

  double mm[N+1];
  mm[0] = fxm;

  for(int j = 1; j < N + 1; j++)
  {
    // [x1,x2] is the range we're currently considering..
    x1 = j * incr;

    // check the range x1,x2 for the first local maximum..
    double mm1 = getDist( x1, p, doc);
    if( mm1 < fxm)
    {
      xm=x1;
      fxm=mm1;
    }
    mm[j]=mm1;
  }
  if ( xm == 0.)
  {
     x1=0.;
     x2=incr;
  }
  else if ( xm >= 1.)
  {
     x1=1.-incr;
     x2=1.;
  }
  else
  {
    x1=xm-incr;
    x2=xm+incr;
  }
  double xm1 = getParamofmin( x1, x2, p, doc);
  double fxm1 = getDist( xm1, p, doc );
  if( fxm1 < fxm )
  {
    // we found a new minimum, save it..
    xm=xm1;
    fxm=fxm1;
  }
  for(int j = 1; j < N - 1; j++)
  {
    if (mm[j] < mm[j-1] && mm[j] < mm[j+1])
    {
       if ( fxm > 2*mm[j]-mm[j-1] || fxm > 2*mm[j]-mm[j+1])
       {
          xm1 = getParamofmin( (j-1)*incr, (j+1)*incr, p, doc);
          fxm1 = getDist( xm1, p, doc );
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

//This function is used to obtain a pseudo-random number using bitwise operators
//it probably should be moved elsewhere, or made completely local...
//
double CurveImp::revert (int n) const
{
 int nl = n;
 double b = 1.0;
 double t = 0.0;

 assert (n > 0);
 while (nl)
 {
   b /= 2;
   if (nl & 1)
        t += b;
   nl >>= 1;
 }
  t += b/2 - b*(qrand() / (RAND_MAX+1.0));
  assert(t <1 && t >0);
  return (t);
}

QString CurveImp::cartesianEquationString (const KigDocument& doc ) const
{
  EquationString ret = EquationString( QLatin1String("") );
  const double threshold = 1e-10;
  const int degmax=6;
  const int N = (degmax + 1)*(degmax + 2)/2;
  const int M = N - 1;
  double rows[M][N];
  double *matrix[M];
  double sol[N];
  double dist;
  int deg, degx, degy, deglocus;
  int i=0, j=0;
  bool test=true;
  double exp[degmax+2];
  double x, y;
  double xi, yi;
  double t;
  double f, fx, fy;
  int m, n, k, idx;
  int exchange[N];
  bool needsign = false;
  Coordinate point[M];
  Coordinate p;
  for(m = 0; m < M; m++) matrix[m] = rows[m];
  for(deglocus = 1; deglocus <= degmax; ++deglocus)
  {
    n=((deglocus+1)*(deglocus+2))/2;
    m=n-1;
    k=1;
    for(i=0; i<m; ++i)
    {
      do{
        point[i] = getPoint(revert(k++), doc);
        } while(point[i].valid() == false); // now we have a valid point
      xi = point[i].x;
      yi = point[i].y;
      j=0;
      matrix[i][j]=1;
      for(deg=1; deg<=deglocus; ++deg)
      {
        for(degy=0; degy<=deg; ++degy)
        {
          j++;
          degx=deg-degy;
          if(degx==0)
            {
              matrix[i][j]=matrix[i][j-deg-1]*yi;
            }
          else
          {
            matrix[i][j]=matrix[i][j-deg]*xi;
          }
        }
      }
    }
    assert(j==m);
    GaussianElimination( matrix, m, n, exchange );
    BackwardSubstitution( matrix, m, n, exchange, sol);
    for(i=0; i<m; ++i)
    {
      test = true;
      do{
        t=revert(k++);
        p = getPoint(t,doc);
      } while(p.valid() == false);
      x = p.x;
      y = p.y;
      exp[0]=1;
      exp[1]=1;
      fx=sol[1];
      fy=sol[2];
      f= sol[0];
      //now we test if p is a point of the curve
      idx=1;
      for (deg=1; deg<=deglocus; ++deg){
        for(degy=0; degy<=deg; ++degy)
        {
          degx = deg - degy;
          if(degy!=deg)
          {
            exp[degy] *= x;
          }
          else
          {
            exp[degy] *= y;
            exp[degy+1]=exp[degy];
          }
          if(deg!= deglocus)
          {
// i_col(dx, dy) =
//    d = dx+dy;
//    icol = (d*(d+1)/2) + dy
//
// qui ci sarebbe i_col (degx+1, degy)
// quindi degnuovo = degvecchio + 1, ovvero
// il nuovo valore di d*(d+1)/2 e'
//    (dv+2)*(dv+1)/2 = dv*(dv+1)/2 + (dv+1)
// quindi sarebbe giusto "deg+1+idx", se idx non fosse stato incrementato!
              fx += (degx+1)*exp[degy]*sol[deg+idx+1];
// qui ci sarebbe i_col (degx, degy+1)
              fy += (degy+1)*exp[degy]*sol[deg+idx+2];
          }
          f += exp[degy]*sol[idx++];
        }
      }
      dist=fabs(f)/(fabs(fx) + fabs(fy));
      if (dist > threshold){
        test=false;
        break;
        }
    }
    if(test==true) // now we can costruct the cartesian equation of the locus
    {
      assert ( deglocus >= 0 && deglocus <= degmax );
      for (deg = deglocus; deg > 0; --deg)
      {
        j = deg*(deg+1)/2;
        for (degy = 0; degy <= deg; ++degy)
          {
            degx = deg - degy;
            ret.addTerm( sol[j++], ret.xnym(degx,degy), needsign );
           }
      }
      ret.addTerm( sol[0], QLatin1String(""), needsign );
      ret.append( " = 0" );
      return ret;
    }
  }
  ret = i18n("Possibly trascendental curve");
  return ret;
}

