// bogus_imp.cc
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

#include "bogus_imp.h"

#include <qcstring.h>
#include <qstringlist.h>
#include <klocale.h>

void BogusImp::draw( KigPainter& ) const
{
}

bool BogusImp::contains( const Coordinate&, int, const ScreenInfo& ) const
{
  return false;
}

bool BogusImp::inRect( const Rect& ) const
{
  return false;
}

bool BogusImp::valid() const
{
  return true;
}

const uint BogusImp::numberOfProperties() const
{
  return 0;
}

const QCStringList BogusImp::properties() const
{
  return QCStringList();
}

ObjectImp* BogusImp::property( uint, const KigDocument& ) const
{
  assert( false );
  return new InvalidImp;
}

DoubleImp::DoubleImp( const double d )
  : mdata( d )
{
}

IntImp::IntImp( const int d )
  : mdata( d )
{
}

StringImp::StringImp( const QString& d )
  : mdata( d )
{
}

DoubleImp* DoubleImp::copy() const
{
  return new DoubleImp( mdata );
}

IntImp* IntImp::copy() const
{
  return new IntImp( mdata );
}

StringImp* StringImp::copy() const
{
  return new StringImp( mdata );
}

bool DoubleImp::inherits( int typeID ) const
{
  return typeID == ID_DoubleImp ? true : Parent::inherits( typeID );
}

bool IntImp::inherits( int typeID ) const
{
  return typeID == ID_IntImp ? true : Parent::inherits( typeID );
}

bool StringImp::inherits( int typeID ) const
{
  return typeID == ID_StringImp ? true : Parent::inherits( typeID );
}

ObjectImp* BogusImp::transform( const Transformation& ) const
{
  return copy();
}

bool InvalidImp::valid() const
{
  return false;
}

InvalidImp* InvalidImp::copy() const
{
  return new InvalidImp();
}

bool InvalidImp::inherits( int t ) const
{
  return Parent::inherits( t );
}

InvalidImp::InvalidImp()
{
}

const char* InvalidImp::baseName() const
{
  return "invalid";
}

const char* DoubleImp::baseName() const
{
  return I18N_NOOP( "floating point number" );
}

const char* IntImp::baseName() const
{
  return I18N_NOOP( "integral number" );
}

const char* StringImp::baseName() const
{
  return I18N_NOOP( "string" );
}

int InvalidImp::id() const
{
  return ID_InvalidImp;
}

int DoubleImp::id() const
{
  return ID_DoubleImp;
}

int IntImp::id() const
{
  return ID_IntImp;
}

int StringImp::id() const
{
  return ID_StringImp;
}

const QCStringList BogusImp::propertiesInternalNames() const
{
  return QCStringList();
}
