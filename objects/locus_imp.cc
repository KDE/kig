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

LocusImp::~LocusImp()
{
  delete mhier;
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
  p.drawLocus( mcurve, *mhier );
}

bool LocusImp::contains( const Coordinate&, const ScreenInfo& ) const
{
  // TODO
  return false;
}

bool LocusImp::inRect( const Rect& ) const
{
  // TODO ?
  return false;
}

bool LocusImp::valid() const
{
  return true;
}

double LocusImp::getParam( const Coordinate& ) const
{
  // TODO
  return 0.5;
}

const Coordinate LocusImp::getPoint( double param ) const
{
  Coordinate arg = mcurve->getPoint( param );
   PointImp argimp( arg );
  Args args;
  args.push_back( &argimp );
  ObjectImp* imp = mhier->calc( args );
  Coordinate ret;
  if ( imp->inherits( ObjectImp::ID_PointImp ) )
    ret = static_cast<PointImp*>( imp )->coordinate();
  delete imp;
  return ret;
}

LocusImp::LocusImp( const Object* movingpoint, const Object* tracingpoint )
  : mcurve( 0 ), mhier( new ObjectHierarchy( Objects( const_cast<Object*>( movingpoint ) ),
                                             tracingpoint ) )
{
  // the ObjectType should make sure we have a ConstrainedPointImp
  // parent, we just assert it..
  assert( movingpoint->has( ObjectImp::ID_PointImp ) && movingpoint->parents().size() == 0 );
  mcurve = static_cast<const CurveImp*>( movingpoint->parents().front()->imp() );
}

