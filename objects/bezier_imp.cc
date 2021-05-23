// SPDX-FileCopyrightText: 2009 Petr Gajdos <pgajdos@suse.cz> and
// Maurizio Paolini <paolini@dmf.unicatt.it>

// SPDX-License-Identifier: GPL-2.0-or-later

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
  return Parent::numberOfProperties() + 3;
}

const QByteArrayList BezierImp::propertiesInternalNames() const
{
  QByteArrayList l = Parent::propertiesInternalNames();
  l += "bezier-number-of-control-points";
  l += "bezier-control-polygon";
  l += "cartesian-equation";    //on purpose, this has the same name as in LocusImp!
  assert( l.size() == BezierImp::numberOfProperties() );
  return l;
}

const QByteArrayList BezierImp::properties() const
{
  QByteArrayList l = Parent::properties();
  l += I18N_NOOP( "Number of control points" );
  l += I18N_NOOP( "Control polygon" );
  l += I18N_NOOP( "Cartesian Equation" );
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
  else if ( which == Parent::numberOfProperties() + 2 )
    return "kig_text";
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
    return new OpenPolygonalImp( mpoints );
  }
  else if ( which == Parent::numberOfProperties() + 2 )
  {
    // cartesian equation
    return new StringImp( cartesianEquationString( w ) );
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
    BezierImp::stype(), "bezier_quadratic",
    I18N_NOOP( "Bézier Quadratic" ),
    I18N_NOOP( "Select this Bézier Quadratic" ),
    I18N_NOOP( "Select Bézier Quadratic %1" ),
    I18N_NOOP( "Remove a Bézier Quadratic" ),
    I18N_NOOP( "Add a Bézier Quadratic" ),
    I18N_NOOP( "Move a Bézier Quadratic" ),
    I18N_NOOP( "Attach to this Bézier Quadratic" ),
    I18N_NOOP( "Show a Bézier Quadratic" ),
    I18N_NOOP( "Hide a Bézier Quadratic" )
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

const Coordinate BezierImp::getPoint( double p, const KigDocument& doc ) const
{
  /*
   *  Algorithm de Casteljau
   */
  doc.mcachedparam = p;
  return deCasteljau( mpoints.size() - 1, 0, p );
}

/*
 *  Rational Bézier Curve
 */

RationalBezierImp::RationalBezierImp( const std::vector<Coordinate>& points, const std::vector<double>& weights )
{
  uint npoints = points.size();
  Coordinate centerofmassn = Coordinate( 0, 0 );
  double totalweight = 0;

  assert(points.size() == weights.size());

  for ( uint i = 0; i < npoints; ++i )
  {
    centerofmassn += points[i];
    totalweight += weights[i];
  }
  mpoints = points;
  mweights = weights;
  mcenterofmass = centerofmassn/totalweight;
  mnpoints = npoints;
}

RationalBezierImp::~RationalBezierImp()
{
}

Coordinate RationalBezierImp::attachPoint() const
{
  return mcenterofmass;
}

ObjectImp* RationalBezierImp::transform( const Transformation& t ) const
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
  return new RationalBezierImp( np, mweights );
}

void RationalBezierImp::draw( KigPainter& p ) const
{
  p.drawCurve( this );
}

bool RationalBezierImp::inRect( const Rect& r, int width, const KigWidget& w ) const
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

bool RationalBezierImp::valid() const
{
  if (mnpoints > 1 && mnpoints == mweights.size())
    return true;
  else
    return false;
}

int RationalBezierImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 3;
}

const QByteArrayList RationalBezierImp::propertiesInternalNames() const
{
  QByteArrayList l = Parent::propertiesInternalNames();
  l += "bezier-number-of-control-points";
  l += "bezier-control-polygon";
  l += "cartesian-equation";    //on purpose, this has the same name as in LocusImp!
  assert( l.size() == RationalBezierImp::numberOfProperties() );
  return l;
}

const QByteArrayList RationalBezierImp::properties() const
{
  QByteArrayList l = Parent::properties();
  l += I18N_NOOP( "Number of control points" );
  l += I18N_NOOP( "Control polygon" );
  l += I18N_NOOP( "Cartesian Equation" );
  assert( l.size() == RationalBezierImp::numberOfProperties() );
  return l;
}

const ObjectImpType* RationalBezierImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return RationalBezierImp::stype();
}

const char* RationalBezierImp::iconForProperty( int which ) const
{
  assert( which < RationalBezierImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  else if ( which == Parent::numberOfProperties() )
    return "en"; // number of sides
  else if ( which == Parent::numberOfProperties() + 1 )
    return "controlpolygon";
  else if ( which == Parent::numberOfProperties() + 2 )
    return "kig_text";
  else assert( false );
  return "";
}

ObjectImp* RationalBezierImp::property( int which, const KigDocument& w ) const
{
  assert( which < RationalBezierImp::numberOfProperties() );
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
    return new OpenPolygonalImp( mpoints );
  }
  else if ( which == Parent::numberOfProperties() + 2 )
  {
    // cartesian equation
    return new StringImp( cartesianEquationString( w ) );
  }
  else assert( false );
  return new InvalidImp;
}

const std::vector<Coordinate> RationalBezierImp::points() const
{
  return mpoints;
}

uint RationalBezierImp::npoints() const
{
  return mnpoints;
}

RationalBezierImp* RationalBezierImp::copy() const
{
  return new RationalBezierImp( mpoints, mweights );
}

void RationalBezierImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool RationalBezierImp::equals( const ObjectImp& rhs ) const
{
  // that's actually sufficient condition for equality of
  // RBks; there are many RBks which don't have the same
  // control points
  return rhs.inherits( BezierImp::stype() ) &&
    static_cast<const BezierImp&>( rhs ).points() == mpoints;
}

const ObjectImpType* RationalBezierImp::stype()
{
  static const ObjectImpType R(
    Parent::stype(), "rational_bezier",
    I18N_NOOP( "Rational Bézier Curve" ),
    I18N_NOOP( "Select this Rational Bézier Curve" ),
    I18N_NOOP( "Select Rational Bézier Curve %1" ),
    I18N_NOOP( "Remove a Rational Bézier Curve" ),
    I18N_NOOP( "Add a Rational Bézier Curve" ),
    I18N_NOOP( "Move a Rational Bézier Curve" ),
    I18N_NOOP( "Attach to this Rational Bézier Curve" ),
    I18N_NOOP( "Show a Rational Bézier Curve" ),
    I18N_NOOP( "Hide a Rational Bézier Curve" )
    );

  return &R;
}

const ObjectImpType* RationalBezierImp::stype2()
{
  static const ObjectImpType R3(
    BezierImp::stype(), "rational_bezier_quadratic",
    I18N_NOOP( "Rational Bézier Quadratic" ),
    I18N_NOOP( "Select this Rational Bézier Quadratic" ),
    I18N_NOOP( "Select Rational Bézier Quadratic %1" ),
    I18N_NOOP( "Remove a Rational Bézier Quadratic" ),
    I18N_NOOP( "Add a Rational Bézier Quadratic" ),
    I18N_NOOP( "Move a Rational Bézier Quadratic" ),
    I18N_NOOP( "Attach to this Rational Bézier Quadratic" ),
    I18N_NOOP( "Show a Rational Bézier Quadratic" ),
    I18N_NOOP( "Hide a Rational Bézier Quadratic" )
    );

  return &R3;
}

const ObjectImpType* RationalBezierImp::stype3()
{
  static const ObjectImpType R4(
    BezierImp::stype(), "rational_bezier_cubic",
    I18N_NOOP( "Rational Bézier Cubic" ),
    I18N_NOOP( "Select this Rational Bézier Cubic" ),
    I18N_NOOP( "Select Rational Bézier Cubic %1" ),
    I18N_NOOP( "Remove a Rational Bézier Cubic" ),
    I18N_NOOP( "Add a Rational Bézier Cubic" ),
    I18N_NOOP( "Move a Rational Bézier Cubic" ),
    I18N_NOOP( "Attach to this Rational Bézier Cubic" ),
    I18N_NOOP( "Show a Rational Bézier Cubic" ),
    I18N_NOOP( "Hide a Rational Bézier Cubic" )
    );

  return &R4;
}

const ObjectImpType* RationalBezierImp::type() const
{
  uint n = mpoints.size();

  if ( n == 3 ) return RationalBezierImp::stype2();
  if ( n == 4 ) return RationalBezierImp::stype3();
  return RationalBezierImp::stype();
}

bool RationalBezierImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  assert( which < RationalBezierImp::numberOfProperties() );
  if ( which < Parent::numberOfProperties() )
    return Parent::isPropertyDefinedOnOrThroughThisImp( which );
  return false;
}

Rect RationalBezierImp::surroundingRect() const
{
  Rect r( 0., 0., 0., 0. );
  for ( uint i = 0; i < mpoints.size(); ++i )
  {
    r.setContains( mpoints[i] );
  }
  return r;
}

bool RationalBezierImp::contains( const Coordinate& o, int width, const KigWidget& w ) const
{
  return internalContainsPoint( o, w.screenInfo().normalMiss( width ), w.document() );
}

bool RationalBezierImp::containsPoint( const Coordinate& p, const KigDocument& doc ) const
{
  return internalContainsPoint( p, test_threshold, doc );
}

bool RationalBezierImp::internalContainsPoint( const Coordinate& p, double threshold, const KigDocument& doc ) const
{
  double param = getParam( p, doc );
  double dist = getDist( param, p, doc );
  return fabs( dist ) <= threshold;
}

Coordinate RationalBezierImp::deCasteljauPoints( unsigned int m, unsigned int k, double p ) const
{
  if (m == 0) return mpoints[k]*mweights[k];
  assert( k + 1 <= mnpoints );
  return (1 - p)*deCasteljauPoints( m - 1, k, p ) + p*deCasteljauPoints( m - 1, k + 1, p );
}

double RationalBezierImp::deCasteljauWeights( unsigned int m, unsigned int k, double p ) const
{
  if (m == 0) return mweights[k];
  assert( k + 1 <= mnpoints );
  return (1 - p)*deCasteljauWeights( m - 1, k, p ) + p*deCasteljauWeights( m - 1, k + 1, p );
}

const Coordinate RationalBezierImp::getPoint( double p, const KigDocument& doc ) const
{
  /*
   *  Algorithm de Casteljau
   */
  doc.mcachedparam = p;
  return deCasteljauPoints( mpoints.size() - 1, 0, p ) / deCasteljauWeights( mweights.size() - 1, 0, p );
}

