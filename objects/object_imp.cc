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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "object_imp.h"

#include "bogus_imp.h"

#include "../misc/coordinate.h"

#include <klocale.h>
#include <map>

class ObjectImpType::StaticPrivate
{
public:
  std::map<QCString, const ObjectImpType*> namemap;
};

ObjectImp::ObjectImp()
{
}

ObjectImp::~ObjectImp()
{
}

bool ObjectImp::valid() const
{
  return ! type()->inherits( InvalidImp::stype() );
}

void ObjectImp::fillInNextEscape( QString&, const KigDocument& ) const
{
  assert( false );
}

const QCStringList ObjectImp::properties() const
{
  QCStringList ret;
  ret << I18N_NOOP( "Object Type" );
  return ret;
}

const uint ObjectImp::numberOfProperties() const
{
  return 1;
}

const QCStringList ObjectImp::propertiesInternalNames() const
{
  QCStringList ret;
  ret << "base-object-type";
  return ret;
}

ObjectImp* ObjectImp::property( uint i, const KigDocument& ) const
{
  if ( i == 0 ) return new StringImp( type()->translatedName() );
  return new InvalidImp;
}

const ObjectImpType* ObjectImp::impRequirementForProperty( uint ) const
{
  return ObjectImp::stype();
}

void ObjectImpVisitor::visit( const ObjectImp* imp )
{
  imp->visit( this );
}

void ObjectImpVisitor::visit( const IntImp* )
{
}

void ObjectImpVisitor::visit( const DoubleImp* )
{
}

void ObjectImpVisitor::visit( const StringImp* )
{
}

void ObjectImpVisitor::visit( const InvalidImp* )
{
}

void ObjectImpVisitor::visit( const HierarchyImp* )
{
}

void ObjectImpVisitor::visit( const LineImp* )
{
}

void ObjectImpVisitor::visit( const PointImp* )
{
}

void ObjectImpVisitor::visit( const TextImp* )
{
}

void ObjectImpVisitor::visit( const AngleImp* )
{
}

void ObjectImpVisitor::visit( const VectorImp* )
{
}

void ObjectImpVisitor::visit( const LocusImp* )
{
}

void ObjectImpVisitor::visit( const CircleImp* )
{
}

void ObjectImpVisitor::visit( const ConicImp* )
{
}

void ObjectImpVisitor::visit( const CubicImp* )
{
}

void ObjectImpVisitor::visit( const SegmentImp* )
{
}

void ObjectImpVisitor::visit( const RayImp* )
{
}

void ObjectImpVisitor::visit( const ArcImp* )
{
}

void ObjectImpVisitor::visit( const PolygonImp* )
{
}

ObjectImpVisitor::~ObjectImpVisitor()
{

}

void ObjectImpVisitor::visit( const TransformationImp* )
{
}

void ObjectImpVisitor::visit( const TestResultImp* )
{
}

const char* ObjectImp::iconForProperty( uint ) const
{
  return "kig_text";
}

bool ObjectImp::canFillInNextEscape() const
{
  return false;
}

ObjectImpType::ObjectImpType( const ObjectImpType* parent,
                              const char* internalname,
                              const char* translatedname,
                              const char* selectstatement,
                              const char* selectnamestatement,
                              const char* removeastatement,
                              const char* addastatement,
                              const char* moveastatement,
                              const char* attachtothisstatement,
                              const char* showastatement,
                              const char* hideastatement )
  : mparent( parent ), minternalname( internalname ),
    mtranslatedname( translatedname ), mselectstatement( selectstatement ),
    mselectnamestatement( selectnamestatement ),
    mremoveastatement( removeastatement ), maddastatement( addastatement ),
    mmoveastatement( moveastatement ),
    mattachtothisstatement( attachtothisstatement ),
    mshowastatement( showastatement ),
    mhideastatement( hideastatement )
{
  sd()->namemap[minternalname] = this;
}

ObjectImpType::~ObjectImpType()
{
}

bool ObjectImpType::inherits( const ObjectImpType* t ) const
{
  return t == this || (mparent && mparent->inherits( t ) );
}

const char* ObjectImpType::internalName() const
{
  return minternalname;
}

QString ObjectImpType::translatedName() const
{
  return i18n( mtranslatedname );
}

const char* ObjectImpType::selectStatement() const
{
  return mselectstatement;
}

const char* ObjectImpType::selectNameStatement() const
{
  return mselectnamestatement;
}

QString ObjectImpType::removeAStatement() const
{
  return i18n( mremoveastatement );
}

QString ObjectImpType::addAStatement() const
{
  return i18n( maddastatement );
}

QString ObjectImpType::moveAStatement() const
{
  return i18n( mmoveastatement );
}

const ObjectImpType* ObjectImpType::typeFromInternalName( const char* string )
{
  QCString s( string );
  std::map<QCString, const ObjectImpType*>::iterator i = sd()->namemap.find( s );
  if ( i == sd()->namemap.end() )
    return 0;
  else return i->second;
}

bool ObjectImp::inherits( const ObjectImpType* t ) const
{
  return type()->inherits( t );
}

const ObjectImpType* ObjectImp::stype()
{
  static const ObjectImpType t(
    0, "any",
    I18N_NOOP( "Object" ),
    I18N_NOOP( "Select this object" ),
    I18N_NOOP( "Select object %1" ),
    I18N_NOOP( "Remove an object" ),
    I18N_NOOP( "Add an object" ),
    I18N_NOOP( "Move an object" ),
    I18N_NOOP( "Attach to this object" ),
    I18N_NOOP( "Show an object" ),
    I18N_NOOP( "Hide an object" ) );
  return &t;
}

ObjectImpType::StaticPrivate* ObjectImpType::sd()
{
  static StaticPrivate d;
  return &d;
}

bool ObjectImp::isCache() const
{
  return false;
}

QString ObjectImpType::attachToThisStatement() const
{
  return i18n( mattachtothisstatement );
}

QString ObjectImpType::showAStatement() const
{
  return i18n( mshowastatement );
}

QString ObjectImpType::hideAStatement() const
{
  return i18n( mhideastatement );
}

bool ObjectImp::isPropertyDefinedOnOrThroughThisImp( uint ) const
{
  return false;
}

