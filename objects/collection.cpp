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


#include "collection.h"

void CollectionObject::draw(KigPainter& p, bool ss) const
{
  Object* o;
  for (Objects::iterator i(cos); (o = i.current()); ++i)
    o->draw(p,ss);
}

bool CollectionObject::contains(const Coordinate& o, bool strict ) const
{
  Object* i;
  for (Objects::iterator it(cos); (i = it.current()); ++it)
    if(i->contains(o, strict)) return true;
  return false;
}

bool CollectionObject::inRect(const Rect& r) const
{
  Object* i;
  for (Objects::iterator it( cos ); (i = it.current()); ++it)
    if( i->inRect(r) ) return true;
  return false;
}

Objects CollectionObject::getParents() const
{
  Objects tmp;
  Objects tmp2;
  Object* i;
  for (Objects::iterator it( cos ); (i = it.current()); ++it)
    {
      tmp2 = i->getParents();
      Object* j;
      for (Objects::iterator jt (tmp2); (j = jt.current()); ++jt)
	if (!cos.contains(j)) tmp.add(j);
    };
  return tmp;
}
