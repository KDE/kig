// locus_imp.cc
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

#include "locus_imp.h"

#include "bogus_imp.h"
#include "point_imp.h"
#include "object.h"
#include "../misc/object_hierarchy.h"
#include "../misc/kigpainter.h"
#include "../misc/coordinate.h"

#include "../kig/kig_view.h"

#include <klocale.h>

#include <cmath>

using namespace std;

LocusImp::~LocusImp()
{
  delete mcurve;
}

bool LocusImp::inherits( int type ) const
{
  return type == ID_LocusImp ? true : Parent::inherits( type );
}

ObjectImp* LocusImp::transform( const Transformation& ) const
{
  return new InvalidImp;
}

void LocusImp::draw( KigPainter& p ) const
{
  p.drawLocus( mcurve, mhier );
}

bool LocusImp::contains( const Coordinate& p, int width, const KigWidget& w ) const
{
  double param = getParam( p, w.document() );
  double dist = getDist( param, p, w.document() );
  return fabs( dist ) <= w.screenInfo().normalMiss( width );
}

bool LocusImp::inRect( const Rect&, int, const KigWidget& ) const
{
  // TODO ?
  return false;
}

bool LocusImp::valid() const
{
  return true;
}

const Coordinate LocusImp::getPoint( double param, const KigDocument& doc ) const
{
  Coordinate arg = mcurve->getPoint( param, doc );
  PointImp argimp( arg );
  Args args;
  args.push_back( &argimp );
  std::vector<ObjectImp*> calcret = mhier.calc( args, doc );
  assert( calcret.size() == 1 );
  ObjectImp* imp = calcret.front();
  Coordinate ret;
  if ( imp->inherits( ObjectImp::ID_PointImp ) )
    ret = static_cast<PointImp*>( imp )->coordinate();
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

int LocusImp::impRequirementForProperty( uint which ) const
{
  return Parent::impRequirementForProperty( which );
}

ObjectImp* LocusImp::property( uint which, const KigDocument& w ) const
{
  return Parent::property( which, w );
}

LocusImp* LocusImp::copy() const
{
  return new LocusImp( mcurve->copy(), mhier );
}

const char* LocusImp::baseName() const
{
  return I18N_NOOP( "locus" );
}

const CurveImp* LocusImp::curve() const
{
  return mcurve;
}

const ObjectHierarchy& LocusImp::hierarchy() const
{
  return mhier;
}

int LocusImp::id() const
{
  return ID_LocusImp;
}

double LocusImp::getDist(double param, const Coordinate& p, const KigDocument& doc) const
{
  param = fmod( param, 1 );
  if( param < 0 ) param += 1.;
  Coordinate p1 = getPoint( param, doc );
  return ( p1 - p ).length();
}

void LocusImp::getInterval( double& x1, double& x2,
                            double& incr,const Coordinate& p,
                            const KigDocument& doc) const
{
  double epsilon = incr/100;
  double x3 = x1 + epsilon;
  double mm = getDist(x1,p,doc);
  double mm1 = getDist(x3,p,doc);
  if (mm < mm1) return;
  else
  {
    while (mm > mm1)
    {
      x1 = x1 + 10 * epsilon;
      mm = getDist( x1, p, doc );
      x2 = x1 + epsilon;
      mm1 = getDist( x2, p, doc );
      if( mm > mm1 ) x3 = x2;
    }
    x2=x1;
    x1=x3;
  }
}

double LocusImp::getParam( const Coordinate& p, const KigDocument& doc ) const
{
  double epsilon=1.e-4;
  int N=50;
  double fxm=10000000.;
  double xm=2.;
  double incr=1./(double) N;
  double x1=0.;
  int j = 1;
  while (j <= N)
  {
    x1=(j-1)*incr;
    double x2=x1+incr;
    getInterval( x1, x2, incr, p, doc );
    j++;
    if ( fabs(x1-(j-2)*incr) > 1.e-8)
    {
      int it=1;
      int i = 1;
      int jj = 1;
      while( (x2-x1)/(double) (2*jj) > epsilon)
      {
        int M=i+jj;
	i=jj;
	jj=M;
	it=it+1;
      }
      int t[3];
      t[0]=jj;
      t[1]=i+jj;
      t[2]=t[1]+t[0];
      double xm1 = getParamofmin( x1, x2, it, p, t, doc);
      double fxm1 = getDist( xm1, p, doc );
      if( fxm1 < fxm )
      {
        xm=xm1;
	fxm=fxm1;
      }
      while ((j-1)*incr < x2 && j<=N) j++;
    }
  }
  return xm;
}

double LocusImp::getParamofmin( double a, double b, int it,
                                const Coordinate& p,int t[3],
                                const KigDocument& doc ) const
{
  double epsilon=(b-a)/(4*t[2]);

  double x=(b-a)/t[2];
  double x1=a+t[0]*x;
  double x2=a+t[1]*x;
  double fx1=getDist(x1,p,doc) ;
  double fx2=getDist(x2,p,doc);
  if (fx1 < fx2)
    b=x2;
  else
    a=x1;

  for( int k=1; k <= it - 2; ++k )
  {
    if ( fx1 < fx2 )
    {
      x=x1;
      x1=a+x2-x1;
      x2=x;
      fx2=fx1;
      fx1=getDist(x1,p,doc);
    }
    else
    {
      x=x2;
      x2=b-x2+x1;
      x1=x;
      fx1=fx2;
      fx2=getDist(x2,p,doc);
    }
    if (fx1 < fx2 )
      b=x2;
    else
      a=x1;
  }
  x1=(a+b)/2.;
  x2=x1+epsilon;
  x1=x1-epsilon;
  fx1=getDist(x1,p,doc);
  fx2=getDist(x2,p,doc);
  if (fx1 < fx2)
    b=x2;
  else
    a=x1;

  x=(a+b)/2.;
  return(x);
}
