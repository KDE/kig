#include "circle.h"

#include <klocale.h>
#include <kdebug.h>

// doing this because #include <cmath> or <math.h> don't seem to work :(
extern "C" {
  double round(double a);
};

Circle::Circle()
{
};

Circle::~Circle()
{
};

bool Circle::contains (const QPoint& o, bool strict) const
{
  return fabs(calcRadius(qpc, Point(o)) - radius) < (strict ? 1 : 5);
};

void Circle::draw (QPainter& p, bool ss) const
{
  p.setPen(QPen (Qt::blue, 1));
  if (selected && ss)
    p.setPen(QPen (Qt::red, 1));
  drawCircle(p, qpc, radius);
};

bool Circle::inRect (const QRect& r) const
{
  return r.contains(qpc.toQPoint());
};

Point Circle::getPoint (double p) const
{
  return Point(qpc) + Point (cos(p * 2 * M_PI), sin(p * 2 * M_PI)) * radius;
};

double Circle::getParam (const Point& p) const
{
  Point tmp = p - Point(qpc);
  return fmod((atan2(tmp.getY(), tmp.getX())) / ( 2 * M_PI ) + 1, 1);
};

CircleBCP::CircleBCP()
  : poc (0), centre(0)
{
};

QString CircleBCP::wantArg (const Object* o) const
{
  if (!o->toPoint()) return 0;
  if (complete) return 0;
  if (!centre) return i18n("Centre point");
  assert (!poc); // we're not completed, and do have a center...
  return i18n("Through point");
};

bool CircleBCP::selectArg (Object* o)
{
  assert (!complete);
  Point* p;
  assert ((p=o->toPoint()));
  if(centre) poc = p;
  else centre = p;
  o->addChild(this);
  if(poc) { return complete = true;};
  return false;
};

void CircleBCP::unselectArg(Object* o)
{
  assert (centre == o);
  centre = 0;
};

void CircleBCP::startMove(const QPoint& p)
{
  if (centre->contains(p, false))
    {
      wawm = notMoving;
    }
  else if (poc->contains(p, false)) // we let poc do the work, which will be faster too
    {
      wawm = lettingPocMove;
      poc->startMove(p);
    }
  else
    {
      wawm=movingPoc;
      pwpsm = QPoint(poc->getX(), poc->getY());
      poc->startMove(pwpsm);
    };
};

void CircleBCP::moveTo(const QPoint& p)
{
  if (wawm == lettingPocMove)
    {
      poc->moveTo(p);
    }
  else if (wawm == movingPoc)
    {
      double nRadius = calcRadius(*centre, Point(p));
      QPoint nPoc= (*centre + (*poc-*centre)*(nRadius/radius)).toQPoint();
      poc->moveTo(nPoc);
    };
  calc();
};

void CircleBCP::stopMove()
{
  wawm = notMoving;
};

void CircleBCP::cancelMove()
{
};

void CircleBCP::calc()
{
  if (poc && centre)
    {
      radius = calcRadius(*centre, *poc);
      qpc = *centre;
    };
};

QString CircleBTP::wantArg(const Object*) const
{
  if ( !p1 || !p2 || !p3 ) return i18n("Through point");
  else return 0;
}

bool CircleBTP::selectArg(Object* o)
{
  Point* p = o->toPoint();
  assert(p);
  if (!p1) p1 = p;
  else if (!p2) p2 = p;
  else p3 = p;
  o->addChild(this);
  if (p3) { return complete = true; };
  return false;
}

void CircleBTP::unselectArg(Object* o)
{
  assert (p1 == o || p2 == o || p3 == o);
  if (p1 == o)
    p1 = p3;
  else if (p2 == o)
    p2 = p3;
  p3 = 0;
  o->delChild(this);
}

inline double sqr (double x)
{
  return x*x;
};

inline double determinant (double a, double b, double c, double d)
{
  return a*d-b*c;
};

Point CircleBTP::calcCenter(double ax, double ay, double bx, double by, double cx, double cy) const
{
  double xdo = bx-ax;
  double ydo = by-ay;

  double xao = cx-ax;
  double yao = cy-ay;

  double a2 = sqr(xdo) + sqr(ydo);
  double b2 = sqr(xao) + sqr(yao);

  double numerator = (xdo * yao - xao * ydo);
  double denominator = 0.5 / numerator;

  double centerx = ax - (ydo * b2 - yao * a2) * denominator;
  double centery = ay + (xdo * b2 - xao * a2) * denominator;

  return Point(centerx, centery);
}

Objects CircleBTP::getParents() const return objs;
{
  objs.push( p1 );
  objs.push( p2 );
  objs.push( p3 );
}

Objects CircleBCP::getParents() const return objs;
{
    objs.push( centre );
    objs.push( poc );
}

void CircleBCP::drawPrelim( QPainter& p, const QPoint& pt) const
{
  if (!centre || !shown) return;
  p.setPen(QPen (Qt::red, 1));
  drawCircle(p,*centre, calcRadius(*centre, Point(pt)));
};

void CircleBCP::getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& t) const
{
  if (!centre||!shown) return;
  circleGetOverlay(*centre, calcRadius(*centre, Point(t)),list,border);
}

void CircleBTP::drawPrelim( QPainter& p, const QPoint& t) const
{
  if (!p1 || !shown) return;
  double xa = p1->getX(), ya = p1->getY();
  double xb = t.x(), yb = t.y();
  double xc,yc;
  if (p2) { xc=p2->getX(); yc = p2->getY(); }
  else {
    // midpoint:
    double xm  = (xa + xb)/2;
    double ym = (ya+yb)/2;
    // direction of the perpend:
    double m = -(xb-xa)/(yb-ya);
    // 
    double ta = 1+m*m;
    double tb = -2*m*m*xm - m*2*ym;
    double tc = xm*m*m + xm*m*2*ym + ym*ym - (yb-ya)*(yb-ya) - (xb-xa)*(xb-xa);
    double det = sqrt(tb*tb-4*ta*tc);
    xc = (tb + det)/2/ta;
    yc = m*(xb-xm) + ym;
    // just picking a random point till i get the above code figgered
    // out (it's supposed to choose a point so that p(xa,ya), q(xb,yb)
    // and r(xc,yc) form a triangle with sides of equal length.
    xc = 359; yc = 158;
  };
  p.setPen(QPen (Qt::red, 1));
  Point nQpc = calcCenter(xa, ya, xb, yb, xc, yc);
  drawCircle(p,
	     nQpc,
	     calcRadius(nQpc, t));
}

void CircleBTP::calc()
{
  double ax = p1->getX();
  double ay = p1->getY();
  double bx = p2->getX();
  double by = p2->getY();
  double cx = p3->getX();
  double cy = p3->getY();
  // the center coords:
  qpc = calcCenter(ax,ay,bx,by,cx,cy);
  radius = calcRadius(qpc, *p3);
};

void Circle::circleGetOverlay(const Point& centre, double radius, QPtrList<QRect>& list, const QRect& border, const QRect& currentRect)
{
  // this code is an adaptation of code from Marc Bartsch, from his KGeo
  Point tl = currentRect.topLeft();
  Point c = currentRect.center();
  Point br = currentRect.bottomRight();
  Point tr = Point(br.getX(), tl.getY());
  Point bl = Point(tl.getX(), br.getY());

  // 1.5 should actually be 1.414...
  double fault = currentRect.width()*1.5 + 2;
  double radiusBig = radius + fault ;
  double radiusSmall = radius - fault ;

  // if the circle doesn't touch this rect, we return
  // too far from the centre
  if (((tl - centre).length() > radiusBig) &&
      ((tr - centre).length() > radiusBig) &&
      ((bl - centre).length() > radiusBig) &&
      ((br - centre).length() > radiusBig))
    return;

  // too near to the centre
  if (((tl - centre).length() < radiusSmall) &&
      ((tr - centre).length() < radiusSmall) &&
      ((bl - centre).length() < radiusSmall) &&
      ((br - centre).length() < radiusSmall))
    return;

  // the rect contains some of the circle
  // -> if it's small enough, we keep it
  if (currentRect.width() < Object::overlayRectSize()) {
    list.append(new QRect(currentRect));
  } else {
    // this func works recursive: we subdivide the current rect, and if
    // it is of a good size, we keep it, else we handle it again
    QSize size = currentRect.size()/2;
    size.setWidth(size.width()+1);
    size.setHeight(size.height()+1);
    QRect r1 (tl.getX(), tl.getY(), size.width(), size.height());
    r1.normalize();
    QRect r2 (c.getX(), tl.getY(), size.width(), size.height());
    r2.normalize();
    QRect r3 (tl.getX(), c.getY(), size.width(), size.height());
    r3.normalize();
    QRect r4 (c.getX(), c.getY(), size.width(), size.height());
    r4.normalize();
    circleGetOverlay(centre, radius, list, border, r1);
    circleGetOverlay(centre, radius, list, border, r2);
    circleGetOverlay(centre, radius, list, border, r3);
    circleGetOverlay(centre, radius, list, border, r4);
  };
}
void CircleBTP::getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& t) const
{
  if (!p1 || !shown) return;
  double xb, yb;
  if (p2) { xb=p2->getX(); yb = p2->getY(); }
  else {
    // we pick some random point (if anyone has better ideas, please tell me...
    // i have had a suggestion to make this point the third corner of
    // a triangle of p1, pt and with equal distance between the
    // corners (gelijkzijdig in dutch, i don't know english geometry terms
    xb = 359; yb = 158;
  };
  Point nQpc = calcCenter(p1->getX(), p1->getY(), xb, yb, t.x(), t.y());
  circleGetOverlay(nQpc, calcRadius (nQpc, *p1), list, border);
}
