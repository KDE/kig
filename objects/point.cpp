#include "point.h"

#include <qpainter.h>

#include <kdebug.h>

#include <typeinfo>

#include "segment.h"
#include "curve.h"

bool Point::contains(const QPoint& o, bool strict) const
{
  if (strict) return o == QPoint(x,y);
  return abs((o - QPoint(x,y)).manhattanLength()) < 4;
};

void Point::draw (QPainter& p, bool ss) const
{
  if (!shown) return;
  p.setBrush (QBrush(Qt::blue));
  p.setPen( QPen ( Qt::blue, 1 ) );
  if (selected && ss)
    {
      p.setPen( QPen (Qt::red, 1) );
      p.setBrush (QBrush(Qt::red));
    };
  p.drawEllipse(qRound(x-2), qRound(y-2),5,5);
  p.setBrush (Qt::NoBrush);
};

void Point::startMove(const QPoint& p)
{
  pwwsm = QPoint(x,y);
  pwwlmt = p;
};

void Point::moveTo(const QPoint& p)
{
  x+=(p-pwwlmt).x();
  y+=(p-pwwlmt).y();
  pwwlmt = p;
};

void Point::stopMove()
{
};

// void Point::cancelMove()
// {
//   x=pwwsm.x();
//   y=pwwsm.y();
// };

QString MidPoint::wantArg(const Object* o) const
{
  if (toSegment(o))
    {
      if(!p1 && !p2) return i18n("On segment");
      else return 0;
    };
  if (!toPoint(o)) return 0;
  if (!p1) return i18n("First point");
  else if (!p2) return i18n("Second point");
  return 0;
};

bool MidPoint::selectArg(Object* o)
{
  Segment* s;
  if ((s = toSegment(o)))
    {
      assert (!(p1 ||p2));
      selectArg(s->getPoint1());
      return selectArg(s->getPoint2());
    };
  // if we get here, o should be a point
  Point* p = toPoint(o);
  assert (p);

  if (!p1) p1 = p;
  else if (!p2)
    {
      p2 = p;
      complete = true;
    }
  else { kdError() << k_funcinfo << " selectArg on a complete midpoint... " << endl; return true; };
  o->addChild(this);
  return complete;
};

void MidPoint::unselectArg(Object* o)
{
  if (o == p1)
    {
      p1=p2;
      p2=0;
    }
  else if (o == p2)
    {
      p2 = 0;
    }
  o->delChild(this);
};

void MidPoint::startMove(const QPoint& p)
{
  pwwlmt = pwwsm = p;
  if (contains(p,false))
    {
      howm = howmMoving;
      p2->startMove(p2->toQPoint());
    }
  else howm = howmFollowing;
}

void MidPoint::moveTo(const QPoint& p)
{
  if (howm == howmFollowing)
    {
      calc();
      return;
    }
  x=p.x();
  y=p.y();
  p2->moveTo( ( (*this)*2-*p1).toQPoint());
}

void MidPoint::stopMove()
{
  pwwlmt = pwwsm = QPoint(x,y);
};

// void MidPoint::cancelMove()
// {
//   x = pwwsm.x();
//   y = pwwsm.y();
// }

void MidPoint::calc()
{
  assert (p1 && p2);
  setX(((p1->getX() + p2->getX())/2));
  setY(((p1->getY() + p2->getY())/2));
}

ConstrainedPoint::ConstrainedPoint(Curve* inC, const QPoint& inPt)
  : c(inC)
{
  c->addChild(this);
  p = c->getParam(inPt);
  kdDebug() << k_funcinfo << "param: "<< p << endl;
  calc();
}

void ConstrainedPoint::calc()
{
  if (!c) return;
  Point::operator=(c->getPoint(p));
}

Objects ConstrainedPoint::getParents() const
{
  Objects tmp;
  tmp.append(c);
  return tmp;
}
void ConstrainedPoint::moveTo(const QPoint& pt)
{
  p = c->getParam(pt);
  calc();
}

bool ConstrainedPoint::selectArg( Object* o)
{
  if (!c) c = Object::toCurve(o);
  return c;
}
QString ConstrainedPoint::wantArg(const Object* o) const
{
  if (!c && Object::toCurve(o)) return i18n("On Curve");
  return 0;
}
MidPoint::MidPoint(const MidPoint& m)
  : Point()
{
  p1 = m.p1;
  p1->addChild(this);
  p2 = m.p2;
  p2->addChild(this);
  complete = m.complete;
  if (complete) calc();
}
ConstrainedPoint::ConstrainedPoint( const ConstrainedPoint& cp)
  : Point()
{
  p = cp.p;
  c = cp.c;
  c->addChild(this);
  complete = cp.complete;
  if (complete) calc();
}

map<QCString,double> Point::getParams()
{
  map<QCString,double> tmp;
  tmp["x"] = x;
  tmp["y"] = y;
  return tmp;
}

void Point::setParams(const map<QCString,double>& m)
{
  x = m.find("x")->second;
  y = m.find("y")->second;
};
map<QCString,double> ConstrainedPoint::getParams()
{
  map<QCString,double> tmp;
  tmp["param"] = p;
  return tmp;
}
void ConstrainedPoint::setParams(const map<QCString,double>& m)
{
  p = m.find("param")->second;
}

ConstrainedPoint::ConstrainedPoint(const double inP)
  : p(inP), c(0)
{
};

ConstrainedPoint::ConstrainedPoint()
  : p(0.5), c(0)
{
    
}
map<QCString,double> MidPoint::getParams()
{
  return map<QCString, double>();
}
void MidPoint::setParams( const map<QCString,double>& /*m*/)
{
}
