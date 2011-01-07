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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "locus_imp.h"

#include "bogus_imp.h"
#include "point_imp.h"
#include "../misc/object_hierarchy.h"
#include "../misc/kigpainter.h"
#include "../misc/coordinate.h"
#include "../misc/common.h"

#include "../kig/kig_view.h"

#include <klocale.h>

#include <cmath>

using namespace std;

static double cachedparam = 0.0;

LocusImp::~LocusImp()
{
  delete mcurve;
}

ObjectImp* LocusImp::transform( const Transformation& t ) const
{
  return new LocusImp( mcurve->copy(), mhier.transformFinalObject( t ) );
}

void LocusImp::draw( KigPainter& p ) const
{
  p.drawCurve( this );
}

bool LocusImp::contains( const Coordinate& p, int width, const KigWidget& w ) const
{
  return internalContainsPoint( p, w.screenInfo().normalMiss( width ), w.document() );
}

bool LocusImp::inRect( const Rect&, int, const KigWidget& ) const
{
  // TODO ?
  return false;
}

const Coordinate LocusImp::getPoint( double param, const KigDocument& doc ) const
{
  Coordinate arg = mcurve->getPoint( param, doc );
  if ( ! arg.valid() ) return arg;
  PointImp argimp( arg );
  Args args;
  args.push_back( &argimp );
  vector<ObjectImp*> calcret = mhier.calc( args, doc );
  assert( calcret.size() == 1 );
  ObjectImp* imp = calcret.front();
  Coordinate ret;
  if ( imp->inherits( PointImp::stype() ) )
  {
    cachedparam = param;
    ret = static_cast<PointImp*>( imp )->coordinate();
  }
  else
    ret = Coordinate::invalidCoord();

  delete imp;
  return ret;
}

LocusImp::LocusImp( CurveImp* curve, const ObjectHierarchy& hier )
  : mcurve( curve ), mhier( hier )
{
}

const uint LocusImp::numberOfProperties() const
{
  return Parent::numberOfProperties();
}

const QCStringList LocusImp::propertiesInternalNames() const
{
  return Parent::propertiesInternalNames();
}

const QCStringList LocusImp::properties() const
{
  return Parent::properties();
}

const ObjectImpType* LocusImp::impRequirementForProperty( uint which ) const
{
  return Parent::impRequirementForProperty( which );
}

const char* LocusImp::iconForProperty( uint which ) const
{
  return Parent::iconForProperty( which );
}

ObjectImp* LocusImp::property( uint which, const KigDocument& w ) const
{
  return Parent::property( which, w );
}

LocusImp* LocusImp::copy() const
{
  return new LocusImp( mcurve->copy(), mhier );
}

const CurveImp* LocusImp::curve() const
{
  return mcurve;
}

const ObjectHierarchy& LocusImp::hierarchy() const
{
  return mhier;
}

/**
 * This function returns the distance between the point with parameter
 * param and point p.  param is allowed to not be between 0 and 1, in
 * which case we consider only the decimal part.
 */
double LocusImp::getDist(double param, const Coordinate& p, const KigDocument& doc) const
{
  param = fmod( param, 1 );
  if( param < 0 ) param += 1.;
  Coordinate p1 = getPoint( param, doc );
  // i don't think the p1.valid() switch is really necessary, but I
  // prefer to not take any chances :)
  return p1.valid() ? ( p1 - p ).length() : +double_inf;
}

/**
 * This function searches starting from x1 for the first interval in
 * which the function of the distance from the point at coordinate x
 * starts to increase.  The range found is returned in the parameters
 * x1 and x2: [x1,x2].
 */
void LocusImp::getInterval( double& x1, double& x2,
                            double incr,const Coordinate& p,
                            const KigDocument& doc) const
{
  double mm = getDist( x1, p, doc);
  double mm1 = getDist( x2, p, doc);
  if( mm  <= mm1 ) return;
  else
  {
    double x3 = x2 + incr;
    double mm2 = getDist (x3, p, doc);
    while( mm > mm1 & mm1 > mm2 )
    {
      x1 = x2;
      x2 = x3;
      x3 = x2 + incr;
      mm = mm1;
      mm1 = mm2;
      mm2 = getDist (x3, p, doc);
    }
    x2=x3;
  }
}

double LocusImp::getParam( const Coordinate& p, const KigDocument& doc ) const
{
  // this function ( and related functions like getInterval etc. ) is
  // written by Franco Pasquarelli <pasqui@dmf.bs.unicatt.it>.
  // I ( domi ) have adapted and documented it a bit.

  if ( cachedparam >= 0. && cachedparam <= 1. &&
       getPoint ( cachedparam, doc ) == p ) return cachedparam;

  // consider the function that returns the distance for a point at
  // parameter x to the locus for a given parameter x.  What we do
  // here is look for the global minimum of this function.  We do that
  // by dividing the range ( [0,1] ) into N parts,  and start looking
  // for a local minimum from there on.  If we find one, we keep it if
  // it is the lowest of all the ones we've already found..

  const int N = 50;
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

  while( j < N )
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
        while( mm1 > mm2 & j <= N )
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

/**
 * This function calculates the parameter of the point that realizes the
 * minimum in [a,b] of the distance between the points of the locus and
 * the point of coordinate p, using the golden ration method.
 */
double LocusImp::getParamofmin( double a, double b,
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

void LocusImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool LocusImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( LocusImp::stype() ) &&
    static_cast<const LocusImp&>( rhs ).curve()->equals( *curve() ) &&
    static_cast<const LocusImp&>( rhs ).hierarchy() == hierarchy();
}

const ObjectImpType* LocusImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "locus",
    I18N_NOOP( "locus" ),
    I18N_NOOP( "Select this locus" ),
    I18N_NOOP( "Select locus %1" ),
    I18N_NOOP( "Remove a Locus" ),
    I18N_NOOP( "Add a Locus" ),
    I18N_NOOP( "Move a Locus" ),
    I18N_NOOP( "Attach to this locus" ),
    I18N_NOOP( "Show a Locus" ),
    I18N_NOOP( "Hide a Locus" )
    );
  return &t;
}

const ObjectImpType* LocusImp::type() const
{
  return LocusImp::stype();
}

bool LocusImp::containsPoint( const Coordinate& p, const KigDocument& doc ) const
{
  return internalContainsPoint( p, test_threshold, doc );
}

bool LocusImp::internalContainsPoint( const Coordinate& p, double threshold, const KigDocument& doc ) const
{
  double param = getParam( p, doc );
  double dist = getDist( param, p, doc );
  return fabs( dist ) <= threshold;
}

bool LocusImp::isPropertyDefinedOnOrThroughThisImp( uint which ) const
{
  return Parent::isPropertyDefinedOnOrThroughThisImp( which );
}

Rect LocusImp::surroundingRect() const
{
  // it's probably possible to calculate this, if it exists, but we
  // don't for now.
  return Rect::invalidRect();
}
