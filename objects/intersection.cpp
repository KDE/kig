#include "intersection.h"

#include "segment.h"
#include "circle.h"
#include "line.h"

#include <kdebug.h>

// doing this because #include <cmath> or <math.h> don't seem to work :(
extern "C" {
  double round(double a);
  long double roundl(long double a);
};

QString IntersectionPoint::wantArg(const Object* o) const
{
  if (o->vBaseTypeName() == Point::sBaseTypeName()) return 0;
  int tmp = 0;
  if (segment1) ++tmp;
  if (segment2) ++tmp;
  if (line1) ++tmp;
  if (line2) ++tmp;
  if (circle1) ++tmp;
  if (circle2) ++tmp;
  if (tmp < 2) return i18n("On %1").arg(o->vTBaseTypeName());
  else return 0;
}

bool IntersectionPoint::selectArg(Object* o)
{
  Segment* s;
  Line* l;
  Circle* c;
  if ((s = toSegment(o))) {
    if (segment1) segment2 = s;
    else segment1 = s;
  }
  else if ((l = toLine(o))) {
    if (line1) line2 = l;
    else line1 = l;
  }
  else if ((c = toCircle(o))) {
    if (circle1) circle2 = c;
    else circle1 = c;
  }
  else {
    kdError() << k_funcinfo << " wrong argument: " << o->vBaseTypeName() <<endl;
    return complete;
  };
  o->addChild(this);
  int tmp = 0;
  if (segment1) ++tmp;
  if (segment2) ++tmp;
  if (line1) ++tmp;
  if (line2) ++tmp;
  if (circle1) ++tmp;
  if (circle2) ++tmp;
  if (tmp > 1) return complete = true;
  return complete = false;
}

void IntersectionPoint::unselectArg(Object* o)
{
  Segment* s;
  Line* l;
  Circle* c;
  if ((s = toSegment(o))) {
    if (segment1 == s) segment1 = segment2;
    segment2 = 0;
  }
  else if ((l = toLine(o))) {
    if (line1 == l) line1 = line2;
    line2 = 0;
  }
  else if ((c = toCircle(o))) {
    if (circle1 == c) circle1 = circle2;
    circle2 = 0;
  }
  else {
    kdError() << k_funcinfo << "wrong argument: " << o->vBaseTypeName() <<endl;
    return;
  };
  complete = false;
}
Objects IntersectionPoint::getParents() const
{
  Objects tmp;
  if (segment1) tmp.append(segment1);
  if (segment2) tmp.append(segment2);
  if (line1) tmp.append(line1);
  if (line2) tmp.append(line2);
  if (circle1) tmp.append(circle1);
  if (circle2) tmp.append(circle2);
  return tmp;
};

void IntersectionPoint::calc()
{
  // a counts how many p's we've already got...
  int a = 0;
  Point p1, p2, p3, p4;
  if (segment1) {
    p1 = segment1->getP1();
    p2 = segment1->getP2();
    ++a;
  }
  if (segment2) {
    (a?p3:p1) = segment2->getP1();
    (a?p4:p2) = segment2->getP2();
    ++a;
  };
  if (a<2 && line1) {
    (a?p3:p1) = line1->getP1();
    (a?p4:p2) = line1->getP2();
    ++a;
  };
  if (a<2 && line2) {
    (a?p3:p1) = line2->getP1();
    (a?p4:p2) = line2->getP2();
    ++a;
  };
  Point result;
  if (a == 2) result = calc(p1,p2,p3,p4);
  else {
    kdDebug() << k_funcinfo <<"not implemented yet: intersection with a circle" << endl;
    // plain bogus value...
    result = Point(50,50);
  }
  x = qRound(result.getX());
  y = qRound(result.getY());
}

Point IntersectionPoint::calc(const Point& p1, const Point& p2, const Point& p3, const Point& p4)
{
  long double xa = p1.getX(),
    xb = p2.getX(),
    xc = p3.getX(),
    xd = p4.getX();
  long double ya = p1.getY(),
    yb = p2.getY(),
    yc = p3.getY(),
    yd = p4.getY();
  
  long double a = (yb-ya)/(xb-xa);
  long double b = (yd-yc)/(xd-xc);
  
  if ((fabsl(xb - xa)> 1) && (fabsl(xd - xc) > 1))
  {
    long double nx = (yc - ya + xa*a - xc*b)/(a-b);
    long double ny = (nx-xa)*a+ya;
    return Point( nx, ny );
  }
  else {
    // we would have had a divide by zero
    if (fabsl(xb - xa) < 1) {
      // xa == xb --> the first line is almost horizontal
      long double nx = xb;
      long double ny = (nx-xc)*b+yc;
      return Point( nx, ny);
    };
    if ( fabs(xd - xc) < 1 ) {
      // the other line is almost horizontal
      long double nx = xd;
      long double ny = (nx - xa)*a+ya;
      return Point (nx, ny);
    }
    else
      {
	// only one horizontal line
	// we know x = xb = xa
	// and finding y is simple...
	long double ny = (xb-xd)*(yd-yc)/(xd-xc);
	return Point(xb, ny);
      };
  }
};
