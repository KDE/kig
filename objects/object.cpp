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

#include "point.h"
#include "segment.h"
#include "circle.h"
#include "curve.h"
#include "line.h"
#include "macro.h"

Object::Object()
  : mColor( Qt::blue ), selected(false), shown (true), complete (false), valid(true)
{
};

void Object::setSelected(bool in)
{
  selected = in;
}

Point* Object::toPoint(Object* o)
{
  Object* tmp = o;
  MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<Point*>(tmp);
}

Segment* Object::toSegment(Object* o)
{
  Object* tmp = o;
  MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<Segment*>(tmp);
}

Line* Object::toLine(Object* o)
{
  Object* tmp = o;
  MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<Line*>(tmp);
}

Circle* Object::toCircle(Object* o)
{
  Object* tmp = o;
  MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<Circle*>(tmp);
}

const Point* Object::toPoint(const Object* o)
{
  const Object* tmp = o;
  const MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<const MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<const Point*>(tmp);
}

const Segment* Object::toSegment(const Object* o)
{
  const Object* tmp = o;
  const MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<const MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<const Segment*>(tmp);
}

const Line* Object::toLine(const Object* o)
{
  const Object* tmp = o;
  const MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<const MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<const Line*>(tmp);
}

const Circle* Object::toCircle(const Object* o)
{
  const Object* tmp = o;
  const MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<const MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<const Circle*>(tmp);
}

const Curve* Object::toCurve(const Object* o)
{
  const Object* tmp = o;
  const MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<const MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<const Curve*>(tmp);
}

Curve* Object::toCurve(Object* o)
{
  Object* tmp = o;
  MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<Curve*>(tmp);
}

const ConstrainedPoint* Object::toConstrainedPoint(const Object* o)
{
  const Object* tmp = o;
  const MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<const MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<const ConstrainedPoint*>(tmp);
}

ConstrainedPoint* Object::toConstrainedPoint(Object* o)
{
  Object* tmp = o;
  MacroObjectOne* tmp2;
  if ((tmp2=dynamic_cast<MacroObjectOne*>(tmp))) tmp = tmp2->getFinal();
  return dynamic_cast<ConstrainedPoint*>(tmp);
}

Objects Object::getAllChildren() const
{
  // what we are going to return...
  Objects tmp;
  // objects to iterate over...
  Objects objs = children;
  // contains the objects to iterate over the next time around...
  Objects objs2;
  while (!objs.isEmpty())
    {
      for (Object* i = objs.first(); i; i = objs.next())
	{
	  tmp.add(i);
	  objs2 |= i->getChildren();
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
