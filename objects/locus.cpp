#include "locus.h"

#include "../misc/hierarchy.h"

#include <kdebug.h>

void Locus::draw(QPainter& p, bool ss) const
{
  kdDebug() << k_funcinfo << "number of objects: " << objs.count() << endl;
  // we don't let the points draw themselves, since that would get us
  // the big points (5x5) like we normally see them...
  // all objs are of the same type, so we only check the first
  if (toPoint(objs.getFirst()))
    {
      QPen pen (selected && ss ? Qt::red : Qt::blue);
      p.setPen(pen);
      Object* i;
      for (Objects::iterator it(objs); (i = it.current()); ++it)
	{
	  Point* tmp = toPoint(i);
	  assert (tmp);
	  p.drawPoint( toPoint(i)->toQPoint() );
	};
    }
  else
    {
      Object* i;
      for ( Objects::iterator it(objs); (i = it.current()); ++it)
	{
	  i->setSelected(selected);
	  i->draw(p, ss);
	}
    };
}

bool Locus::contains(const QPoint& o, bool strict ) const
{
  Object* i;
  for (Objects::iterator it(objs); (i = it.current()); ++it)
    {
      if (i->contains(o, strict)) return true;
    };
  return false;
}

bool Locus::inRect(const QRect& r) const
{
  Object* i;
  for (Objects::iterator it(objs); (i = it.current()); ++it)
    {
      if (i->inRect(r)) return true;
    };
  return false;
}

QString Locus::wantArg( const Object* o ) const
{
  if (toConstrainedPoint(o) && !cp)
    {
      return i18n("Moving point");
    };
  if (!obj)
    {
      return i18n("Dependent object");
    };
  return 0;
}

bool Locus::selectArg(Object* o)
{
  if (!cp && (cp = toConstrainedPoint(o)))
    {
      cp->addChild(this);
    }
  else 
    {
      assert (!obj);
      obj = o;
      obj->addChild(this);
    };
  if (cp && obj) { complete = true; calc(); }
  return complete;
}

void Locus::calc()
{
  objs.deleteAll();
  objs.clear();
  Objects given;
  given.append(cp);
  Objects final;
  final.append(obj);
  hierarchy = new ObjectHierarchy(given, final, 0);
  if (toPoint(obj)) calcPointLocus();
  else calcObjectLocus();
  kdDebug() << k_funcinfo << " at line no. " << __LINE__ << endl;
}

void Locus::getOverlay(QPtrList<QRect>& list, const QRect& border) const
{
  Object* i;
  for (Objects::iterator it (objs); (i = it.current()); ++it)
    {
      QRect* r = new QRect (0, 0, overlayRectSize(), overlayRectSize());
      r->moveTopLeft(toPoint(i)->toQPoint());
      if (border.intersects(*r)) list.append(r);
    };
}

Point Locus::getPoint(double param) const
{
  Point t;
  if (toPoint(obj))
    {
      double tmp = cp->getP();
      cp->setP(param);
      hierarchy->calc();
      t= *toPoint(obj);
      cp->setP(tmp);
      hierarchy->calc();
    }
  else t = Point();
  return t;
}
double Locus::getParam(const Point&) const
{
  return 0.5;
}
void Locus::calcObjectLocus()
{
  objs.deleteAll();
  objs.clear();
  double oldP = cp->getP();
  double period = double(1)/numberOfSamples;
  for (double i = 0; i <= 1; i += period)
    {
      cp->setP(i);
      cp->calc();
      hierarchy->calc();
      objs.append(obj->copy());
    };
  cp->setP(oldP);
  cp->calc();
  hierarchy->calc();
}
void Locus::calcPointLocus()
{
  objs.deleteAll();
  objs.clear();
  double oldP = cp->getP();
  // TODO: improve (cf. KSeg: choose which samples we want...)
  double period = double(1)/numberOfSamples;
  for (double i = 0; i <= 1; i += period)
    {
      cp->setP(i);
      cp->calc();
      hierarchy->calc();
      objs.append(obj->copy());
    };
  cp->setP(oldP);
  cp->calc();
  hierarchy->calc();
}

Locus::Locus(const Locus& loc)
  : Curve(), cp(loc.cp), obj(loc.obj), hierarchy(loc.hierarchy)
{
  cp->addChild(this);
  obj->addChild(this);
  complete = loc.complete;
  if (complete) calc();
}
