#include "segment.h"

#include "segment.h"

#include <klocale.h>
#include <kdebug.h>

#include <math.h>

#include "line.h"
#include "intersection.h"

#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a>b)?b:a)

Segment::Segment()
 : p1( 0 ), p2( 0 )
{
}

Segment::~Segment()
{
}

bool Segment::contains(const QPoint& o, bool strict) const
{
    if ( !p1 || !p2 ) return false;
    // check your math theory ( homogeneous coördinates ) for this
    double tmp = fabs( o.x() * y1_y2 + o.y()*x2_x1 + x1y2_y1x2 );
    return tmp < (( strict ? 4 : 2 )* (*p2-*p1).length()) // if o is on the line ( if the determinant of the matrix
      //       |---|---|---|
      //       | x | y | z |
      //       |---|---|---|
      //       | x1| y1| z1|
      //       |---|---|---|
      //       | x2| y2| z2|
      //       |---|---|---|
      // equals 0, then p(x,y,z) is on the line containing points p1(x1,y1,z1) and p2
      // here, we're working with normal coords, no homogeneous ones, so all z's equal 1
      && (o.x() - max(p1->getX(),p2->getX()) < (strict ? 3:1)) // not too far to the right
      && ( min (p1->getX(), p2->getX()) - o.x() < (strict ? 3:1)) // not too far to the left
      && ( min (p1->getY(), p2->getY()) - o.y() < (strict ? 3:1)) // not too high
      && ( o.y() - max (p1->getY(), p2->getY()) < (strict?3:1)); // not too low
}

void Segment::draw(QPainter& p, bool showSelection) const
{
  p.setPen( QPen( Qt::blue, 1 ));
  if (showSelection && selected)
    p.setPen( QPen( Qt::red, 1));
  p.drawLine( p1->getX(), p1->getY(), p2->getX(), p2->getY() );
}

bool Segment::inRect(const QRect& p) const
{
    if ( !p1 || !p2 ) return false;
    // TODO: implement for real...
    if ( p1->inRect( p ) || p2->inRect( p ) ) return true;
    return false;
}

QString Segment::wantArg( const Object* o) const
{
  if (complete) return 0;
  if ( !toPoint(o) ) return 0;
  if ( !p1 ) return i18n( "Start point" );
  if ( !p2 ) return i18n( "End point" );
  return 0;
}

bool Segment::selectArg(Object* o)
{
  Point* p = toPoint(o);
  assert(p);
  assert (! (p1 && p2));
  if ( !p1 ) p1 = p;
  else p2 = p;
  o->addChild(this);
  return complete=p2;
}

void Segment::unselectArg(Object* which)
{
    if ( which == p1 )
      {
	p1 = p2; p2 = 0;
	which->delChild(this);
      }
    if ( which == p2 )
      {
	p2 = 0;
	which->delChild(this);
      };
}

void Segment::startMove(const QPoint& p)
{
    pwwsm = p;
    assert ( p1 && p2 );
    p1->startMove( p );
    p2->startMove( p );
}

void Segment::moveTo(const QPoint& p)
{
    p1->moveTo( p );
    p2->moveTo( p );
}

void Segment::stopMove()
{
}

// void Segment::cancelMove()
// {
//     p1->cancelMove();
//     p2->cancelMove();
// }

void Segment::calc()
{
  calcVars();
}

Point Segment::getPoint(double param) const
{
  Point dir = *p2 - *p1;
  return *p1 + dir*param;
}

double Segment::getParam(const Point& p) const
{
  Point pt = LinePerpend::calcPointOnPerpend(*p1, *p2, p);
  pt = IntersectionPoint::calc(*p1, *p2, p, pt);
  if ((pt - *p1).length() > (*p2 - *p1).length() || (pt- *p2).length() > (*p2 - *p1).length()) pt = *p2;
  if(!contains(pt.toQPoint(), false)) kdError() << k_funcinfo << endl;
  if (*p2 == *p1) return 0;
  return ((pt - *p1).length())/((*p2-*p1).length());
}

void Segment::calcVars()
{
  x1y2_y1x2 = p1->getX()*p2->getY() - p1->getY()*p2->getX();
  y1_y2 = p1->getY() - p2->getY();
  x2_x1 = p2->getX() - p1->getX();
}

Objects Segment::getParents() const
{
  Objects objs;
  objs.append( p1 );
  objs.append( p2 );
  return objs;
}

void Segment::drawPrelim( QPainter& p, const QPoint& pt) const
{
  if ( !shown ) return;
  if (!p1) return;
  p.setPen( QPen( Qt::red, 1));
  p.drawLine( p1->getX(), p1->getY(), pt.x(), pt.y() );
}

void Segment::segmentOverlay(const Point& p1, const Point& p2, QPtrList<QRect>& list, const QRect& border)
{
  // this code is inspired by what Marc Bartsch wrote in his KGeo
  // some stuff we may need:
  
  Point p3 = p2 - p1;
  double length = p3.length();
  // if length is smaller than one, we risk getting a divide by zero
  length = max(length, 1);
  p3 *= overlayRectSize();
  p3 /= length;
  // these are correct...
//   kdDebug() << "p3.x(): " << p3.getX() << "  p3.y(): " << p3.getY() << endl;

  int size = overlayRectSize();
  QRect lastRect(0,0,size, size);
  lastRect.moveCenter(p2.toQPoint());

  int counter = 0;

  QRect r(p1.toQPoint(), p2.toQPoint());
  r = r.normalize();

  QRect* appel;
  if (p3 != Point(0,0)) {
    do {
      appel = new QRect(0,0,size,size);
      Point tmp = p1+p3*counter;
      appel->moveCenter(tmp.toQPoint());
      // there used to be a bug here :(
//       if (counter == 0) {
// 	kdDebug() << "appel: tl.x: " << appel->topLeft().x() << " y: " << appel->topLeft().y() << " br.x: " << appel->bottomRight().x() << " y: " << appel->bottomRight().y() << endl;
//       };
      //QPoint(p1.getX() + (p3.getX() * counter), p1.getY() + (p3.getY() * counter)));
      if (!appel->intersects(r)) {
	delete appel;
// 	kdDebug()<< "stopped after "<< counter << " passes." << endl;
	break;
      }
      if (appel->intersects(border)) list.append(appel); else delete appel;
      ++counter;
      // preventing disasters...
      if (counter > 100) { kdError()<< k_funcinfo << "counter got too big :( " << endl; break; }
    }
    while (true);
  };
  list.append(new QRect(lastRect));
}

void Segment::getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& pt) const
{
  if (!(shown  && p1)) return;
  segmentOverlay(*p1, pt, list, border);
}
