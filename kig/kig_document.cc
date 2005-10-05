// Copyright (C)  2004  Dominique Devriese <devriese@kde.org>

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

#include "kig_document.h"

#include "../objects/object_calcer.h"
#include "../objects/object_holder.h"
#include "../objects/point_imp.h"
#include "../objects/polygon_imp.h"
#include "../misc/coordinate_system.h"
#include "../misc/rect.h"

#include <assert.h>

KigDocument::KigDocument( std::set<ObjectHolder*> objects, CoordinateSystem* coordsystem,
                          bool showgrid, bool showaxes, bool nv )
  : mobjects( objects ), mcoordsystem( coordsystem ), mshowgrid( showgrid ),
    mshowaxes( showaxes ), mnightvision( nv )
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
    if ( (*i)->imp()->inherits( PointImp::stype() ) ) ret.push_back( *i );
    else 
      if ( !(*i)->imp()->inherits( PolygonImp::stype() ) ) curves.push_back( *i );
      else fatobjects.push_back( *i );
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

const bool KigDocument::grid() const
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

const bool KigDocument::axes() const
{
  return mshowaxes;
}

const bool KigDocument::getNightVision() const
{
  return mnightvision;
}
