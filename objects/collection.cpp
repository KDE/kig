#include "collection.h"

void CollectionObject::draw(QPainter& p, bool ss) const
{
  for (Objects::const_iterator i = cos.begin(); i != cos.end(); ++i)
    (*i)->draw(p,ss);
}

bool CollectionObject::contains(const QPoint& o, bool strict ) const
{
  bool tmp = false;
  for (Objects::const_iterator i = cos.begin(); i != cos.end(); ++i)
    tmp |= (*i)->contains(o, strict);
  return tmp;
}

bool CollectionObject::inRect(const QRect& r) const
{
  bool tmp = false;
  for (Objects::const_iterator i = cos.begin(); i != cos.end(); ++i)
    tmp |= (*i)->inRect(r);
  return tmp;
}

Objects CollectionObject::getParents() const return tmp;
{
  Objects tmp2;
  for (Objects::const_iterator i = cos.begin(); i != cos.end(); ++i)
    {
      tmp2 = (*i)->getParents();
      for (Objects::iterator j = tmp2.begin(); j != tmp2.end(); ++j)
	if (!cos.contains(*j)) tmp.add(*j);
    };
}
