#include "locus.h"

#include "../misc/hierarchy.h"

#include <kdebug.h>

void Locus::draw(QPainter& p, bool ss) const
{
  // todo: draw lines between the points instead of only the
  // points...
  if (ss && selected ) p.setPen( QPen(Qt::red) );
  else p.setPen( QPen(Qt::blue) );
  kdDebug() << k_funcinfo << "number of points: " << points.size() << endl;
  for (PointList::const_iterator i = points.begin(); i!=points.end(); ++i)
    {
      p.drawPoint(i->toQPoint());
    };
}

bool Locus::contains(const QPoint& o, bool strict ) const
{
  for (PointList::const_iterator i = points.begin(); i != points.end(); ++i)
    {
      if (i->contains(o, strict)) return true;
    };
  return false;
}

bool Locus::inRect(const QRect& r) const
{
  for (PointList::const_iterator i = points.begin(); i != points.end(); ++i)
    {
      if (r.contains(i->toQPoint())) return true;
    };
  return false;
}

QString Locus::wantArg( const Object* o ) const
{
  if (toConstrainedPoint(o) && !cp)
    {
      return i18n("Moving point");
    };
  if (toPoint(o) && !point)
    {
      return i18n("Dependent point");
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
      assert (!point);
      point = toPoint(o);
      assert (point);
      point->addChild(this);
    };
  if (cp && point) { complete = true; calc(); }
  return complete;
}

void Locus::calc()
{
  points.clear();
  Objects given;
  given.append(cp);
  Objects final;
  final.append(point);
  hierarchy = new ObjectHierarchy(given, final, 0);
  double oldP = cp->getP();
  double period = double(1)/numberOfPoints;
  for (double i = 0; i <= 1; i += period)
    {
      cp->setP(i);
      cp->calc();
      hierarchy->calc();
      points.push_back(*point);
    };
  cp->setP(oldP);
  cp->calc();
  hierarchy->calc();
  kdDebug() << k_funcinfo << " at line no. " << __LINE__ << endl;
}

void Locus::getOverlay(QPtrList<QRect>& list, const QRect& border) const
{
  for (PointList::const_iterator i = points.begin(); i != points.end(); ++i)
    {
      QRect* r = new QRect (0, 0,overlayRectSize(), overlayRectSize());
      r->moveTopLeft(i->toQPoint() - QPoint(2,2));
      while (r->contains(i->toQPoint())) ++i;
      if (border.intersects(*r)) list.append(r);
    };
}

Point Locus::getPoint(double param) const
{
  double tmp = cp->getP();
  cp->setP(param);
  hierarchy->calc();
  Point t= *point;
  cp->setP(tmp);
  hierarchy->calc();
  return t;
}
double Locus::getParam(const Point&) const
{
  return 0.5;
}
