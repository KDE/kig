#include "objects.h"

#include "../objects/object.h"

void Objects::deleteAll()
{
  for (Object* i = first(); i; i = next())
    delete i;
  clear();
};

void Objects::calc() const
{
  Object* o;
  for (iterator i(*this);(o=i.current()); ++i)
    o->calc();
};

void Objects::draw( QPainter& p, bool showSelection ) const
{
  Object* o;
  for (iterator i(*this); (o=i.current()); ++i)
    o->drawWrap(p, showSelection);
};

QPtrList<QRect> Objects::getOverlay( const QRect& border) const
{
  QPtrList<QRect> tmp;
  Object* o;
  for (iterator i(*this); (o = i.current()); ++i)
    o->getOverlayWrap(tmp, border);
  return tmp;
};
