#include "macro.h"

#include "../misc/hierarchy.h"

QString MacroObject::wantArg( const Object* o) const
{
  if (complete) return 0;
  if (o->vBaseTypeName() != hier->getGegElems()[arguments.count()]->getTypeName())  return 0;
  else return i18n("Select this %1").arg(o->vTBaseTypeName());
};

bool MacroObject::selectArg(Object* o)
{
  assert(o->vBaseTypeName() == hier->getGegElems()[arguments.count()]->getTypeName());
  arguments.append(o);
  o->addChild(this);
  if (arguments.count() != hier->getGegElems().size()) return false;
  return complete = true;
}

MacroObject::MacroObject(ObjectHierarchy* inHier)
  : hier(inHier)
{
}

MacroObjectOne::MacroObjectOne( ObjectHierarchy* inHier )
  : MacroObject(inHier), final(0), constructed(false)
{
//   assert (inHier->getFinElems().size() == 1);
}

MacroObjectOne::~MacroObjectOne()
{
  cos.deleteAll();
  delete final;
}

void MacroObjectOne::draw(QPainter& p, bool ss) const
{
  final->draw(p, ss);
}

bool MacroObjectOne::contains(const QPoint& p, bool strict ) const
{
  return final->contains(p, strict);
}

bool MacroObjectOne::inRect(const QRect& r) const
{
  return final->inRect(r);    
}

void MacroObjectOne::drawPrelim(QPainter&, const QPoint& ) const
{
};

void MacroObjectOne::startMove(const QPoint& p)
{
  final->startMove(p);    
};
void MacroObjectOne::moveTo(const QPoint& p)
{
  final->moveTo(p);    
}
void MacroObjectOne::stopMove()
{
  final->stopMove();    
}

void MacroObjectOne::calc()
{
  if (!constructed) {
    handleNewObjects( hier->fillUp(arguments) );
    constructed = true;
  };
  cos.calc();
  // two times, cause the order in which we calc sometimes sucks
  cos.calc();
  final->calc();
}

QCString MacroObjectOne::vBaseTypeName() const
{
  if (!final)
    {
      // actually this returns a FullTypeName(), but i'm hoping this
      // part of the function won't get used too often
      return hier->getFinElems()[0]->getTypeName();
    }
  else return final->vBaseTypeName();
};

QCString MacroObjectOne::vFullTypeName() const
{
  if (!final)
    {
      return hier->getFinElems()[0]->getTypeName();
    }
  else return final->vFullTypeName();
};

void MacroObjectOne::handleNewObjects(const Objects& o)
{
  hier->calc();
  final = hier->getFinElems()[0]->actual;
  cos = o;
  cos.remove(final);
  for (Object* i = arguments.first(); i; i = arguments.next())
    {
      for (Object* j = cos.first(); j; j = cos.next())
	{
	  i->delChild(j);
	};
    };
  for (Object* i = cos.first(); i; i = cos.next())
    i->setShown(false);
}
// void MacroObjectMulti::handleNewObjects(const Objects& o)
// {
//   cos = o;
// }

