#include "locus.h"

#include "../misc/hierarchy.h"

void Locus::draw(QPainter& p, bool ss) const
{
  // todo: draw lines between the points instead of only the
  // points...
  p.setPen(ss&&selected?QPen(Qt::red) : QPen(Qt::blue));
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
  if (o->toConstrainedPoint() && !cp)
    {
      return i18n("Moving point");
    };
  if (o->toPoint() && !point)
    {
      return i18n("Dependent point");
    };
  return 0;
}

bool Locus::selectArg(Object* o)
{
  ConstrainedPoint* tmpCp;
  Point* tmpPt;
  if (!cp && (tmpCp = o->toConstrainedPoint()))
    {
      cp = tmpCp;
      cp->addChild(this);
    }
  else 
    {
      tmpPt = o->toPoint();
      assert (!point && tmpPt);
      point = tmpPt;
      point->addChild(this);
      complete = true;
    };
  if (complete) 
    {
      hierarchy = new ObjectHierarchy(cp, point, 0);
      double oldP = cp->getP();
      double period = double(1)/numberOfPoints;
      for (double i = 0; i < 1; i += period)
	{
	  cp->setP(i);
	  hierarchy->calc();
	  points.push_back(*point);
	};
      cp->setP(oldP);
      hierarchy->calc();
    };
  return complete;
}

void Locus::calc()
{
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
