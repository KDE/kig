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

#include <klocale.h>

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

bool LocusImp::contains( const Coordinate&, int, const ScreenInfo& ) const
{
  // TODO
  return false;
}

bool LocusImp::inRect( const Rect&, int, const ScreenInfo& ) const
{
  // TODO ?
  return false;
}

bool LocusImp::valid() const
{
  return true;
}

double LocusImp::getParam( const Coordinate&, const KigDocument& ) const
{
  // TODO
  return 0.5;
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

