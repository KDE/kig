// line_imp.cc
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "line_imp.h"

#include "bogus_imp.h"

#include "../misc/rect.h"
#include "../misc/common.h"
#include "../misc/kigtransform.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"

#include <cmath>
using namespace std;

AbstractLineImp::AbstractLineImp( const Coordinate& a, const Coordinate& b )
  : mdata( a, b )
{
}

AbstractLineImp::~AbstractLineImp()
{
}

bool AbstractLineImp::inRect( const Rect& r, int width, const ScreenInfo& si ) const
{
  double miss = si.normalMiss( width );
  if ( fabs( mdata.a.x - mdata.b.x ) <= 1e-7 )
  {
    // too small to be useful..
    return r.contains( Coordinate( mdata.a.x, r.center().y ), miss );
  }
  Coordinate dir = mdata.dir();
  double m = dir.y / dir.x;
  double lefty = mdata.a.y + m * ( r.left() - mdata.a.x );
  double righty = mdata.a.y + m * ( r.right() - mdata.a.x );
  double minv = dir.x / dir.y;
  double bottomx = mdata.a.x + minv * ( r.bottom() - mdata.a.y );
  double topx = mdata.a.x + minv * ( r.top() - mdata.a.y );

  // these are the intersections between the line, and the lines
  // defined by the sides of the rectangle.
  Coordinate leftint( r.left(), lefty );
  Coordinate rightint( r.right(), righty );
  Coordinate bottomint( bottomx, r.bottom() );
  Coordinate topint( topx, r.top() );

  // For each intersection, we now check if we contain the
  // intersection ( this might not be the case for a segment, when the
  // intersection is not between the begin and end point.. ) and if
  // the rect contains the intersection..  If it does, we have a winner..
  return
    ( contains( leftint, width, si ) && r.contains( leftint, miss ) ) ||
    ( contains( rightint, width, si ) && r.contains( rightint, miss ) ) ||
    ( contains( bottomint, width, si ) && r.contains( bottomint, miss ) ) ||
    ( contains( topint, width, si ) && r.contains( topint, miss ) );
}

const uint AbstractLineImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 2;
}

ObjectImp* AbstractLineImp::property( uint which, const KigDocument& w ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  if ( which == Parent::numberOfProperties() )
    return new DoubleImp( slope() );
  if ( which == Parent::numberOfProperties() + 1 )
    return new StringImp( equationString() );
  else assert( false );
}

const QCStringList AbstractLineImp::propertiesInternalNames() const
{
  QCStringList l = Parent::propertiesInternalNames();
  l << I18N_NOOP( "slope" );
  l << I18N_NOOP( "equation" );
  assert( l.size() == AbstractLineImp::numberOfProperties() );
  return l;
}

const QCStringList AbstractLineImp::properties() const
{
  QCStringList l = Parent::properties();
  l << I18N_NOOP( "Slope" );
  l << I18N_NOOP( "Equation" );
  assert( l.size() == AbstractLineImp::numberOfProperties() );
  return l;
}

int SegmentImp::type() const
{
  return 0;
}

int RayImp::type() const
{
  return 1;
}

int LineImp::type() const
{
  return 2;
}

const uint SegmentImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 1;
}

const QCStringList SegmentImp::propertiesInternalNames() const
{
  QCStringList s = Parent::propertiesInternalNames();
  s << I18N_NOOP( "length" );
  assert( s.size() == SegmentImp::numberOfProperties() );
  return s;
}

const QCStringList SegmentImp::properties() const
{
  QCStringList s = Parent::properties();
  s << I18N_NOOP( "Length" );
  assert( s.size() == SegmentImp::numberOfProperties() );
  return s;
}

ObjectImp* SegmentImp::property( uint which, const KigDocument& w ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  if ( which == Parent::numberOfProperties() )
    return new DoubleImp( mdata.dir().length() );
  else assert( false );
}

double AbstractLineImp::slope() const
{
  Coordinate diff = mdata.dir();
  return diff.y / diff.x;
}

const QString AbstractLineImp::equationString() const
{
  QString ret = QString::fromUtf8( "y = %1x + %2" );
  Coordinate p = mdata.a;
  Coordinate q = mdata.b;

  double m = ( q.y - p.y ) / ( q.x - p.x );
  double r = ( q.y - p.y ) * p.x / ( q.x - p.x ) + p.y;

  ret = ret.arg( m, 0, 'g', 3 );
  ret = ret.arg( r, 0, 'g', 3 );

  return ret;
}

void SegmentImp::draw( KigPainter& p ) const
{
  p.drawSegment( mdata );
}

bool SegmentImp::contains( const Coordinate& p, int width, const ScreenInfo& si ) const
{
  return isOnSegment( p, mdata.a, mdata.b, si.normalMiss( width ) );
}

void RayImp::draw( KigPainter& p ) const
{
  p.drawRay( mdata );
}

bool RayImp::contains( const Coordinate& p, int width, const ScreenInfo& si ) const
{
  return isOnRay( p, mdata.a, mdata.b, si.normalMiss( width ) );
}

void LineImp::draw( KigPainter& p ) const
{
  p.drawLine( mdata );
}

bool LineImp::contains( const Coordinate& p, int width, const ScreenInfo& si ) const
{
  return isOnLine( p, mdata.a, mdata.b, si.normalMiss( width ) );
}

SegmentImp::SegmentImp( const Coordinate& a, const Coordinate& b )
  : AbstractLineImp( a, b )
{
}

RayImp::RayImp( const Coordinate& a, const Coordinate& b )
  : AbstractLineImp( a, b )
{
}

LineImp::LineImp( const Coordinate& a, const Coordinate& b )
  : AbstractLineImp( a, b )
{
}

SegmentImp* SegmentImp::copy() const
{
  return new SegmentImp( mdata );
}

RayImp* RayImp::copy() const
{
  return new RayImp( mdata );
}

LineImp* LineImp::copy() const
{
  return new LineImp( mdata );
}

bool AbstractLineImp::inherits( int typeID ) const
{
  return typeID == ID_LineImp ? true : Parent::inherits( typeID );
}

const Coordinate SegmentImp::getPoint( double param, const KigDocument& ) const
{
  return mdata.a + mdata.dir()*param;
}

double SegmentImp::getParam( const Coordinate& p, const KigDocument& ) const
{
  Coordinate pt = calcPointOnPerpend( data(), p );
  pt = calcIntersectionPoint( data(), LineData( p, pt ) );
  // if pt is over the end of the segment ( i.e. it's on the line
  // which the segment is a part of, but not of the segment itself..;
  // ) we set it to one of the end points of the segment...
  if ((pt - mdata.a).length() > mdata.dir().length() )
    pt = mdata.b;
  else if ( (pt- mdata.b).length() > mdata.dir().length() )
    pt = mdata.a;
  if (mdata.b == mdata.a) return 0;
  return ((pt - mdata.a).length())/(mdata.dir().length());
}

LineData AbstractLineImp::data() const
{
  return mdata;
}

const Coordinate RayImp::getPoint( double param, const KigDocument& ) const
{
  param = 1.0/param - 1.0;
  return mdata.a + mdata.dir()*param;
}

double RayImp::getParam( const Coordinate& p, const KigDocument& ) const
{
  const LineData ld = data();
  Coordinate pt = calcPointOnPerpend( ld, p );
  pt = calcIntersectionPoint( ld, LineData( p, pt ));
  // if pt is over the end of the ray ( i.e. it's on the line
  // which the ray is a part of, but not of the ray itself..;
  // ) we set it to the start point of the ray...
  Coordinate dir = ld.dir();
  pt -= ld.a;
  double param;
  if ( dir.x != 0 ) param = pt.x / dir.x;
  else if ( dir.y != 0 ) param = pt.y / dir.y;
  else param = 0.;
  if ( param < 0. ) param = 0.;

  // mp:  let's try with 1/(x+1),  this reverses the mapping, but
  // should allow to take advantage of the tightness of floating point
  // numbers near zero, in order to get more possible positions near
  // infinity

  param = 1./( param + 1. );

  assert( param >= 0. && param <= 1. );
  return param;
}

const Coordinate LineImp::getPoint( double p, const KigDocument& ) const
{
  // inspired upon KSeg

  // we need to spread the points over the line, it should also come near
  // the (infinite) end of the line, but most points should be near
  // the two points we contain...
  if ( p <= 0. ) p = 1e-6;
  if ( p >= 1. ) p = 1 - 1e-6;
  p = 2*p - 1;
  if (p > 0) p = p/(1 - p);
    else p = p/(1 + p);
//  p *= 1024;    // such multiplying factor could be useful in order to
                  // have more points near infinity, at the expense of
                  // points near ma and mb
  return mdata.a + p*mdata.dir();
}

double LineImp::getParam( const Coordinate& point, const KigDocument& ) const
{
  // somewhat the reverse of getPoint, although it also supports
  // points not on the line...

  Coordinate pa = point - mdata.a;
  Coordinate ba = mdata.dir();
  double balsq = ba.x*ba.x + ba.y*ba.y;
  assert (balsq > 0);

  double p = (pa.x*ba.x + pa.y*ba.y)/balsq;
//  p /= 1024;
  if (p > 0) p = p/(1+p);
    else p = p/(1-p);

  return 0.5*(p + 1);
}

ObjectImp* SegmentImp::transform( const Transformation& t ) const
{
  bool valid = true;
  Coordinate na = t.apply( mdata.a, valid );
  Coordinate nb = t.apply( mdata.b, valid );
  if( valid ) return new SegmentImp( na, nb );
  else return new InvalidImp();
}

ObjectImp* LineImp::transform( const Transformation& t ) const
{
  bool valid = true;
  Coordinate na = t.apply( mdata.a, valid );
  Coordinate nb = t.apply( mdata.b, valid );
  if ( valid ) return new LineImp( na, nb );
  else return new InvalidImp();
}

ObjectImp* RayImp::transform( const Transformation& t ) const
{
  bool valid = true;
  Coordinate na = t.apply( mdata.a, valid );
  Coordinate nb = t.apply( mdata.b, valid );
  if ( valid ) return new RayImp( na, nb );
  else return new InvalidImp();
}

AbstractLineImp::AbstractLineImp( const LineData& d )
  : mdata( d )
{
}

SegmentImp::SegmentImp( const LineData& d )
  : AbstractLineImp( d )
{
}

RayImp::RayImp( const LineData& d )
  : AbstractLineImp( d )
{
}

LineImp::LineImp( const LineData& d )
  : AbstractLineImp( d )
{
}

const char* SegmentImp::baseName() const
{
  return I18N_NOOP( "segment" );
}

const char* RayImp::baseName() const
{
  return I18N_NOOP( "ray" );
}

const char* LineImp::baseName() const
{
  return I18N_NOOP( "line" );
}

bool SegmentImp::inherits( int type ) const
{
  return type == ID_SegmentImp ? true : Parent::inherits( type );
}

double SegmentImp::length() const
{
  return mdata.length();
}

bool RayImp::inherits( int type ) const
{
  return type == ID_RayImp ? true : Parent::inherits( type );
}

int SegmentImp::id() const
{
  return ID_SegmentImp;
}

int RayImp::id() const
{
  return ID_RayImp;
}

int LineImp::id() const
{
  return ID_LineImp;
}
