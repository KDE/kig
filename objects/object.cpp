/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese

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
#include "../kig/constructactions.h"

#include <klocale.h>

Object::Object()
  : mColor( Qt::blue ), selected(false), shown (true), complete (false), valid(true)
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

Types& Object::types()
{
  static Types t;
  return t;
}

Object* Object::newObject( const QCString& type )
{
  return types().buildObject( type );
}

KAction* Object::sConstructAction( KigDocument* d, Type* t, int cut )
{
  return new ConstructAction( d, t, cut );
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
