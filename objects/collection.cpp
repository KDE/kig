#include "collection.h"

void CollectionObject::draw(QPainter& p, bool ss) const
{
  Object* o;
  for (Objects::iterator i(cos); (o = i.current()); ++i)
    o->draw(p,ss);
}

bool CollectionObject::contains(const QPoint& o, bool strict ) const
{
  Object* i;
  for (Objects::iterator it(cos); (i = it.current()); ++it)
    if(i->contains(o, strict)) return true;
  return false;
}

bool CollectionObject::inRect(const QRect& r) const
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
