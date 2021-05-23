// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "object_imp.h"

#include "bogus_imp.h"

#include "../misc/coordinate.h"

#include <map>

class ObjectImpType::StaticPrivate
{
public:
  std::map<QByteArray, const ObjectImpType*> namemap;
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

const QByteArrayList ObjectImp::properties() const
{
  QByteArrayList ret;
  ret << I18N_NOOP( "Object Type" );
  return ret;
}

int ObjectImp::numberOfProperties() const
{
  return 1;
}

const QByteArrayList ObjectImp::propertiesInternalNames() const
{
  QByteArrayList ret;
  ret << "base-object-type";
  return ret;
}

ObjectImp* ObjectImp::property( int i, const KigDocument& ) const
{
  if ( i == 0 ) return new StringImp( type()->translatedName() );
  return new InvalidImp;
}

const ObjectImpType* ObjectImp::impRequirementForProperty( int ) const
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

void ObjectImpVisitor::visit( const FilledPolygonImp* )
{
}

void ObjectImpVisitor::visit( const ClosedPolygonalImp* )
{
}

void ObjectImpVisitor::visit( const OpenPolygonalImp* )
{
}

void ObjectImpVisitor::visit( const BezierImp* )
{
}

void ObjectImpVisitor::visit( const RationalBezierImp* )
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

const char* ObjectImp::iconForProperty( int ) const
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
//  return t == this || (mparent && mparent->inherits( t ) );
  return t->match( this ) || (mparent && mparent->inherits( t ) );
}

bool ObjectImpType::match( const ObjectImpType* t ) const
{
  return t == this;
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
  QByteArray s( string );
  std::map<QByteArray, const ObjectImpType*>::iterator i = sd()->namemap.find( s );
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

bool ObjectImp::isPropertyDefinedOnOrThroughThisImp( int ) const
{
  return false;
}

static QByteArrayList propertiesGlobalInternalNames;

int ObjectImp::getPropGid( const char* pname ) const
{
  int wp = propertiesGlobalInternalNames.indexOf( pname );
  if ( wp >= 0 ) return wp;

  int lp = propertiesInternalNames().indexOf( pname );
  if ( lp < 0 ) return lp; // insist that this exists as a property

  propertiesGlobalInternalNames << pname;
  wp = propertiesGlobalInternalNames.indexOf( pname );
  assert( wp >= 0 );
  return wp;
}

int ObjectImp::getPropLid( int propgid ) const
{
  assert( propgid >= 0 && propgid < propertiesGlobalInternalNames.size() );
  int proplid = propertiesInternalNames().indexOf(
              propertiesGlobalInternalNames[propgid] );
//  printf ("getPropLid: converting %d in %d\n", propgid, proplid);
  return proplid;
}

const char* ObjectImp::getPropName( int propgid ) const
{
  assert( propgid >= 0 && propgid < propertiesGlobalInternalNames.size() );
  return propertiesGlobalInternalNames[propgid];
}
