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

#include <qstring.h>

const int Property::type()
{
  return mtype;
}

const double Property::doubleData()
{
  assert ( mtype == Double );
  return mdata.d;
}

const QString Property::qstringData()
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
  if ( mtype == String ) delete mdata.qs;
}

QString Property::toString()
{
  switch( mtype )
  {
  case String:
    return *mdata.qs;
  case Double:
    return QString::number( mdata.d );
  default:
    assert( false );
  };
}

Property TextLabelProperty::value()
{
  assert( obj );
  assert( index != static_cast<unsigned int>( -1 ) );
  return obj->property( index );
}

bool TextLabelProperty::valid()
{
  return obj && index != static_cast<unsigned int>( -1 );
}


