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
  // here, we're working with normal coords, no homogeneous ones, so
  // all z's equal 1
}

void Line::draw(KigPainter& p, bool ss) const
{
  p.setPen( ss && selected ? QPen(Qt::red,1) : QPen( mColor, 1 ));
  p.drawLine( p1, p2 );
}

bool Line::inRect(const Rect& p) const
{
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

void LineTTP::calc( const ScreenInfo& )
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

void LinePerpend::startMove(const Coordinate&)
{
}

void LinePerpend::moveTo(const Coordinate&)
{

}
void LinePerpend::stopMove()
{
}

void LinePerpend::calc( const ScreenInfo& )
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

Objects LineParallel::getParents() const
{
  Objects objs;
  if ( segment ) objs.push_back( segment );
  else objs.push_back( line );
  objs.push_back( point );
  return objs;
}

void LineParallel::calc( const ScreenInfo& )
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
  : Line( l ), pt1( l.pt1 ), pt2( l.pt2 )
{
  pt1->addChild(this);
  pt2->addChild(this);
}

LineParallel::LineParallel(const LineParallel& l)
  : Line( l ), segment( l.segment ), line( l.line ), point( l.point )
{
  if( segment ) segment->addChild(this);
  if( line ) line->addChild(this);
  if( point ) point->addChild(this);
}

LinePerpend::LinePerpend(const LinePerpend& l)
  : Line( l ), segment( l.segment ), line( l.line ), point( l.point )
{
  if( segment ) segment->addChild(this);
  if( line ) line->addChild(this);
  if( point ) point->addChild(this);
}

Objects LineRadical::getParents() const
{
  Objects objs;
  if( c1 ) objs.push_back( c1 );
  if( c2 ) objs.push_back( c2 );
  return objs;
}

LineRadical::LineRadical(const LineRadical& l)
  : Line( l ), c1( l.c1 ), c2( l.c2 )
{
  if( c1 ) c1->addChild( this );
  if( c2 ) c2->addChild( this );
}

void LineRadical::calc( const ScreenInfo& )
{
  if( !c1 && !c2 ) return;

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

const char* LineTTP::sActionName()
{
  return "objects_new_linettp";
}

const char* LinePerpend::sActionName()
{
  return "objects_new_lineperpend";
}

const char* LineParallel::sActionName()
{
  return "objects_new_lineparallel";
}

const char* LineRadical::sActionName()
{
  return "objects_new_radicalline";
}

Line::Line( const Line& l )
  : Curve( l ), p1( l.p1 ), p2( l.p2 ), pwwsm( l.pwwsm )
{
}

const QCString Line::vBaseTypeName() const
{
  return sBaseTypeName();
}

LineTTP::LineTTP( const Objects& os )
 : pt1(0), pt2(0)
{
  assert( os.size() == 2 );
  pt1 = os[0]->toPoint();
  pt2 = os[1]->toPoint();
  assert( pt1 && pt2 );
  pt1->addChild( this );
  pt2->addChild( this );
}

Object::WantArgsResult LineTTP::sWantArgs( const Objects& os )
{
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toPoint() ) return NotGood;
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  return size == 2 ? Complete : NotComplete;
}

QString LineTTP::sUseText( const Objects& os, const Object* )
{
  uint size = os.size();
  return size == 1 ? i18n( "Last Point" ) : i18n( "First Point" );
}

void LineTTP::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  assert( os[0]->toPoint() && os[1]->toPoint() );
  Coordinate a = os[0]->toPoint()->getCoord();
  Coordinate b = os[1]->toPoint()->getCoord();
  p.setPen (QPen (Qt::red,1));
  p.drawLine( a, b );
}

LinePerpend::LinePerpend( const Objects& os )
 : segment(0), line(0), point(0)
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! point ) point = (*i)->toPoint();
    if ( ! segment ) segment = (*i)->toSegment();
    if ( ! line ) line = (*i)->toLine();
  };
  assert( point && ( segment || line ) );
  point->addChild( this );
  if ( segment ) segment->addChild( this );
  if ( line ) line->addChild( this );
}

void LinePerpend::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Point* pt = 0;
  Segment* s = 0;
  Line* l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( !pt ) pt = (*i)->toPoint();
    if ( !s ) s = (*i)->toSegment();
    if ( !l ) l = (*i)->toLine();
  };
  assert( pt && ( s || l ) );

  Coordinate pc = pt->getCoord();

  // next a line or segment... --> these both just give us two
  // coords...
  Coordinate t1, t2;
  // do we have a line or segment selected already ?
  if ( l )
  {
    t1 = l->getP1();
    t2 = l->getP2();
  }
  else
  {
    t1 = s->getP1();
    t2 = s->getP2();
  }

  // now we have what we need, so we draw things...
  p.setPen (QPen (Qt::red,1));
  p.drawLine( pc, calcPointOnPerpend( t1, t2, pc ) );
}

LineParallel::LineParallel( const Objects& o )
 : segment(0), line(0), point(0)
{
  assert( o.size() == 2 );
  for ( Objects::const_iterator i = o.begin(); i != o.end(); ++i )
  {
    if ( ! point ) point = (*i)->toPoint();
    if ( ! segment ) segment = (*i)->toSegment();
    if ( ! line ) line = (*i)->toLine();
  };
  assert( point && ( segment || line ) );
  point->addChild( this );
  if ( segment ) segment->addChild( this );
  if ( line ) line->addChild( this );
}

void LineParallel::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Point* q = 0;
  Segment* s = 0;
  Line* l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( !q ) q = (*i)->toPoint();
    if ( !s ) s = (*i)->toSegment();
    if ( !l ) l = (*i)->toLine();
  };
  assert( q && ( s || l ) );
  Coordinate pt = q->getCoord();
  Coordinate pa, pb;
  if ( l )
  {
    pa = l->getP1();
    pb = l->getP2();
  }
  else
  {
    pa = s->getP1();
    pb = s->getP2();
  }

  p.setPen( QPen (Qt::red,1) );
  p.drawLine( pt, calcPointOnParallel( pa, pb, pt ) );
}

Object::WantArgsResult LinePerpend::sWantArgs( const Objects& os )
{
  // i know i'm lazy, but lineperpend needs exactly the same as
  // LineParallel...
  return LineParallel::sWantArgs( os );
};

Object::WantArgsResult LineParallel::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  uint pt = 0;
  uint s = 0;
  uint l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( (*i)->toPoint() ) ++pt;
    else if ( (*i)->toSegment() ) ++s;
    else if ( (*i)->toLine() ) ++l;
    else return NotGood;
  };
  if ( pt > 1 ) return NotGood;
  if ( s + l > 1 ) return NotGood;
  return size == 2 ? Complete : NotComplete;
}

QString LinePerpend::sUseText( const Objects&, const Object* o )
{
  if ( o->toPoint() ) return i18n( "Perpendicular line through this point" );
  else if ( o->toSegment() ) return i18n( "Line perpendicular on this segment" );
  else if ( o->toLine() ) return i18n( "Line perpendicular on this line" );
  else assert( false );
}

QString LineParallel::sUseText( const Objects&, const Object* o )
{
  if ( o->toPoint() ) return i18n( "Parallel line through this point" );
  else if ( o->toSegment() ) return i18n( "Line parallel on this segment" );
  else if ( o->toLine() ) return i18n( "Line parallel on this line" );
  else assert( false );
}

LineRadical* LineRadical::copy()
{
  return new LineRadical (*this);
}

LineRadical::LineRadical( const Objects& os )
{
  assert( os.size() == 2 );
  c1 = os[0]->toCircle();
  c2 = os[1]->toCircle();
  assert( c1 && c2 );
  c1->addChild( this );
  c2->addChild( this );
}

void LineRadical::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Circle* ca = os[0]->toCircle();
  Circle* cb = os[1]->toCircle();
  assert ( ca && cb );
  Coordinate pa = ca->getCenter();
  Coordinate pb = cb->getCenter();
  Coordinate m = ( pa + pb ) / 2;
  p.setPen( QPen (Qt::red,1) );
  p.drawLine( m, calcPointOnPerpend( pa, pb, m ) );
}

Object::WantArgsResult LineRadical::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toCircle() ) return NotGood;
  return size == 2 ? Complete : NotComplete;
}

QString LineRadical::sUseText( const Objects&, const Object* )
{
  return i18n("Radical Line of this circle");
}
