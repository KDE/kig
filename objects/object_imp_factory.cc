// object_imp_factory.cc
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

#include "object_imp_factory.h"

#include "object_imp.h"
#include "bogus_imp.h"

const char* ObjectImpFactory::string_names[numstrings] =
{
  "Double",
  "Integer",

  "Point",
  "Line",
  "Label",
  "Angle",
  "Vector",
  "Locus",
  "Circle",
  "Conic",
  "Cubic"
};

const QCString ObjectImpFactory::nameforid( int id ) const
{
  return string_names[id];
}

int ObjectImpFactory::idforname( const QCString& n ) const
{
  for ( const char** i = string_names; i < string_names + numstrings; ++i )
    if ( n == *i ) return i - string_names;
  assert( false );
  return -1;
}

ObjectImp* ObjectImpFactory::deserialize( int id, const QString dd ) const
{
  bool ok = true;
  switch( id )
  {
  case ObjectImp::ID_DoubleImp:
  {
    double d = dd.toDouble( &ok );
    return ok ? new DoubleImp( d ) : 0;
  };
  case ObjectImp::ID_IntImp:
  {
    int i = dd.toInt( &ok );
    return ok ? new IntImp( i ) : 0;
  };
  case ObjectImp::ID_StringImp:
    return new StringImp( dd );
  default:
    return 0;
  };
}

QString ObjectImpFactory::serialize( const ObjectImp& d ) const
{
  if ( d.inherits( ObjectImp::ID_DoubleImp ) )
    return QString::number( static_cast<const DoubleImp&>( d ).data() );
  else if ( d.inherits( ObjectImp::ID_IntImp ) )
    return QString::number( static_cast<const IntImp&>( d ).data() );
  else if ( d.inherits( ObjectImp::ID_StringImp ) )
    return static_cast<const StringImp&>( d ).data();
  else
    return 0;
}
