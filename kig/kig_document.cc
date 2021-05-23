// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "kig_document.h"

#include "../misc/common.h"
#include "../objects/object_calcer.h"
#include "../objects/object_holder.h"
#include "../objects/point_imp.h"
#include "../objects/polygon_imp.h"
#include "../misc/coordinate_system.h"
#include "../misc/rect.h"
#include "../misc/calcpaths.h"

#include <assert.h>
#include <iterator>
#include <cmath>

KigDocument::KigDocument( const std::set<ObjectHolder*> &objects, CoordinateSystem* coordsystem,
                          bool showgrid, bool showaxes, bool nv )
  : mobjects( objects ), mcoordsystem( coordsystem ), mshowgrid( showgrid ),
    mshowaxes( showaxes ), mnightvision( nv ), mcoordinatePrecision( -1 ), mcachedparam( 0.0 )
{
}

const CoordinateSystem& KigDocument::coordinateSystem() const
{
  assert( mcoordsystem );
  return *mcoordsystem;
}

const std::vector<ObjectHolder*> KigDocument::objects() const
{
  return std::vector<ObjectHolder*>( mobjects.begin(), mobjects.end() );
}

const std::set<ObjectHolder*>& KigDocument::objectsSet() const
{
  return mobjects;
}

void KigDocument::setCoordinateSystem( CoordinateSystem* s )
{
  delete switchCoordinateSystem( s );
}

CoordinateSystem* KigDocument::switchCoordinateSystem( CoordinateSystem* s )
{
  CoordinateSystem* ret = mcoordsystem;
  mcoordsystem = s;
  return ret;
}

std::vector<ObjectHolder*> KigDocument::whatAmIOn( const Coordinate& p, const KigWidget& w ) const
{
  std::vector<ObjectHolder*> ret;
  std::vector<ObjectHolder*> curves;
  std::vector<ObjectHolder*> fatobjects;
  for ( std::set<ObjectHolder*>::const_iterator i = mobjects.begin();
        i != mobjects.end(); ++i )
  {
    if(!(*i)->contains(p, w, mnightvision)) continue;
    const ObjectImp* oimp = (*i)->imp();
    if ( oimp->inherits( PointImp::stype() ) ) ret.push_back( *i );
    else
      if ( !oimp->inherits( FilledPolygonImp::stype() ) ) curves.push_back( *i );
      else
      {
        fatobjects.push_back( *i );
      }
  };
  std::copy( curves.begin(), curves.end(), std::back_inserter( ret ) );
  std::copy( fatobjects.begin(), fatobjects.end(), std::back_inserter( ret ) );
  return ret;
}

std::vector<ObjectHolder*> KigDocument::whatIsInHere( const Rect& p, const KigWidget& w )
{
  std::vector<ObjectHolder*> ret;
  std::vector<ObjectHolder*> nonpoints;
  for ( std::set<ObjectHolder*>::const_iterator i = mobjects.begin();
        i != mobjects.end(); ++i )
  {
    if(! (*i)->inRect( p, w ) ) continue;
    if ( (*i)->imp()->inherits( PointImp::stype() ) ) ret.push_back( *i );
    else nonpoints.push_back( *i );
  };
  std::copy( nonpoints.begin(), nonpoints.end(), std::back_inserter( ret ) );
  return ret;
}

Rect KigDocument::suggestedRect() const
{
  bool rectInited = false;
  Rect r(0.,0.,0.,0.);
  for ( std::set<ObjectHolder*>::const_iterator i = mobjects.begin();
        i != mobjects.end(); ++i )
  {
    if ( (*i)->shown() )
    {
      Rect cr = (*i)->imp()->surroundingRect();
      if ( ! cr.valid() ) continue;
      if( !rectInited )
      {
        r = cr;
        rectInited = true;
      }
      else
        r.eat( cr );
    };
  };

  if ( ! rectInited )
    return Rect( -5.5, -5.5, 11., 11. );
  r.setContains( Coordinate( 0, 0 ) );
  if( r.width() == 0 ) r.setWidth( 1 );
  if( r.height() == 0 ) r.setHeight( 1 );
  Coordinate center = r.center();
  r *= 2;
  r.setCenter(center);
  return r;
}

void KigDocument::addObject( ObjectHolder* o )
{
  mobjects.insert( o );
}

void KigDocument::addObjects( const std::vector<ObjectHolder*>& os )
{
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
    ( *i )->calc( *this );
  std::copy( os.begin(), os.end(), std::inserter( mobjects, mobjects.begin() ) );
}

void KigDocument::delObject( ObjectHolder* o )
{
  mobjects.erase( o );
}

void KigDocument::delObjects( const std::vector<ObjectHolder*>& os )
{
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
    mobjects.erase( *i );
}

KigDocument::KigDocument()
  : mcoordsystem( new EuclideanCoords )
{
  mshowgrid = true;
  mshowaxes = true;
  mnightvision = false;
  mcoordinatePrecision = -1;
}

KigDocument::~KigDocument()
{
  typedef std::set<ObjectHolder*> s;
  for ( s::iterator i = mobjects.begin(); i != mobjects.end(); ++i ) {
    delete *i;
  }
  delete mcoordsystem;
}

void KigDocument::setGrid( bool showgrid )
{
  mshowgrid = showgrid;
}

bool KigDocument::grid() const
{
  return mshowgrid;
}

void KigDocument::setAxes( bool showaxes )
{
  mshowaxes = showaxes;
}

void KigDocument::setNightVision( bool nv )
{
  mnightvision = nv;
}

void KigDocument::setCoordinatePrecision( int precision )
{
  mcoordinatePrecision = precision;
}

bool KigDocument::axes() const
{
  return mshowaxes;
}

bool KigDocument::getNightVision() const
{
  return mnightvision;
}

bool KigDocument::isUserSpecifiedCoordinatePrecision() const
{
  return mcoordinatePrecision != -1;
}

int KigDocument::getCoordinatePrecision() const
{
  if( mcoordinatePrecision == -1 )
  {
    // we use default coordinate precision calculation
    Rect sr = suggestedRect();
    double m = kigMax( sr.width(), sr.height() );

    return kigMax( 0, (int) ( 3 - log10( m ) ) );
  }

  return mcoordinatePrecision;
}

/*
 * scan all points in the document and find those that simultaneously
 * belong to two curves c1 and c2.  This is required when the user
 * asks for intersections between a conic and a line (or two circles)
 * and one of the intersections is already present.  In this case
 * we construct the "other" intersection instead of the two standard
 * intersections.  Note that this is a completely different construction
 * which takes into account the known intersection in order to reduce
 * the resolvant equation to first degree, thus ensuring that the newly
 * constructed point is *always* the other intersection
 */

std::vector<ObjectCalcer*>
KigDocument::findIntersectionPoints( const ObjectCalcer* c1,
                                     const ObjectCalcer* c2) const
{
  std::vector<ObjectCalcer*> ret;
  for ( std::set<ObjectHolder*>::const_iterator i = mobjects.begin();
        i != mobjects.end(); ++i )
  {
    if ( !(*i)->imp()->inherits( PointImp::stype() ) ) continue;
    ObjectCalcer* o = (*i)->calcer();
    if ( isPointOnCurve( o, c1 ) &&
         isPointOnCurve( o, c2 ) )
    {
      ret.push_back( o );
    }
  };

  return ret;
}
