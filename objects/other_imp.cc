// other_imp.cc
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

#include "other_imp.h"

#include "bogus_imp.h"
#include "property.h"

#include "../misc/screeninfo.h"
#include "../misc/common.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"

#include <math.h>

AngleImp::~AngleImp()
{
}

ObjectImp* AngleImp::transform( const Transformation& ) const
{
  // TODO ?
  return new InvalidImp;
}

void AngleImp::draw( KigPainter& p ) const
{
  p.drawAngle( mpoint, mstartangle, mangle );
}

AngleImp::AngleImp( const Coordinate& pt, double start_angle_in_radials,
                    double angle_in_radials )
  : mpoint( pt ), mstartangle( start_angle_in_radials ),
    mangle( angle_in_radials )
{
}

bool AngleImp::contains( const Coordinate& p, const ScreenInfo& si ) const
{
  double radius = 50*si.pixelWidth();

  if ( std::fabs( (p-mpoint).length() - radius ) > si.normalMiss() )
    return false;

  // and next we check if the angle is appropriate...
  Coordinate vect = p - mpoint;
  double angle = std::atan2( vect.y, vect.x );
  while ( angle < mstartangle ) angle += 2*M_PI;
  return angle <= mstartangle + mangle;
}

bool AngleImp::inRect( const Rect& r ) const
{
  // TODO ?
  return r.contains( mpoint );
}

const uint AngleImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 2;
}

const QCStringList AngleImp::properties() const
{
  QCStringList l = Parent::properties();
  l << I18N_NOOP( "Angle in Radians" );
  l << I18N_NOOP( "Angle in Degrees" );
  assert( l.size() == AngleImp::numberOfProperties() );
  return l;
}

const Property AngleImp::property( uint which, const KigWidget& w ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  if ( which == Parent::numberOfProperties() )
    return Property( size() );
  else if ( which == Parent::numberOfProperties() + 1 )
    return Property( size() * 180 / M_PI );
  else assert( false );
}

const double AngleImp::size() const
{
  return mangle;
}

bool AngleImp::inherits( int typeID ) const
{
  return typeID == ID_AngleImp ? true : Parent::inherits( typeID );
}

ObjectImp* AngleImp::copy() const
{
  return new AngleImp( mpoint, mstartangle, mangle );
}

VectorImp::VectorImp( const Coordinate& a, const Coordinate& b )
  : ma( a ), mb( b )
{
}

VectorImp::~VectorImp()
{
}

ObjectImp* VectorImp::transform( const Transformation& ) const
{
  // TODO ?
  return new InvalidImp;
}

void VectorImp::draw( KigPainter& p ) const
{
  p.drawVector( ma, mb );
}

bool VectorImp::contains( const Coordinate& o, const ScreenInfo& si ) const
{
  return isOnSegment( o, ma, mb, si.normalMiss() );
}

bool VectorImp::inRect( const Rect& ) const
{
  // TODO ?
  return false;
}

const uint VectorImp::numberOfProperties() const
{
  return Parent::numberOfProperties();
}

const QCStringList VectorImp::properties() const
{
  return Parent::properties();
}

const Property VectorImp::property( uint which, const KigWidget& w ) const
{
  return Parent::property( which, w );
}

bool VectorImp::inherits( int type ) const
{
  return type == ID_VectorImp ? true : Parent::inherits( type );
}

ObjectImp* VectorImp::copy() const
{
  return new VectorImp( ma, mb );
}

const Coordinate VectorImp::a()
{
  return ma;
}

const Coordinate VectorImp::b()
{
  return mb;
}

const Coordinate VectorImp::dir()
{
  return mb - ma;
}
