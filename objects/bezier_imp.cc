// Copyright (C)  2009  Petr Gajdos <pgajdos@suse.cz>

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

#include "bezier_imp.h"

#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"
#include "polygon_imp.h"

#include "../misc/common.h"
#include "../misc/coordinate.h"
#include "../misc/kigpainter.h"
#include "../misc/kigtransform.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"

#include <klocale.h>

#include <cmath>
//#include <gsl/gsl_poly.h>

/*
 *   Polynomial Bézier Curve
 */

BezierImp::BezierImp( const std::vector<Coordinate>& points )
{
  uint npoints = points.size();
  Coordinate centerofmassn = Coordinate( 0, 0 );

  for ( uint i = 0; i < npoints; ++i )
  {
    centerofmassn += points[i];
  }
  mpoints = points;
  mcenterofmass = centerofmassn/npoints;
  mnpoints = npoints;
}

BezierImp::~BezierImp()
{
}

Coordinate BezierImp::attachPoint() const
{
  return mcenterofmass;
}

ObjectImp* BezierImp::transform( const Transformation& t ) const
{
/*
 * To perform affine transformation of Bezier curve is the same as transform
 * control points and then draw Bezier curve with this control points.
 */
  if ( ! t.isAffine() )  /* Don't know how to do it in general so far. */
  {
    return new InvalidImp;
  }
  std::vector<Coordinate> np;
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    Coordinate nc = t.apply( mpoints[i] );
    if ( !nc.valid() )
      return new InvalidImp;
    np.push_back( nc );
  }
  return new BezierImp( np );
}

void BezierImp::draw( KigPainter& p ) const
{
  p.drawCurve( this );
}

bool BezierImp::inRect( const Rect& r, int width, const KigWidget& w ) const
{
  bool ret = false;
  uint reduceddim = mpoints.size() - 1;
  for ( uint i = 0; !ret && i < reduceddim; ++i )
  {
    SegmentImp s( mpoints[i], mpoints[i+1] );
    ret = lineInRect( r, mpoints[i], mpoints[i+1], width, &s, w );
  }
  if ( !ret )
  {
    SegmentImp s( mpoints[reduceddim], mpoints[0] );
    ret = lineInRect( r, mpoints[reduceddim], mpoints[0], width, &s, w );
  }

  return ret;
}

bool BezierImp::valid() const
{
  if (mnpoints > 1)
    return true;
  else
    return false;
}

int BezierImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 2;
}

const QByteArrayList BezierImp::propertiesInternalNames() const
{
  QByteArrayList l = Parent::propertiesInternalNames();
  l += "bezier-number-of-control-points";
  l += "bezier-control-polygon";
  assert( l.size() == BezierImp::numberOfProperties() );
  return l;
}

const QByteArrayList BezierImp::properties() const
{
  QByteArrayList l = Parent::properties();
  l += I18N_NOOP( "Number of control points" );
  l += I18N_NOOP( "Control polygon" );
  assert( l.size() == BezierImp::numberOfProperties() );
  return l;
}

const ObjectImpType* BezierImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return BezierImp::stype();
}

const char* BezierImp::iconForProperty( int which ) const
{
  assert( which < BezierImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "en"; // number of sides
  else if ( which == Parent::numberOfProperties() + 1 )
    return "controlpolygon";
  else assert( false );
  return "";
}

ObjectImp* BezierImp::property( int which, const KigDocument& w ) const
{
  assert( which < BezierImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  else if ( which == Parent::numberOfProperties() )
  {
    // number of points
    return new IntImp( mnpoints );
  }
  else if ( which == Parent::numberOfProperties() + 1 )
  {
    // control polygon
    return new PolygonImp( mpoints, true );
  }
  else assert( false );
  return new InvalidImp;
}

const std::vector<Coordinate> BezierImp::points() const
{
  return mpoints;
}

uint BezierImp::npoints() const
{
  return mnpoints;
}

BezierImp* BezierImp::copy() const
{
  return new BezierImp( mpoints );
}

void BezierImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool BezierImp::equals( const ObjectImp& rhs ) const
{
  // that's actually sufficient condition for equality
  // of Bks; there are many equal Bks with distinct
  // control points
  return rhs.inherits( BezierImp::stype() ) &&
    static_cast<const BezierImp&>( rhs ).points() == mpoints;
}

const ObjectImpType* BezierImp::stype()
{
  static const ObjectImpType B(
    Parent::stype(), "bezier",
    I18N_NOOP( "Bézier Curve" ),
    I18N_NOOP( "Select this Bézier Curve" ),
    I18N_NOOP( "Select Bézier Curve %1" ),
    I18N_NOOP( "Remove a Bézier Curve" ),
    I18N_NOOP( "Add a Bézier Curve" ),
    I18N_NOOP( "Move a Bézier Curve" ),
    I18N_NOOP( "Attach to this Bézier Curve" ),
    I18N_NOOP( "Show a Bézier Curve" ),
    I18N_NOOP( "Hide a Bézier Curve" )
    );

  return &B;
}

const ObjectImpType* BezierImp::stype2()
{
  static const ObjectImpType B3(
    BezierImp::stype(), "bezier_quadric",
    I18N_NOOP( "Bézier Quadric" ),
    I18N_NOOP( "Select this Bézier Quadric" ),
    I18N_NOOP( "Select Bézier Quadric %1" ),
    I18N_NOOP( "Remove a Bézier Quadric" ),
    I18N_NOOP( "Add a Bézier Quadric" ),
    I18N_NOOP( "Move a Bézier Quadric" ),
    I18N_NOOP( "Attach to this Bézier Quadric" ),
    I18N_NOOP( "Show a Bézier Quadric" ),
    I18N_NOOP( "Hide a Bézier Quadric" )
    );

  return &B3;
}

const ObjectImpType* BezierImp::stype3()
{
  static const ObjectImpType B4(
    BezierImp::stype(), "bezier_cubic",
    I18N_NOOP( "Bézier Cubic" ),
    I18N_NOOP( "Select this Bézier Cubic" ),
    I18N_NOOP( "Select Bézier Cubic %1" ),
    I18N_NOOP( "Remove a Bézier Cubic" ),
    I18N_NOOP( "Add a Bézier Cubic" ),
    I18N_NOOP( "Move a Bézier Cubic" ),
    I18N_NOOP( "Attach to this Bézier Cubic" ),
    I18N_NOOP( "Show a Bézier Cubic" ),
    I18N_NOOP( "Hide a Bézier Cubic" )
    );

  return &B4;
}

const ObjectImpType* BezierImp::type() const
{
  uint n = mpoints.size();

  if ( n == 3 ) return BezierImp::stype2();
  if ( n == 4 ) return BezierImp::stype3();
  return BezierImp::stype();
}

bool BezierImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  assert( which < BezierImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

Rect BezierImp::surroundingRect() const
{
  Rect r( 0., 0., 0., 0. );
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    r.setContains( mpoints[i] );
  }
  return r;
}

bool BezierImp::contains( const Coordinate& o, int width, const KigWidget& w ) const
{
  return internalContainsPoint( o, w.screenInfo().normalMiss( width ), w.document() );
}

bool BezierImp::containsPoint( const Coordinate& p, const KigDocument& doc ) const
{
  return internalContainsPoint( p, test_threshold, doc );
}

bool BezierImp::internalContainsPoint( const Coordinate& p, double threshold, const KigDocument& doc ) const
{
  double param = getParam( p, doc );
  double dist = getDist( param, p, doc );
  return fabs( dist ) <= threshold;
}

Coordinate BezierImp::deCasteljau( unsigned int m, unsigned int k, double p ) const
{
  if (m == 0) return mpoints[k];
  assert( k + 1 <= mnpoints );
  return (1 - p)*deCasteljau( m - 1, k, p ) + p*deCasteljau( m - 1, k + 1, p );
}

const Coordinate BezierImp::getPoint( double p, const KigDocument& ) const
{
  /*
   *  Algorithm de Casteljau
   */
  return deCasteljau( mpoints.size() - 1, 0, p );
}

