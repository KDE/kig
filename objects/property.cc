// property.cc
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

#include "property.h"

#include "../objects/object.h"
#include "../misc/coordinate.h"
#include "../misc/coordinate_system.h"

#include "../kig/kig_part.h"

#include <qstring.h>

#include <assert.h>

const int Property::type() const
{
  return mtype;
}

const double Property::doubleData() const
{
  assert ( mtype == Double );
  return mdata.d;
}

const QString Property::qstringData() const
{
  assert( mtype == String );
  return *mdata.qs;
}

Property::Property( const QString& s )
{
  mtype = String;
  mdata.qs = new QString( s );
}

Property::Property( const double d )
{
  mtype = Double;
  mdata.d = d;
}

Property::~Property()
{
  switch( mtype )
  {
  case String:
    delete mdata.qs;
    break;
  case Coord:
    delete mdata.coord;
    break;
  default:
    break;
  };
}

QString Property::toString( const KigDocument& d, const KigWidget& w )
{
  switch( mtype )
  {
  case String:
    return *mdata.qs;
  case Double:
    return QString::number( mdata.d );
  case Coord:
    return d.coordinateSystem().fromScreen( *mdata.coord, w );
  default:
    assert( false );
  };
}

Property TextLabelProperty::value( const KigWidget& w ) const
{
  assert( obj );
  assert( index != static_cast<unsigned int>( -1 ) );
  return obj->property( index, w );
}

bool TextLabelProperty::valid()
{
  return obj && index != static_cast<unsigned int>( -1 );
}

Property::Property( const Coordinate& c )
{
  mtype = Coord;
  mdata.coord = new Coordinate( c );
}

const Coordinate Property::coordData() const
{
  assert( mtype == Coord );
  return *mdata.coord;
}

QString TextLabelProperty::getString( const KigDocument& d, const KigWidget& w ) const
{
  return value( w ).toString( d, w );
}


