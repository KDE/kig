// nobject.cc
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

#include "object.h"

#include "object_type.h"
#include "object_imp.h"
#include "curve_imp.h"
#include "property.h"
#include "../misc/kigpainter.h"

#include <qpen.h>
#include <functional>
#include <algorithm>

RealObject::RealObject( const ObjectType* type, const Objects& parents )
  : ObjectWithParents( parents ),
    mcolor( Qt::blue ), mselected( false ), mshown( true ), mwidth( 1 ), mtype( type ),
    mimp( 0 )
{
}

RealObject::~RealObject()
{
  delete mimp;
}

void RealObject::draw( KigPainter& p, bool ss ) const
{
  if ( ! mshown ) return;
  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( mselected && ss ? Qt::red : mcolor );
  p.setPen( QPen ( mselected && ss ? Qt::red : mcolor,  1) );
  p.setWidth( mwidth );
  mimp->draw( p );
}

bool RealObject::contains( const Coordinate& p, const ScreenInfo& si ) const
{
  return mimp->contains( p, si );
}

bool RealObject::inRect( const Rect& r ) const
{
  return mimp->inRect( r );
}

void RealObject::move( const Coordinate& from, const Coordinate& dist )
{
  mtype->move( this, from, dist );
}

void ObjectWithParents::calc()
{
  using namespace std;
  Args a;
  a.reserve( mparents.size() );
  transform( mparents.begin(), mparents.end(),
             back_inserter( a ), mem_fun( &Object::imp ) );
  calc( a );
}

void RealObject::reset( const ObjectType* t, const Objects& parents )
{
  mtype = t;
  setParents( parents );
}

QCString RealObject::baseTypeName() const
{
  return mimp->baseName();
}

QString RealObject::translatedBaseTypeName() const
{
  return mimp->translatedBaseName();
}

void RealObject::setImp( ObjectImp* i )
{
  delete mimp;
  mimp = i;
}

bool RealObject::canMove() const
{
  return mtype->canMove();
}

Object::~Object()
{
  // tell our children that we're dead, so they don't try to tell us
  // that they're dying too, which would cause segfaults...
  for ( uint i = 0; i < mchildren.size(); ++i )
    mchildren[i]->delParent( this );
}

bool Object::hasimp( int type ) const
{
  return imp()->inherits( type );
}

const uint Object::numberOfProperties() const
{
  return imp()->numberOfProperties();
}

const Property Object::property( uint which, const KigWidget& w ) const
{
  return imp()->property( which, w );
}

const QCStringList Object::properties() const
{
  return imp()->properties();
}

const Objects& Object::children() const
{
  return mchildren;
}

const Objects Object::getAllChildren() const
{
  Objects ret;
  // objects to iterate over...
  Objects objs = mchildren;
  // contains the objects to iterate over the next time around...
  Objects objs2;
  while( !objs.empty() )
  {
    for( Objects::iterator i = objs.begin();
         i != objs.end(); ++i )
    {
      ret.upush( *i );
      objs2 |= (*i)->children();
    };
    objs = objs2;
    objs2.clear();
  };
  return ret;
}

void Object::addChild( Object* o )
{
  mchildren.upush( o );
  childAdded();
}

void Object::delChild( Object* o )
{
  mchildren.remove( o );
  childRemoved();
}

void ObjectWithParents::addParent( Object* o )
{
  mparents.upush( o );
}

void ObjectWithParents::delParent( Object* o )
{
  mparents.remove( o );
}

void ObjectWithParents::setParents( const Objects& parents )
{
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->delChild( this );
  mparents = parents;
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->addChild( this );
}

Objects ObjectWithParents::parents() const
{
  return mparents;
}

ObjectWithParents::~ObjectWithParents()
{
  // tell our parents that we're dead...
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->delChild( this );
}

const ObjectImp* RealObject::imp() const
{
  return mimp;
}

const ObjectType* RealObject::type() const
{
  return mtype;
}

void RealObject::calc( const Args& a )
{
  delete mimp;
  mimp = mtype->calc( a );
}

Object::Object()
{
}

ObjectWithParents::ObjectWithParents( const Objects& parents )
  : mparents( parents )
{
  for ( uint i = 0; i < mparents.size(); ++i )
    mparents[i]->addChild( this );
}

DataObject::DataObject( ObjectImp* imp )
  : mimp( imp ), mrefs( 0 )
{
}

DataObject::~DataObject()
{
  delete mimp;
}

const ObjectImp* DataObject::imp() const
{
  return mimp;
}

Objects DataObject::parents() const
{
  return Objects();
}

void DataObject::draw( KigPainter&, bool ) const
{
}

bool DataObject::contains( const Coordinate&, const ScreenInfo& ) const
{
  return false;
}

bool DataObject::inRect( const Rect& ) const
{
  return false;
}

bool DataObject::canMove() const
{
  return false;
}

void DataObject::move( const Coordinate&, const Coordinate& )
{
//    assert( false );
}

void DataObject::calc()
{
}

QCString DataObject::baseTypeName() const
{
  return "SHOULDNOTBESEEN";
}

QString DataObject::translatedBaseTypeName() const
{
  return QString::fromUtf8( "SHOULDNOTBESEEN" );
}

void DataObject::childAdded()
{
  ++mrefs;
}

void DataObject::childRemoved()
{
  if ( --mrefs <= 0 ) delete this;
}

void Object::childRemoved()
{
}

void Object::childAdded()
{
}

void Object::addParent( Object* )
{
  assert( false );
}

void Object::delParent( Object* )
{
  assert( false );
}

void Object::setParents( const Objects& )
{
  assert( false );
}

bool Object::inherits( int ) const
{
  return false;
}

bool RealObject::inherits( int type ) const
{
  return type == ID_RealObject;
}

void RealObject::calc()
{
  // no idea why this is necessary
  ObjectWithParents::calc();
}

void DataObject::setImp( ObjectImp* imp )
{
  delete mimp;
  mimp = imp;
}

bool DataObject::inherits( int type ) const
{
  return type == ID_DataObject;
}

bool Object::valid() const
{
  return imp()->valid();
}

bool RealObject::shown() const
{
  return mshown;
}

bool DataObject::shown() const
{
  return false;
}
