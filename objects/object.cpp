/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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

#include "../misc/types.h"
#include "../misc/type.h"
#include "../modes/constructing.h"

#include "../kig/kig_part.h"

#include <klocale.h>

myvector<Type*> Object::sbuiltintypes;
myvector<Type*> Object::susertypes;
Types Object::stypes;

Object::Object()
  : mColor( Qt::blue ), selected(false), shown (true), valid(true)
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

std::map<QCString,QString> Object::getParams()
{
  std::map<QCString, QString> m;
  m["color"] = mColor.name();
  m["shown"] = QString::fromUtf8( shown ? "true" : "false" );
  return m;
}

void Object::setParams( const std::map<QCString,QString>& m )
{
  {
    std::map<QCString,QString>::const_iterator p = m.find("color");
    if( p == m.end() ) mColor = Qt::blue;
    else mColor = QColor( p->second );
    assert( mColor.isValid() );
  };
  {
    std::map<QCString,QString>::const_iterator p = m.find("shown");
    if( p != m.end() && p->second == QString::fromUtf8( "false" ) )
      shown = false;
    else shown = true;
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
                           const std::map<QCString, QString>& params )
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
    : mColor( o.mColor ), selected( false ), shown( true ),
      valid( o.valid )
{

}

void Object::drawWrap(KigPainter& p, bool ss) const
{
  if (!shown) return;
  draw(p,ss);
}

const QStringList Object::objectActions() const
{
  return QStringList();
}

void Object::doAction( int, KigDocument*, KigView*, NormalMode* )
{
  return;
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
