#include "locus.h"

#include "../misc/hierarchy.h"

#include <kdebug.h>

void Locus::draw(QPainter& p, bool ss) const
{
  // we don't let the points draw themselves, since that would get us
  // the big points (5x5) like we normally see them...
  // all objs are of the same type, so we only check the first
  if (isPointLocus())
    {
      kdDebug() << k_funcinfo << " at line no. " << __LINE__ << " - count: " << pts.size()<< endl;

      QPen pen (selected && ss ? Qt::red : Qt::blue);
      p.setPen(pen);
      for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
	{
	  p.drawPoint( i->pt.toQPoint() );
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
  if (!isPointLocus())
    {
      Object* i;
      for (Objects::iterator it(objs); (i = it.current()); ++it)
	{
	  if (i->contains(o, strict)) return true;
	};
    }
  else
    {
      for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
	{
	  if (i->pt.contains(o, strict)) return true;
	};
    };
  return false;
}

bool Locus::inRect(const QRect& r) const
{
  if (!isPointLocus())
    {
      Object* i;
      for (Objects::iterator it(objs); (i = it.current()); ++it)
	{
	  if (i->inRect(r)) return true;
	};
    }
  else 
    {
      for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
	{
	  if (i->pt.inRect(r)) return true;
	};
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
      // if obj is a point, we are a pointLocus
      _pointLocus = (bool) toPoint(obj);
      kdDebug() << k_funcinfo << " at line no. " << __LINE__
		<< " - _pointLocus: " << _pointLocus << endl;
    };
  if (cp && obj)
    {
      complete = true;
      // construct our hierarchy...
      Objects given;
      given.append(cp);
      Objects final;
      final.append(obj);
      hierarchy = new ObjectHierarchy(given, final, 0);
      // calc...
      calc();
    }
  return complete;
}

void Locus::calc()
{
  if (isPointLocus()) calcPointLocus();
  else calcObjectLocus();
}

void Locus::getOverlay(QPtrList<QRect>& list, const QRect& border) const
{
  Object* i;
  if (!isPointLocus())
    {
      for (Objects::iterator it (objs); (i = it.current()); ++it)
	{
	  i->getOverlay(list, border);
	};
    }
  else
    {
      for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
	{
	  QRect* r = new QRect (0,0,2,2);
	  r->moveCenter(i->pt.toQPoint());
	  if (r->intersects(border))list.append(r);
	};
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

inline Locus::CPts::iterator Locus::addPoint(double param)
{
  cp->setP(param);
  cp->calc();
  hierarchy->calc();
  Point* p = Object::toPoint(obj);
  pts.push_front(CPt(*p, param));
  return pts.begin();
}

void Locus::recurse(CPts::iterator first, CPts::iterator last, int& i)
{
  double p = (first->pm+last->pm)/2;
  CPts::iterator n = addPoint(p);
  if (++i > numberOfSamples) return;
  if ((n->pt - first->pt).length() >3) recurse (n, first, i);
  if (i > numberOfSamples) return;
  if ((n->pt - last->pt).length() >3) recurse (n, last,i);
  if (i > numberOfSamples) return;
}

void Locus::calcPointLocus()
{
  kdDebug() << k_funcinfo << " at line no. " << __LINE__ << endl;
  pts.clear();
  double oldP = cp->getP();
  // TODO: improve (cf. KSeg: choose which samples we want...)
//   double period = double(1)/numberOfSamples;
//   for (double i = 0; i <= 1; i += period)
//     {
//       cp->setP(i);
//       cp->calc();
//       hierarchy->calc();
//       objs.append(obj->copy());
//     };
  CPts::iterator b = addPoint(0);
  CPts::iterator e = addPoint(1);
  int i = 2;
  recurse(b,e,i);
  // reset cp and its children to their former state...
  cp->setP(oldP);
  cp->calc();
  hierarchy->calc();
  kdDebug() << k_funcinfo << " at line no. " << __LINE__ << "  - count: " << pts.size()<< endl;
}

Locus::Locus(const Locus& loc)
  : Curve(), cp(loc.cp), obj(loc.obj), hierarchy(loc.hierarchy)
{
  cp->addChild(this);
  obj->addChild(this);
  complete = loc.complete;
  if (complete) calc();
}
