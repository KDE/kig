/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#include "object.h"

#include "coordproppoint.h"

#include "../misc/types.h"
#include "../misc/type.h"
#include "../modes/constructing.h"
#include "../modes/popup.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

#include <klocale.h>
#include <qpopupmenu.h>

myvector<Type*> Object::sbuiltintypes;
myvector<Type*> Object::susertypes;
Types Object::stypes;

Object::Object()
  : mColor( Qt::blue ), selected(false), mshown (true), mvalid(true)
{
};

void Object::setSelected(bool in)
{
  selected = in;
}

Objects Object::getAllChildren() const
{
  // what we are going to return...
  Objects tmp;
  // objects to iterate over...
  Objects objs = children;
  // contains the objects to iterate over the next time around...
  Objects objs2;
  while( !objs.empty() )
  {
    for( Objects::iterator i = objs.begin();
         i != objs.end(); ++i )
    {
      tmp.upush( *i );
      objs2 |= (*i)->getChildren();
    };
    objs = objs2;
    objs2.clear();
  };
  return tmp;
}

Object::prop_map Object::getParams()
{
  prop_map m;
  m["color"] = mColor.name();
  m["shown"] = QString::fromLatin1( mshown ? "true" : "false" );
  return m;
}

void Object::setParams( const prop_map& m )
{
  {
    prop_map::const_iterator p = m.find("color");
    if( p == m.end() ) mColor = Qt::blue;
    else mColor = QColor( p->second );
    assert( mColor.isValid() );
  };
  {
    prop_map::const_iterator p = m.find("shown");
    if( p != m.end() && p->second == QString::fromUtf8( "false" ) )
      mshown = false;
    else mshown = true;
  };
}

const Types& Object::types()
{
  return stypes;
}

const myvector<Type*>& Object::builtinTypes()
{
  return sbuiltintypes;
}

const myvector<Type*>& Object::userTypes()
{
  return susertypes;
}

Object* Object::newObject( const QCString& type, const Objects& parents,
                           const prop_map& params )
{
  return types().buildObject( type, parents, params );
}

KigMode* Object::sConstructMode( StdConstructibleType* ourtype,
                                 KigDocument* theDoc,
                                 NormalMode* previousMode )
{
  return new StdConstructionMode( ourtype, previousMode, theDoc );
}

const QString Object::vTBaseTypeName() const
{
  return i18n(vBaseTypeName());
}

void Object::addChild(Object* o)
{
  children.push_back(o);
}

void Object::delChild(Object* o)
{
  children.remove( o );
}

Object::Object( const Object& o )
  : mColor( o.mColor ), selected( false ), mshown( true ),
    mvalid( o.mvalid )
{

}

void Object::drawWrap(KigPainter& p, bool ss) const
{
  if ( mshown && mvalid ) draw(p,ss);
}

void Object::addBuiltinType( Type* t )
{
  stypes.addType( t );
  sbuiltintypes.push_back( t );
}

void Object::addUserType( Type* t, bool notify )
{
  Types ts;
  ts.addType( t );
  addUserTypes( ts, notify );
}

void Object::addUserTypes( Types& t, bool notify )
{
  for ( Types::const_iterator i = t.begin(); i != t.end(); ++i )
  {
    stypes.addType( i->second );
    susertypes.push_back( i->second );
  };
  if ( notify )
  {
    typedef myvector<KigDocument*> mv;
    mv& v = KigDocument::documents();
    for ( mv::const_iterator i = v.begin(); i != v.end(); ++i )
    {
      (*i)->unplugActionLists();
      for ( Types::const_iterator j = t.begin(); j != t.end(); ++j )
      {
        (*i)->addType( j->second );
      };
      (*i)->plugActionLists();
    };
  };
  t.clear();
}

void Object::removeUserType( Type* t )
{
  susertypes.remove( t );
  stypes.removeType( t );
  typedef myvector<KigDocument*> mv;
  mv& v = KigDocument::documents();
  for ( mv::const_iterator i = v.begin(); i != v.end(); ++i )
  {
    (*i)->unplugActionLists();
  };
  t->deleteActions();
  for ( mv::const_iterator i = v.begin(); i != v.end(); ++i )
  {
    (*i)->plugActionLists();
  };
}

// void Object::removeUserTypes( Types& t, bool notifyParts )
// {

// }
bool Object::valid() const
{
  return mvalid;
}

bool Object::shown() const
{
  return mshown;
}
void Object::setShown( bool in )
{
  mshown = in;
}

void Object::addActions( NormalModePopupObjects& m )
{
  QPopupMenu* pm = new QPopupMenu( &m, "construct_coord_props_menu" );

  bool content = false;         // true if we have content..

  QCStringList s = properties();
  for ( uint i = 0; i < numberOfProperties(); ++i )
  {
    Property p = property( i );
    if ( p.type() == Property::Coord )
    {
      content = true;
      QString str = i18n( s[i] );
      uint id = pm->insertItem( str, i );
      assert( id == i );
    };
  };

  m.addPopupAction( 3849, i18n( "Construct the ..." ), pm );
  return;
}

void Object::doNormalAction( int, KigDocument*, KigWidget*, NormalMode* )
{
  return;
}

void Object::doPopupAction( int popupid, int actionid, KigDocument* doc, KigWidget* w, NormalMode*)
{
  assert( popupid == 3849 );
  uint pid = static_cast<uint>( actionid );
  assert( property( pid ).type() == Property::Coord );
  Object* o = new CoordinatePropertyPoint( this, pid );
  o->calc();
  doc->addObject( o );
  w->redrawScreen();
}

const QCStringList Object::properties() const
{
  QCStringList s;
  s << I18N_NOOP( "Base Object Type" );
  assert( s.size() == Object::numberOfProperties() );
  return s;
}

const Property Object::property( uint which ) const
{
  assert( which == 0 );
  return Property( vTBaseTypeName() );
}

const uint Object::numberOfProperties() const
{
  return 1;
}

void Object::calcForWidget( const KigWidget& )
{
  calc();
}

void Object::startMove( const Coordinate&, const ScreenInfo& )
{
  // default is that we don't move..
}

void Object::moveTo(const Coordinate&)
{
  // default is that we don't move..
}

void Object::stopMove()
{
  // default is that we don't move..
}
