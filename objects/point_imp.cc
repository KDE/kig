// point_imp.cc
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

#include "point_imp.h"

#include "bogus_imp.h"
#include "../misc/kigtransform.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"

PointImp::PointImp( const Coordinate& c )
  : mc( c )
{
}

void PointImp::draw( KigPainter& p ) const
{
  p.drawFatPoint( mc );
}

bool PointImp::contains( const Coordinate& p, int width, const ScreenInfo& si ) const
{
  int twidth = width == -1 ? 5 : width;
  return (p - mc).length() - twidth*si.pixelWidth() < 0;
}

bool PointImp::inRect( const Rect& r ) const
{
  return r.contains( mc );
}

const uint PointImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 1;
}

const QCStringList PointImp::propertiesInternalNames() const
{
  QCStringList l = Parent::propertiesInternalNames();
  l << I18N_NOOP( "coordinate" );
  assert( l.size() == PointImp::numberOfProperties() );
  return l;
}

const QCStringList PointImp::properties() const
{
  QCStringList l = Parent::properties();
  l << I18N_NOOP( "Coordinate" );
  assert( l.size() == PointImp::numberOfProperties() );
  return l;
}

ObjectImp* PointImp::property( uint which, const KigDocument& d ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, d );
  if ( which == Parent::numberOfProperties() )
    return new PointImp( mc );
  else assert( false );
}

PointImp::~PointImp()
{
}

PointImp* PointImp::copy() const
{
  return new PointImp( mc );
}

bool PointImp::inherits( int typeID ) const
{
  return typeID == ID_PointImp ? true : Parent::inherits( typeID );
}

ObjectImp* PointImp::transform( const Transformation& t ) const
{
  bool valid = true;
  Coordinate nc = t.apply( mc, valid );
  if ( valid ) return new PointImp( nc );
  else return new InvalidImp();
}

void PointImp::setCoordinate( const Coordinate& c )
{
  mc = c;
}

const char* PointImp::baseName() const
{
  return I18N_NOOP( "point" );
}

int PointImp::id() const
{
  return ID_PointImp;
}

