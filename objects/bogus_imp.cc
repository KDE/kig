// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "bogus_imp.h"

#include <QStringList>

#include "../misc/rect.h"

Coordinate BogusImp::attachPoint( ) const
{
  return Coordinate::invalidCoord();
}

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

ObjectImp* BogusImp::transform( const Transformation& ) const
{
  return copy();
}

InvalidImp* InvalidImp::copy() const
{
  return new InvalidImp();
}

InvalidImp::InvalidImp()
{
}

void InvalidImp::fillInNextEscape( QString& s, const KigDocument& ) const
{
  s = s.arg( QStringLiteral("[invalid]") );
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

HierarchyImp* HierarchyImp::copy() const
{
  return new HierarchyImp( mdata );
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

TransformationImp* TransformationImp::copy() const
{
  return new TransformationImp( mdata );
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
  return rhs.inherits( DoubleImp::stype() ) &&
    static_cast<const DoubleImp&>( rhs ).data() == mdata;
}

bool IntImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( IntImp::stype() ) &&
    static_cast<const IntImp&>( rhs ).data() == mdata;
}

bool StringImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( StringImp::stype() ) &&
    static_cast<const StringImp&>( rhs ).data() == mdata;
}

bool HierarchyImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( HierarchyImp::stype() ) &&
    static_cast<const HierarchyImp&>( rhs ).data() == mdata;
}

bool TransformationImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( TransformationImp::stype() ) &&
    static_cast<const TransformationImp&>( rhs ).data() == mdata;
}

bool InvalidImp::canFillInNextEscape() const
{
  return true;
}

bool DoubleImp::canFillInNextEscape() const
{
  return true;
}

bool IntImp::canFillInNextEscape() const
{
  return true;
}

bool StringImp::canFillInNextEscape() const
{
  return true;
}

const ObjectImpType* InvalidImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "invalid", "", "", "", "", "", "", "", "", "" );
  return &t;
}

const ObjectImpType* StringImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "string",
    "string", "", "", "", "", "", "", "", "" );
  return &t;
}
const ObjectImpType* HierarchyImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "hierarchy", "", "", "", "", "", "", "", "", "" );
  return &t;
}
const ObjectImpType* TransformationImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "transformation", "", "", "", "", "", "", "", "", "");
  return &t;
}

const ObjectImpType* InvalidImp::type() const
{
  return InvalidImp::stype();
}

const ObjectImpType* DoubleImp::type() const
{
  return DoubleImp::stype();
}

const ObjectImpType* IntImp::type() const
{
  return IntImp::stype();
}

const ObjectImpType* StringImp::type() const
{
  return StringImp::stype();
}

const ObjectImpType* HierarchyImp::type() const
{
  return HierarchyImp::stype();
}

const ObjectImpType* TransformationImp::type() const
{
  return TransformationImp::stype();
}

const ObjectImpType* DoubleImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "double",
    "double", "", "", "", "", "", "", "", "" );
  return &t;
}

const ObjectImpType* IntImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "int",
    "int", "", "", "", "", "", "", "", "" );
  return &t;
}

const ObjectImpType* BogusImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "bogus",
    "", "", "", "", "", "", "", "", "" );
  return &t;
}

const ObjectImpType* TestResultImp::stype()
{
  static const ObjectImpType t(
    Parent::stype(), "testresult", "", "", "", "", "", "", "", "", "" );
  return &t;

}

TestResultImp::TestResultImp( bool t, const QString& s )
  : StringImp( s ), mtruth( t )
{
}

TestResultImp* TestResultImp::copy() const
{
  return new TestResultImp( mtruth, data() );
}

const ObjectImpType* TestResultImp::type() const
{
  return stype();
}

void TestResultImp::visit( ObjectImpVisitor* vtor ) const
{
  vtor->visit( this );
}

bool TestResultImp::equals( const ObjectImp& rhs ) const
{
  return rhs.inherits( TestResultImp::stype() ) &&
// (FIXME?)    static_cast<const TestResultImp&>( rhs ).mdata == mdata &&
    static_cast<const TestResultImp&>( rhs ).mtruth == mtruth;

}

int TestResultImp::numberOfProperties() const
{
  return Parent::numberOfProperties() + 1;
}

const QByteArrayList TestResultImp::properties() const
{
  QByteArrayList l = Parent::properties();
  l << I18N_NOOP( "Test Result" );
  assert( l.size() == TestResultImp::numberOfProperties() );
  return l;
}

const QByteArrayList TestResultImp::propertiesInternalNames() const
{
  QByteArrayList s = Parent::propertiesInternalNames();
  s << "test-result";
  assert( s.size() == TestResultImp::numberOfProperties() );
  return s;
}

ObjectImp* TestResultImp::property( int which, const KigDocument& d ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::property( which, d );
  if ( which == Parent::numberOfProperties() )
    return new StringImp( data() );
  else assert( false );
  return new InvalidImp;
}

const char* TestResultImp::iconForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::iconForProperty( which );
  if ( which == Parent::numberOfProperties() )
    return ""; // test-result
  else assert( false );
  return "";
}

const ObjectImpType* TestResultImp::impRequirementForProperty( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return TestResultImp::stype();
}

bool TestResultImp::isPropertyDefinedOnOrThroughThisImp( int which ) const
{
  if ( which < Parent::numberOfProperties() )
    return Parent::impRequirementForProperty( which );
  else return false;
}

Rect BogusImp::surroundingRect() const
{
  return Rect::invalidRect();
}
