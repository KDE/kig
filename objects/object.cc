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

Object::Object( ObjectType* type, const Objects& parents, const Args& fixedArgs )
  : mcolor( Qt::blue ), mselected( false ), mshown( true ), mwidth( 1 ), mtype( type ),
    mimp( 0 ), mparents( parents ), mfixedargs( fixedArgs )
{
  for ( Objects::iterator i = mparents.begin(); i != mparents.end(); ++i )
    (*i)->addChild( this );
}

Object::~Object()
{
  // tell our parents that we're dead...
  for ( Objects::iterator i = mparents.begin(); i != mparents.end(); ++i )
    (*i)->delChild( this );
  // tell our children that we're dead, so they don't try to tell us
  // that they're dying too, which would cause segfaults...
  for ( Objects::iterator i = mchildren.begin(); i != mchildren.end(); ++i )
    (*i)->delParent( this );

  delete mtype;
}

const uint Object::numberOfProperties() const
{
  return mimp->numberOfProperties();
}

const Property Object::property( uint which, const KigWidget& w ) const
{
  return mimp->property( which, w );
}

const QCStringList Object::properties() const
{
  return mimp->properties();
}

// prop_map Object::saveImp()
// {
//   prop_map ret;
//   ret["object-color"] = mColor.name();
//   ret["object-shown"] = QString::fromLatin1( mshown ? "true" : "false" );
//   return ret;
// }

// void Object::loadImp( const prop_map& m )
// {
//   mtype->loadImp( m, *mimp );
// }

void Object::draw( KigPainter& p, bool ss ) const
{
  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( mselected && ss ? Qt::red : mcolor );
  p.setPen( QPen ( mselected && ss ? Qt::red : mcolor,  1) );
  p.setWidth( mwidth );
  mimp->draw( p );
}

bool Object::contains( const Coordinate& p, const ScreenInfo& si ) const
{
  return mimp->contains( p, si );
}

bool Object::inRect( const Rect& r ) const
{
  return mimp->inRect( r );
}

void Object::move( const Coordinate& from, const Coordinate& dist )
{
  mtype->move( this, from, dist );
}

void Object::calc( const KigWidget& w )
{
  using namespace std;
  Args a = mfixedargs;
  transform( mparents.begin(), mparents.end(),
             back_inserter( a ), mem_fun( &Object::imp ) );
  mimp.reset( mtype->calc( a, w ) );
}

// void Object::addActions( NormalModePopupObjects& m )
// {
// }

// void Object::doNormalAction( int which, KigDocument& d, KigWidget& w, NormalMode& m, const Coordinate& cp )
// {
// }

// void Object::doPopupAction( int popupid, int actionid, KigDocument* d, KigWidget* w, NormalMode* m, const Coordinate& cp )
// {
// }

bool Object::valid() const
{
  return mimp->valid();
}

Objects Object::getAllChildren() const
{
  // what we are going to return...
  Objects tmp;
  // objects to iterate over...
  Objects objs = mchildren;
  // contains the objects to iterate over the next time around...
  Objects objs2;
  while( !objs.empty() )
  {
    for( Objects::iterator i = objs.begin();
         i != objs.end(); ++i )
    {
      tmp.upush( *i );
      objs2 |= (*i)->children();
    };
    objs = objs2;
    objs2.clear();
  };
  return tmp;
}

Objects Object::parents() const
{
  return mparents;
}

void Object::addChild( Object* o )
{
  mchildren.upush( o );
}

void Object::delChild( Object* o )
{
  mchildren.remove( o );
}

bool Object::has( int typeID ) const
{
  return mimp->inherits( typeID );
}

void Object::reset( ObjectType* t, const Args& fixedArgs, const Objects& parents )
{
  delete mtype;
  mtype = t;

  for ( Args::const_iterator i = mfixedargs.begin(); i != mfixedargs.end(); ++i )
    delete *i;
  mfixedargs = fixedArgs;

  for ( Objects::iterator i = mparents.begin(); i != mparents.end(); ++i )
    (*i)->delChild( this );
  mparents = parents;
  for ( Objects::iterator i = mparents.begin(); i != mparents.end(); ++i )
    (*i)->addChild( this );
}

double Object::getParam( const Coordinate& c ) const
{
  assert( mimp->inherits( ObjectImp::ID_CurveImp ) );
  return static_cast<const CurveImp*>( mimp.get() )->getParam( c );
}

const Coordinate Object::getPoint( double param ) const
{
  assert( mimp->inherits( ObjectImp::ID_CurveImp ) );
  return static_cast<const CurveImp*>( mimp.get() )->getPoint( param );
}

QCString Object::baseTypeName() const
{
  return mtype->baseName();
}

QString Object::translatedBaseTypeName() const
{
  return mtype->translatedBaseName();
}

Object::Object( const Object& o )
  : mcolor( o.mcolor ), mselected( o.mselected ), mshown( o.mshown ),
    mwidth( 1 ), mtype( o.mtype->copy() ), mimp( o.mimp->copy() ), mparents( o.mparents )
{
  for ( Objects::const_iterator i = mparents.begin(); i != mparents.end(); ++i )
    (*i)->addChild( this );
  for ( Args::const_iterator i = o.mfixedargs.begin(); i != o.mfixedargs.end(); ++i )
    mfixedargs.push_back( (*i)->copy() );
}

void Object::setImp( ObjectImp* i )
{
  mimp.reset( i );
}

bool Object::canMove() const
{
  return mtype->canMove();
}

void Object::delParent( Object* o )
{
  mparents.remove( o );
}
