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

bool BogusImp::contains( const Coordinate&, int, const KigWidget& ) const
{
  return false;
}

bool BogusImp::inRect( const Rect&, int, const KigWidget& ) const
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

int BogusImp::impRequirementForProperty( uint which ) const
{
  return Parent::impRequirementForProperty( which );
}

ObjectImp* BogusImp::property( uint, const KigDocument& ) const
{
  return new InvalidImp;
}

const char* BogusImp::iconForProperty( uint ) const
{
  assert( false );
  return "";
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

void InvalidImp::fillInNextEscape( QString& s, const KigDocument& ) const
{
  s = s.arg( "[invalid]" );
}

void DoubleImp::fillInNextEscape( QString& s, const KigDocument& ) const
{
  s = s.arg( mdata );
}

void IntImp::fillInNextEscape( QString& s, const KigDocument& ) const
{
  s = s.arg( mdata );
}

void StringImp::fillInNextEscape( QString& s, const KigDocument& ) const
{
  s = s.arg( mdata );
}

HierarchyImp::HierarchyImp( const ObjectHierarchy& h )
  : BogusImp(), mdata( h )
{
}

bool HierarchyImp::inherits( int type ) const
{
  return type == ID_HierarchyImp ? true : Parent::inherits( type );
}

HierarchyImp* HierarchyImp::copy() const
{
  return new HierarchyImp( mdata );
}

const char* HierarchyImp::baseName() const
{
  return I18N_NOOP( "Hierarchy" );
}

int HierarchyImp::id() const
{
  return ID_HierarchyImp;
}

void InvalidImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

void DoubleImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

void IntImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

void StringImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

void HierarchyImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

TransformationImp::TransformationImp( const Transformation& h )
  : mdata( h )
{
}

bool TransformationImp::inherits( int type ) const
{
  return type == ID_TransformationImp ? true : Parent::inherits( type );
}

TransformationImp* TransformationImp::copy() const
{
  return new TransformationImp( mdata );
}

const char* TransformationImp::baseName() const
{
  return I18N_NOOP( "transformation" );
}

int TransformationImp::id() const
{
  return ID_TransformationImp;
}

void TransformationImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool InvalidImp::equals( const ObjectImp& rhs ) const
{
  return !rhs.valid();
}

bool DoubleImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( ID_DoubleImp ) &&
    static_cast<const DoubleImp&>( rhs ).data() == mdata;
}

bool IntImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( ID_IntImp ) &&
    static_cast<const IntImp&>( rhs ).data() == mdata;
}

bool StringImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( ID_StringImp ) &&
    static_cast<const StringImp&>( rhs ).data() == mdata;
}

bool HierarchyImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( ID_HierarchyImp ) &&
    static_cast<const HierarchyImp&>( rhs ).data() == mdata;
}

bool TransformationImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( ID_TransformationImp ) &&
    static_cast<const TransformationImp&>( rhs ).data() == mdata;
}

