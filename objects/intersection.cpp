#include "intersection.h"

#include "segment.h"
#include "circle.h"
#include "line.h"

#include <kdebug.h>

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
  Coordinate t1, t2, t3, t4;
  bool gotfirst = false;
  if ( segment1 )
    {
      if ( gotfirst )
	{
	  t3 = segment1->getP1();
	  t4 = segment1->getP2();
	}
      else
	{
	  t1 = segment1->getP1();
	  t2 = segment1->getP2();
	  gotfirst = true;
	};
    };
  if ( segment2 )
    {
      if ( gotfirst )
	{
	  t3 = segment2->getP1();
	  t4 = segment2->getP2();
	}
      else
	{
	  t1 = segment2->getP1();
	  t2 = segment2->getP2();
	  gotfirst = true;
	};
    };
  if ( line1 )
    {
      if ( gotfirst )
	{
	  t3 = line1->getP1();
	  t4 = line1->getP2();
	}
      else
	{
	  t1 = line1->getP1();
	  t2 = line1->getP2();
	  gotfirst = true;
	};
    };
  if ( line2 )
    {
      if ( gotfirst )
	{
	  t3 = line2->getP1();
	  t4 = line2->getP2();
	}
      else
	{
	  t1 = line2->getP1();
	  t2 = line2->getP2();
	  gotfirst = true;
	};
    };
  mC = calcIntersectionPoint( t1, t2, t3, t4 );
}
