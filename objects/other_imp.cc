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

#include "../misc/screeninfo.h"
#include "../misc/common.h"
#include "../misc/kigtransform.h"
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

bool AngleImp::contains( const Coordinate& p, int width, const ScreenInfo& si ) const
{
  double radius = 50*si.pixelWidth();

  if ( std::fabs( (p-mpoint).length() - radius ) > si.normalMiss( width ) )
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

ObjectImp* AngleImp::property( uint which, const KigWidget& w ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, w );
  if ( which == Parent::numberOfProperties() )
    return new DoubleImp( size() );
  else if ( which == Parent::numberOfProperties() + 1 )
    return new DoubleImp( size() * 180 / M_PI );
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

ObjectImp* VectorImp::transform( const Transformation& t ) const
{
  bool valid = true;
  Coordinate ta = t.apply( ma, valid );
  Coordinate tb = t.apply( mb, valid );
  if ( !valid ) return new InvalidImp;
  else return new VectorImp( ta, tb );
}

void VectorImp::draw( KigPainter& p ) const
{
  p.drawVector( ma, mb );
}

bool VectorImp::contains( const Coordinate& o, int width, const ScreenInfo& si ) const
{
  return isOnSegment( o, ma, mb, si.normalMiss( width ) );
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

ObjectImp* VectorImp::property( uint which, const KigWidget& w ) const
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

const Coordinate VectorImp::dir() const
{
  return mb - ma;
}

const char* AngleImp::baseName() const
{
  return I18N_NOOP( "angle" );
}

const char* VectorImp::baseName() const
{
  return I18N_NOOP( "vector" );
}

int AngleImp::id() const
{
  return ID_AngleImp;
}

int VectorImp::id() const
{
  return ID_VectorImp;
}
