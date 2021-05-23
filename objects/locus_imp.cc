// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "locus_imp.h"

#include "bogus_imp.h"
#include "point_imp.h"
#include "../misc/object_hierarchy.h"
#include "../misc/kigpainter.h"
#include "../misc/coordinate.h"
#include "../misc/common.h"
#include "../misc/kignumerics.h"
#include "../misc/equationstring.h"
#include "../kig/kig_view.h"
#include "../kig/kig_document.h"

#include <cmath>

using namespace std;

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
    doc.mcachedparam = param;
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

int LocusImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 1;
}

const QByteArrayList LocusImp::propertiesInternalNames() const
{
  QByteArrayList l = Parent::propertiesInternalNames();
  l << "cartesian-equation";
  assert( l.size() == LocusImp::numberOfProperties() );
  return l;
}

const QByteArrayList LocusImp::properties() const
{
  QByteArrayList l = Parent::properties();
  l << I18N_NOOP( "Cartesian Equation" );
  assert( l.size() == LocusImp::numberOfProperties() );
  return l;
}

const ObjectImpType* LocusImp::impRequirementForProperty( int which ) const
{
  return Parent::impRequirementForProperty( which );
}

const char* LocusImp::iconForProperty( int which ) const
{
  int pnum = 0;
  if ( which < Parent::numberOfProperties() )
	return Parent::iconForProperty( which );
  if ( which == Parent::numberOfProperties() + pnum++ )
	return "kig_text"; // cartesian equation string
  else 
	assert( false );
  return "";
}

ObjectImp* LocusImp::property( int which, const KigDocument& w ) const
{
  int pnum = 0;

  if ( which < Parent::numberOfProperties() )
	return Parent::property( which, w );
  if ( which == Parent::numberOfProperties() + pnum++ )
	return new StringImp( cartesianEquationString( w ) );
  else 
	assert( false );
  return new InvalidImp;
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
 * This function searches starting from x1 for the first interval in
 * which the function of the distance from the point at coordinate x
 * starts to increase.  The range found is returned in the parameters
 * x1 and x2: [x1,x2].
 * TODO: is this function still used? should it be removed?
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
    while( mm > mm1 && mm1 > mm2 )
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

bool LocusImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  return Parent::isPropertyDefinedOnOrThroughThisImp( which );
}

Rect LocusImp::surroundingRect() const
{
  // it's probably possible to calculate this, if it exists, but we
  // don't for now.
  return Rect::invalidRect();
}

/*

--- moved in CurveImp ---

double LocusImp::revert (int n) const

QString LocusImp::cartesianEquationString (const KigDocument& doc ) const
{

 */
