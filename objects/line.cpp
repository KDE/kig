#include "line.h"

#include "segment.h"
#include "intersection.h"

#include <klocale.h>
#include <kdebug.h>

#include <cmath>

#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a>b)?b:a)
#define SIGN(x) (x < 0 ? -1 : 1)

bool Line::contains(const Coordinate& o, const double fault ) const
{
  if ( !complete ) return false;
  // check your math theory ( homogeneous coördinates ) for this
  double tmp = fabs( o.x * (p1.y-p2.y) + o.y*(p2.x-p1.x) + p1.x*p2.y - p1.y*p2.x );
  return tmp < ( fault * (p2-p1).length());
  // if o is on the line ( if the determinant of the matrix
  //       |---|---|---|
  //       | x | y | z |
  //       |---|---|---|
  //       | x1| y1| z1|
  //       |---|---|---|
  //       | x2| y2| z2|
  //       |---|---|---|
  // equals 0, then p(x,y,z) is on the line containing points
  // p1(x1,y1,z1) and p2 
  // here, we're working with normal coords, no homogeneous ones, so all z's equal 1
}

void Line::draw(KigPainter& p, bool ss) const
{
  p.setPen( ss && selected ? QPen(Qt::red,1) : QPen( Qt::blue, 1 ));
  p.drawLine( p1, p2 );
}

bool Line::inRect(const Rect& p) const
{
    if ( !complete ) return false;
    // TODO: implement for real...
    if ( p.contains( p1 ) || p.contains( p2 ) ) return true;
    return false;
}

Coordinate Line::getPoint(double p) const
{
  // inspired upon KSeg
  p = (p - 0.5) * 2;

  Coordinate m = (p1+p2)/2;
  Coordinate dir = (p1 - p2);
  if (dir.x < 0) dir = -dir;
  // we need to spread the points over the line, it should also come near
  // the (infinite) end of the line, but most points should be near
  // the two points we contain...
  p = p*1.5;
  if (p>0) p = pow(p, 8);
  else p = -pow(p,8);

  return m+dir*p;
};

double Line::getParam(const Coordinate& point) const
{
  // somewhat the reverse of getPoint, although it also supports
  // points not on the line...
  
  // first we project the point onto the line...
  Coordinate pt = calcPointOnPerpend(p1, p2, point);
  pt = calcIntersectionPoint(p1, p2, point, pt);

  // next we fetch the parameter
  Coordinate m = Coordinate(p1+p2)/2;
  Coordinate dir = p1 - p2;
  if (dir.x < 0) dir = -dir;
  Coordinate d = pt-m;

  double p = d.x/dir.x;
  if (p>=0) p = sqrt(sqrt(sqrt(p)));
  else p = -sqrt(sqrt(sqrt(-p)));
  p/=1.5;
  p = p/2 + 0.5;
  return p;
}

LineTTP::~LineTTP()
{
}

QString LineTTP::wantArg( const Object* o) const
{
  if (complete) return 0;
  if ( !toPoint(o)) return 0;
  if ( !pt1 ) return i18n( "Point 1" );
  if ( !pt2 ) return i18n( "Point 2" );
  return 0;
}

bool LineTTP::selectArg(Object* o)
{
  Point* p=toPoint(o);
  assert(p);
  assert (!(pt1 && pt2));
  if ( !pt1 ) pt1 = p;
  else pt2 = p;
  o->addChild(this);
  if (pt2) calc();
  return complete=pt2;
}

void LineTTP::unselectArg(Object* which)
{
  if ( which == pt1 )
    {
      pt1 = pt2; pt2 = 0;
      which->delChild(this);
    }
  if ( which == pt2 )
    {
      pt2 = 0;
      which->delChild(this);
    };
  complete = false;
}

void LineTTP::startMove(const Coordinate& p)
{
  pwwsm = p;
  assert ( pt1 && pt2 );
  pt1->startMove( p );
  pt2->startMove( p );
}

void LineTTP::moveTo(const Coordinate& p)
{
  pt1->moveTo( p );
  pt2->moveTo( p );
}

void LineTTP::stopMove()
{
}

// void LineTTP::cancelMove()
// {
//   p1->cancelMove();
//   p2->cancelMove();
//   calc();
// }

void LineTTP::calc()
{
  p1 = pt1->getCoord();
  p2 = pt2->getCoord();
};

LinePerpend::~LinePerpend()
{
}

QString LinePerpend::wantArg( const Object* o) const
{
  if (toSegment(o) && !segment && !line ) return i18n("On segment");
  if (toLine(o) && !segment && !line ) return i18n("On line");
  if (toPoint(o) && !point) return i18n("Through point");
  return 0;
}

bool LinePerpend::selectArg(Object* o)
{
  Segment* s;
  assert (!(point && (line || segment)));
  if ((s= toSegment(o)))
    segment = s;
  Line* l;
  if ((l = toLine(o)))
    line = l;
  Point* p;
  if ((p = toPoint(o)))
    point = p;
  o->addChild(this);
  if (point && (line || segment)) { complete = true; };
  return complete;
}

// void LinePerpend::unselectArg(Object* o)
// {
//   if ( o == segment) { segment->delChild(this); segment = 0; complete = false; return;};
//   if ( o == line) { line->delChild(this); line = 0; complete = false; return;};
//   if ( o == point) { point->delChild(this); point = 0; complete = false; return;};
// }
void LinePerpend::startMove(const Coordinate&)
{

}
void LinePerpend::moveTo(const Coordinate&)
{

}
void LinePerpend::stopMove()
{

}
void LinePerpend::cancelMove()
{

}

void LinePerpend::calc()
{
  assert (point && (segment || line));
  p1 = point->getCoord();
  Coordinate t1;
  Coordinate t2;
  if (segment)
    {
      t1 = segment->getP1();
      t2 = segment->getP2();
    }
  else
    {
      t1 = line->getP1();
      t2 = line->getP2();
    };
  p2 = calcPointOnPerpend(t1, t2, point->getCoord());
}

Objects LineTTP::getParents() const
{
  Objects objs;
  objs.append( pt1 );
  objs.append( pt2 );
  return objs;
}

Objects LinePerpend::getParents() const
{
  Objects objs;
  if ( segment ) objs.append( segment );
  else objs.append( line );
  objs.append( point );
  return objs;
}

void LineTTP::drawPrelim( KigPainter& p, const Coordinate& pt) const
{
  if (!pt1 || !shown) return;
  p.setPen (QPen (Qt::red,1));
  p.drawLine( pt1->getCoord(), pt );
}

void LinePerpend::drawPrelim( KigPainter& p, const Coordinate& pt) const
{
  if (!(segment || line) || !shown) return;
  p.setPen (QPen (Qt::red,1));
  Coordinate t1, t2;
  if (segment)
    {
      t1 = segment->getP1();
      t2 = segment->getP2();
    }
  else
    {
      t1 = line->getP1();
      t2 = line->getP2();
    };
  
  p.drawLine( pt, calcPointOnPerpend( t1, t2, pt ) );
}

QString LineParallel::wantArg( const Object* o) const
{
  if (toSegment(o) && !segment && !line ) return i18n("On segment");
  if (toLine(o) && !segment && !line ) return i18n("On line");
  if (toPoint(o) && !point) return i18n("Through point");
  return 0;
}

bool LineParallel::selectArg(Object* o)
{
  // we shouldn't be finished yet...
  assert (!(point && (line || segment)));
  // is this a segment ?
  Segment* s;
  if ((s= toSegment(o)))
    {
      assert (!segment);
      segment = s;
    };
  // perhaps a line...
  Line* l;
  if ((l = toLine(o)))
    {
      assert (!line);
      line = l;
    };
  Point* p;
  if ((p = toPoint(o)))
    {
      assert (!point);
      point = p;
    };
  o->addChild(this);
  if (point && (line || segment)) { complete = true; };
  return complete;
}

Objects LineParallel::getParents() const
{
  Objects objs;
  if ( segment ) objs.append( segment );
  else objs.append( line );
  objs.append( point );
  return objs;
}

void LineParallel::drawPrelim( KigPainter& p , const Coordinate& pt) const
{
  if (!(segment || line) || !shown) return;
  p.setPen (QPen (Qt::red,1));
  Coordinate t1, t2;
  if (segment)
    {
      t1 = segment->getP1();
      t2 = segment->getP2();
    }
  else
    {
      t1 = line->getP1();
      t2 = line->getP2();
    };
  p.drawLine( pt, calcPointOnParallel(t1, t2,pt));
}

void LineParallel::calc()
{
  assert (point && (segment || line));
  p1 = point->getCoord();
  Coordinate qpt1, qpt2;
  if (segment)
    {
      qpt1 = segment->getP1();
      qpt2 = segment->getP2();
    }
  else
    {
      qpt1 = line->getP1();
      qpt2 = line->getP2();
    };
  p2 = calcPointOnParallel(qpt1, qpt2, point->getCoord());
}

LineTTP::LineTTP(const LineTTP& l)
  : Line()
{
  pt1 = l.pt1;
  pt1->addChild(this);
  pt2=l.pt2;
  pt2->addChild(this);
  complete = l.complete;
  if(complete) calc();
}
LineParallel::LineParallel(const LineParallel& l)
  : Line()
{
  segment = l.segment;
  if(segment) segment->addChild(this);
  line = l.line;
  if (line) line->addChild(this);
  point = l.point;
  point->addChild(this);
  complete = l.complete;
  if (complete) calc();
}
LinePerpend::LinePerpend(const LinePerpend& l)
  : Line()
{
  segment = l.segment;
  if(segment) segment->addChild(this);
  line = l.line;
  if (line) line->addChild(this);
  point = l.point;
  point->addChild(this);
  complete = l.complete;
  if (complete) calc();
}
