/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#include "line.h"
#include "segment.h"
#include "point.h"
#include "circle.h"
#include "../misc/kigpainter.h"

#include <kdebug.h>
#include <qpen.h>

#include <cmath>

#include "../misc/i18n.h"

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
  p.setPen( ss && selected ? QPen(Qt::red,1) : QPen( mColor, 1 ));
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
  if ( ! o->toPoint()) return 0;
  if ( !pt1 ) return i18n( "Point 1" );
  if ( !pt2 ) return i18n( "Point 2" );
  return 0;
}

bool LineTTP::selectArg(Object* o)
{
  Point* p= o->toPoint();
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
  if( !pt1->getValid() || !pt2->getValid() )
    {
      valid = false;
      return;
    };
  p1 = pt1->getCoord();
  p2 = pt2->getCoord();
};

LinePerpend::~LinePerpend()
{
}

QString LinePerpend::wantArg( const Object* o) const
{
  if (o->toSegment() && !segment && !line ) return i18n("On segment");
  if (o->toLine() && !segment && !line ) return i18n("On line");
  if (o->toPoint() && !point ) return i18n("Through point");
  return 0;
}

bool LinePerpend::selectArg(Object* o)
{
  Segment* s;
  assert (!(point && (line || segment)));
  if ((s= o->toSegment()))
    segment = s;
  Line* l;
  if ((l = o->toLine()))
    line = l;
  Point* p;
  if ((p = o->toPoint()))
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
  objs.push_back( pt1 );
  objs.push_back( pt2 );
  return objs;
}

Objects LinePerpend::getParents() const
{
  Objects objs;
  if ( segment ) objs.push_back( segment );
  else objs.push_back( line );
  objs.push_back( point );
  return objs;
}

void LineTTP::drawPrelim( KigPainter& p, const Object* arg ) const
{
  if (!pt1 || !shown) return;
  assert( arg->toPoint() );
  Coordinate pt = arg->toPoint()->getCoord();
  p.setPen (QPen (Qt::red,1));
  p.drawLine( pt1->getCoord(), pt );
}

void LinePerpend::drawPrelim( KigPainter& p, const Object* arg ) const
{
  // we need a point, and a line or segment, and we can get it from
  // args we selected already + our own argument arg...
  // first a point:
  Coordinate tpoint;
  if ( point ) tpoint = point->toPoint()->getCoord();
  else
  {
    // arg should be a point...
    if ( !arg->toPoint() ) return;
    tpoint = arg->toPoint()->getCoord();
  };
  // next a line or segment... --> these both just give us two
  // coords...
  Coordinate t1, t2;
  // do we have a line or segment selected already ?
  if ( line )
  {
    t1 = line->getP1();
    t2 = line->getP2();
  }
  else if ( segment )
  {
    t1 = line->getP1();
    t2 = line->getP2();
  }
  else if ( arg->toLine() )
  {
    t1 = arg->toLine()->getP1();
    t2 = arg->toLine()->getP2();
  }
  else if ( arg->toSegment() )
  {
    t1 = arg->toSegment()->getP1();
    t2 = arg->toSegment()->getP2();
  }
  else return; // not enough args...

  // now we have what we need, so we draw things...
  p.setPen (QPen (Qt::red,1));
  p.drawLine( tpoint, calcPointOnPerpend( t1, t2, tpoint ) );
}

QString LineParallel::wantArg( const Object* o) const
{
  if (o->toSegment() && !segment && !line ) return i18n("On segment");
  if (o->toLine() && !segment && !line ) return i18n("On line");
  if (o->toPoint() && !point ) return i18n("Through point");
  return 0;
}

bool LineParallel::selectArg(Object* o)
{
  // we shouldn't be finished yet...
  assert (!(point && (line || segment)));
  // is this a segment ?
  Segment* s;
  if ((s= o->toSegment()))
    {
      assert (!segment);
      segment = s;
    };
  // perhaps a line...
  Line* l;
  if ((l = o->toLine()))
    {
      assert (!line);
      line = l;
    };
  // or even a point...
  Point* p;
  if ((p = o->toPoint()))
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
  if ( segment ) objs.push_back( segment );
  else objs.push_back( line );
  objs.push_back( point );
  return objs;
}

void LineParallel::drawPrelim( KigPainter& p , const Object* arg ) const
{
  // we need a point, and a line or segment, and we can get it from
  // args we selected already + our own argument arg...
  // first a point:
  Coordinate tpoint;
  if ( point ) tpoint = point->toPoint()->getCoord();
  else
  {
    // arg should be a point...
    if ( !arg->toPoint() ) return;
    tpoint = arg->toPoint()->getCoord();
  };
  // next a line or segment... --> these both just give us two
  // coords...
  Coordinate t1, t2;
  // do we have a line or segment selected already ?
  if ( line )
  {
    t1 = line->getP1();
    t2 = line->getP2();
  }
  else if ( segment )
  {
    t1 = line->getP1();
    t2 = line->getP2();
  }
  else if ( arg->toLine() )
  {
    t1 = arg->toLine()->getP1();
    t2 = arg->toLine()->getP2();
  }
  else if ( arg->toSegment() )
  {
    t1 = arg->toSegment()->getP1();
    t2 = arg->toSegment()->getP2();
  }
  else return; // not enough args...

  // now we have what we need, so we draw things...
  p.setPen (QPen (Qt::red,1));
  p.drawLine( tpoint, calcPointOnParallel( t1, t2, tpoint ) );
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

Objects LineRadical::getParents() const
{
  Objects objs;
  if( c1 ) objs.push_back( c1 );
  if( c2 ) objs.push_back( c2 );
  return objs;
}

LineRadical::LineRadical(const LineRadical& l)
  : Line()
{
  c1 = l.c1;
  if( c1 ) c1->addChild( this );
  c2 = l.c2;
  if( c2 ) c2->addChild( this );
  complete = l.complete;
  calc();
}

QString LineRadical::wantArg( const Object* o ) const
{
  if( o->toCircle() && ( !c1 || !c2 ) ) return i18n("Radical Line of this circle");
  return QString::null;
}

bool LineRadical::selectArg( Object* o )
{
  assert( !c2 );
  Circle* c = o->toCircle();
  assert( c );
  if( !c1 ) c1 = c; else c2 = c;
  o->addChild( this );
  complete = c2;
  if( complete ) calc();
  return complete;
}

void LineRadical::drawPrelim( KigPainter&, const Object* ) const
{
  return;
}

void LineRadical::calc()
{
  if( !c1 && !c2 )
    {
      assert( complete == false );
      return;
    };

  Coordinate ce1, ce2, direc, startpoint;
  double r1sq, r2sq, dsq, lambda;

  ce1 = c1->getCenter();
  ce2 = c2->getCenter();
  // the radical line is not defined if the centers are the same...
  if( ce1 == ce2 || !c1->getValid() || !c2->getValid() )
    {
      valid = false;
      return;
    }
  else valid = true; // else always defined...

  r1sq = c1->getRadius();
  r1sq = r1sq * r1sq;
  r2sq = c2->getRadius();
  r2sq = r2sq * r2sq;

  direc = ce2 - ce1;
  startpoint = (ce1 + ce2)/2;

  dsq = direc.squareLength();
  if (dsq == 0)
    {
      lambda = 0.0;
    }
  else
    {
      lambda = (r1sq - r2sq) / dsq / 2;
    }
  direc *= lambda;
  startpoint = startpoint + direc;
  //  startCoords.coords = startpoint;
  p1 = startpoint;
  //  endCoords.coords = startpoint + direc.orthogonal();
  p2 = startpoint + direc.orthogonal();
}

LineRadical::LineRadical()
  : c1( 0 ), c2( 0 )
{
}
Line* Line::toLine()
{
  return this;
}
const Line* Line::toLine() const
{
  return this;
}

const QCString Line::sBaseTypeName()
{
  return I18N_NOOP("line");
}

const QString LineTTP::sDescriptiveName()
{
  return i18n("Line by two points");
}

const QString LineTTP::sDescription()
{
  return i18n( "A line constructed through two points" );
}

const QString LinePerpend::sDescriptiveName()
{
  return i18n("Perpendicular");
}

QString LinePerpend::sDescription()
{
  return i18n( "A line constructed through a point, perpendicular on another line or segment." );
}

const QString LineParallel::sDescriptiveName()
{
  return i18n("Parallel");
}

const QString LineParallel::sDescription()
{
  return i18n( "A line constructed through a point, and parallel to another line or segment" );
}

const QString LineRadical::sDescriptiveName()
{
  return i18n("Radical Line");
}

const QString LineRadical::sDescription()
{
  return i18n( "A line constructed through the intersections of two circles.  This is also defined for non-intersecting circles..." );
}

