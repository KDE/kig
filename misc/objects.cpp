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
