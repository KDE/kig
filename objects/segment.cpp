#include "segment.h"

#include "segment.h"

#include <klocale.h>
#include <kdebug.h>

#include <math.h>

#include "../misc/common.h"

#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a>b)?b:a)

Segment::Segment()
 : p1( 0 ), p2( 0 )
{
}

Segment::~Segment()
{
}

bool Segment::contains(const Coordinate& o, const double fault ) const
{
  double x1 = p1->getCoord().x;
  double y1 = p1->getCoord().y;
  double x2 = p2->getCoord().x;
  double y2 = p2->getCoord().y;
  if ( !p1 || !p2 ) return false;
  // check your math theory ( homogeneous coördinates ) for this
  double tmp = fabs( o.x * (y1-y2) + o.y*(x2-x1) + (x1*y2-y1*x2) );
  return tmp < ( fault * (p2->getCoord()-p1->getCoord()).length()) // if o is on the line ( if the determinant of the matrix
    //       |---|---|---|
    //       | x | y | z |
    //       |---|---|---|
    //       | x1| y1| z1|
    //       |---|---|---|
    //       | x2| y2| z2|
    //       |---|---|---|
    // equals 0, then p(x,y,z) is on the line containing points p1(x1,y1,z1) and p2
    // here, we're working with normal coords, no homogeneous ones, so all z's equal 1
    && (o.x - max(p1->getX(),p2->getX()) < fault ) // not too far to the right
    && ( min (p1->getX(), p2->getX()) - o.x < fault ) // not too far to the left
    && ( min (p1->getY(), p2->getY()) - o.y < fault ) // not too high
    && ( o.y - max (p1->getY(), p2->getY()) < fault ); // not too low
}

void Segment::draw(KigPainter& p, bool showSelection) const
{
  p.setPen( QPen( Qt::blue, 1 ));
  if (showSelection && selected)
    p.setPen( QPen( Qt::red, 1));
  p.drawSegment( p1->getCoord(), p2->getCoord() );
}

bool Segment::inRect(const Rect& p) const
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

void Segment::startMove(const Coordinate& p)
{
    pwwsm = p;
    assert ( p1 && p2 );
    p1->startMove( p );
    p2->startMove( p );
}

void Segment::moveTo(const Coordinate& p)
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
}

Coordinate Segment::getPoint(double param) const
{
  Coordinate dir = p2->getCoord() - p1->getCoord();
  return p1->getCoord() + dir*param;
}

double Segment::getParam(const Coordinate& p) const
{
  Coordinate pt = calcPointOnPerpend(p1->getCoord(), p2->getCoord(), p);
  pt = calcIntersectionPoint(p1->getCoord(), p2->getCoord(), p, pt);
  if ((pt - p1->getCoord()).length() > (p2->getCoord() - p1->getCoord()).length() || (pt- p2->getCoord()).length() > (p2->getCoord() - p1->getCoord()).length()) pt = p2->getCoord();
  if(!contains(pt, false)) kdError() << k_funcinfo << endl;
  if (p2->getCoord() == p1->getCoord()) return 0;
  return ((pt - p1->getCoord()).length())/((p2->getCoord()-p1->getCoord()).length());
}

Objects Segment::getParents() const
{
  Objects objs;
  objs.append( p1 );
  objs.append( p2 );
  return objs;
}

void Segment::drawPrelim( KigPainter& p, const Coordinate& pt) const
{
  if ( !(shown && p1 ) ) return;
  p.setPen( QPen( Qt::red, 1));
  p.drawSegment( p1->getCoord(), pt );
}

Segment::Segment(const Segment& s)
  : Curve()
{
  p1 = s.p1;
  p1->addChild(this);
  p2 = s.p2;
  p2->addChild(this);
  complete = s.complete;
  if (complete) calc();
}
