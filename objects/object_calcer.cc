// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "object_calcer.h"

#include "object_holder.h"
#include "object_imp.h"
#include "bogus_imp.h"
#include "object_type.h"
#include "../misc/coordinate.h"
#include "common.h"

#include <algorithm>
#include <set>
#include <typeinfo>
#include <iterator>

void ObjectTypeCalcer::calc( const KigDocument& doc )
{
  Args a;
  a.reserve( mparents.size() );
  std::transform( mparents.begin(), mparents.end(),
                  std::back_inserter( a ), std::mem_fun( &ObjectCalcer::imp ) );
  ObjectImp* n = mtype->calc( a, doc );
  delete mimp;
  mimp = n;
}

ObjectTypeCalcer::ObjectTypeCalcer( const ObjectType* type, 
      const std::vector<ObjectCalcer*>& parents, bool sort )
  : mparents( ( sort )?type->sortArgs( parents ):parents ), mtype( type ), mimp( 0 )
{
  std::for_each( mparents.begin(), mparents.end(),
                 std::bind2nd( std::mem_fun( &ObjectCalcer::addChild ), this ) );
}

ObjectCalcer::~ObjectCalcer()
{
}

ObjectConstCalcer::ObjectConstCalcer( ObjectImp* imp )
  : mimp( imp )
{
}

ObjectConstCalcer::~ObjectConstCalcer()
{
  delete mimp;
}

const ObjectImp* ObjectConstCalcer::imp() const
{
  return mimp;
}

void ObjectConstCalcer::calc( const KigDocument& )
{
}

std::vector<ObjectCalcer*> ObjectConstCalcer::parents() const
{
  // we have no parents..
  return std::vector<ObjectCalcer*>();
}

void ObjectCalcer::ref()
{
  ++refcount;
}

void ObjectCalcer::deref()
{
  if ( --refcount <= 0 ) delete this;
}

void intrusive_ptr_add_ref( ObjectCalcer* p )
{
  p->ref();
}

void intrusive_ptr_release( ObjectCalcer* p )
{
  p->deref();
}

const ObjectImp* ObjectTypeCalcer::imp() const
{
  return mimp;
}

std::vector<ObjectCalcer*> ObjectTypeCalcer::parents() const
{
  return mparents;
}

void ObjectCalcer::addChild( ObjectCalcer* c )
{
  mchildren.push_back( c );
  ref();
}

void ObjectCalcer::delChild( ObjectCalcer* c )
{
  std::vector<ObjectCalcer*>::iterator i = std::find( mchildren.begin(), mchildren.end(), c );
  assert( i != mchildren.end() );

  mchildren.erase( i );
  deref();
}

ObjectTypeCalcer::~ObjectTypeCalcer()
{
  std::for_each( mparents.begin(), mparents.end(),
                 std::bind2nd( std::mem_fun( &ObjectCalcer::delChild ), this ) );
  delete mimp;
}

const ObjectType* ObjectTypeCalcer::type() const
{
  return mtype;
}

ObjectPropertyCalcer::ObjectPropertyCalcer( ObjectCalcer* parent, const char* pname )
  : mimp( 0 ), mparent( parent ), mparenttype( 0 )
{
  mparent->addChild( this );
  mpropgid = mparent->imp()->getPropGid( pname );
}

ObjectPropertyCalcer::ObjectPropertyCalcer( ObjectCalcer* parent, int propid, bool islocal )
  : mimp( 0 ), mparent( parent ), mparenttype( 0 )
{
  mparent->addChild( this );
  if ( islocal )
  {
    mpropgid = parent->imp()->getPropGid( parent->imp()->propertiesInternalNames()[propid] );
  } else {
    mpropgid = propid;
  }
}

ObjectPropertyCalcer::~ObjectPropertyCalcer()
{
  mparent->delChild( this );
  delete mimp;
}

const ObjectImp* ObjectPropertyCalcer::imp() const
{
  return mimp;
}

std::vector<ObjectCalcer*> ObjectPropertyCalcer::parents() const
{
  std::vector<ObjectCalcer*> ret;
  ret.push_back( mparent );
  return ret;
}

void ObjectPropertyCalcer::calc( const KigDocument& doc )
{
  //if ( mparenttype != mparent->imp()->type() )
  if ( mparenttype == 0 || *mparenttype != typeid( *(mparent->imp()) ) )
  {
    mpropid = mparent->imp()->getPropLid( mpropgid );
//    mparenttype = mparent->imp()->type();
    mparenttype = &typeid( *(mparent->imp()) );
//    printf ("changing type, new type: %s\n", mparenttype->internalName());
  }
  ObjectImp* n;
  if ( mpropid >= 0 )
  {
    n = mparent->imp()->property( mpropid, doc );
  } else n = new InvalidImp;
  delete mimp;
  mimp = n;
}

ObjectImp* ObjectConstCalcer::switchImp( ObjectImp* newimp )
{
  ObjectImp* ret = mimp;
  mimp = newimp;
  return ret;
}

std::vector<ObjectCalcer*> ObjectCalcer::children() const
{
  return mchildren;
}

const ObjectImpType* ObjectPropertyCalcer::impRequirement(
  ObjectCalcer*, const std::vector<ObjectCalcer*>& ) const
{
  int proplid = mparent->imp()->getPropLid( mpropgid );
  return mparent->imp()->impRequirementForProperty( proplid );
}

const ObjectImpType* ObjectConstCalcer::impRequirement(
  ObjectCalcer*, const std::vector<ObjectCalcer*>& ) const
{
  assert( false );
  return ObjectImp::stype();
}

const ObjectImpType* ObjectTypeCalcer::impRequirement(
 ObjectCalcer* o, const std::vector<ObjectCalcer*>& os ) const
{
  Args args;
  args.reserve( mparents.size() );
  std::transform(
    os.begin(), os.end(),
    std::back_inserter( args ),
    std::mem_fun( &ObjectCalcer::imp ) );
  assert( std::find( args.begin(), args.end(), o->imp() ) != args.end() );
  return mtype->impRequirement( o->imp(), args );
}

void ObjectConstCalcer::setImp( ObjectImp* newimp )
{
  delete switchImp( newimp );
}

void ObjectTypeCalcer::setParents( const std::vector<ObjectCalcer*> &np )
{
  std::for_each( np.begin(), np.end(),
                 std::bind2nd( std::mem_fun( &ObjectCalcer::addChild ), this ) );
  std::for_each( mparents.begin(), mparents.end(),
                 std::bind2nd( std::mem_fun( &ObjectCalcer::delChild ), this ) );
  mparents = np;
}

void ObjectTypeCalcer::setType( const ObjectType* t )
{
  mtype = t;
}

bool ObjectCalcer::canMove() const
{
  return false;
}

bool ObjectCalcer::isFreelyTranslatable() const
{
  return false;
}

Coordinate ObjectCalcer::moveReferencePoint() const
{
  assert( false );
  return Coordinate::invalidCoord();
}

void ObjectCalcer::move( const Coordinate&, const KigDocument& )
{
  assert( false );
}

bool ObjectTypeCalcer::canMove() const
{
  return mtype->canMove( *this );
}

bool ObjectTypeCalcer::isFreelyTranslatable() const
{
  return mtype->isFreelyTranslatable( *this );
}

Coordinate ObjectTypeCalcer::moveReferencePoint() const
{
  return mtype->moveReferencePoint( *this );
}

void ObjectTypeCalcer::move( const Coordinate& to, const KigDocument& doc )
{
  // we need to check if type can in fact move, because this check is
  // not done for us in all circumstances ( e.g. LineABType::move uses
  // move on its parents to move them ), and the ObjectType's depend
  // on move only being called if canMove() returns true..
  if ( mtype->canMove( *this ) )
    mtype->move( *this, to, doc );
}

ObjectCalcer* ObjectPropertyCalcer::parent() const
{
  return mparent;
}

ObjectCalcer::ObjectCalcer()
  : refcount( 0 )
{
}

std::vector<ObjectCalcer*> ObjectCalcer::movableParents() const
{
  return std::vector<ObjectCalcer*>();
}

std::vector<ObjectCalcer*> ObjectTypeCalcer::movableParents() const
{
  return mtype->movableParents( *this );
}

bool ObjectConstCalcer::isDefinedOnOrThrough( const ObjectCalcer* ) const
{
  return false;
}

bool ObjectPropertyCalcer::isDefinedOnOrThrough( const ObjectCalcer* o ) const
{
  return o == mparent &&
    mparent->imp()->isPropertyDefinedOnOrThroughThisImp( 
       mparent->imp()->getPropLid( mpropgid ) );
}

bool ObjectTypeCalcer::isDefinedOnOrThrough( const ObjectCalcer* o ) const
{
  Args args;
  args.reserve( mparents.size() );
  std::transform(
    mparents.begin(), mparents.end(),
    std::back_inserter( args ),
    std::mem_fun( &ObjectCalcer::imp ) );
  if ( std::find( args.begin(), args.end(), o->imp() ) == args.end() )
    return false;

  return mtype->isDefinedOnOrThrough( o->imp(), args );
}

int ObjectPropertyCalcer::propLid() const
{
  return mparent->imp()->getPropLid( mpropgid );
}

int ObjectPropertyCalcer::propGid() const
{
  return mpropgid;
}

