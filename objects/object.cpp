#include "object.h"

#include "point.h"
#include "segment.h"
#include "circle.h"
#include "curve.h"
#include "line.h"
#include "macro.h"

Object::Object()
  : selected(false), shown (true), complete (false)
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

void Object::saveXML( QDomDocument&, QDomElement& )
{
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
