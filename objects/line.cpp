/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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
#include "point.h"
#include "circle.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "../misc/common.h"

#include <kdebug.h>
#include <qpen.h>

#include <cmath>

bool Line::contains(const Coordinate& o, const ScreenInfo& si ) const
{
  return isOnLine( o, mpa, mpb, si.normalMiss() );
}

void Line::draw(KigPainter& p, bool ss) const
{
  p.setPen( ss && selected ? QPen(Qt::red,1) : QPen( mColor, 1 ));
  p.drawLine( mpa, mpb );
}

bool Line::inRect(const Rect& p) const
{
  // TODO: implement for real...
  if ( p.contains( mpa ) || p.contains( mpb ) ) return true;
  return false;
}

Coordinate Line::getPoint(double p) const
{
  // inspired upon KSeg

  // we need to spread the points over the line, it should also come near
  // the (infinite) end of the line, but most points should be near
  // the two points we contain...
  if ( p <= 0. ) p = 1e-6;
  if ( p >= 1. ) p = 1 - 1e-6;
  p = 2*p - 1;
  if (p > 0) p = p/(1 - p);
    else p = p/(1 + p);
//  p *= 1024;    // such multiplying factor could be useful in order to
                  // have more points near infinity, at the expense of
                  // points near mpa and mpb
  return mpa + p*(mpb - mpa); 
};

double Line::getParam(const Coordinate& point) const
{
  // somewhat the reverse of getPoint, although it also supports
  // points not on the line...

  Coordinate pa = point - mpa;
  Coordinate ba = mpb - mpa;
  double balsq = ba.x*ba.x + ba.y*ba.y;
  assert (balsq > 0);

  double p = (pa.x*ba.x + pa.y*ba.y)/balsq;
//  p /= 1024;
  if (p > 0) p = p/(1+p);
    else p = p/(1-p);

  return 0.5*(p + 1);
}

LineTTP::~LineTTP()
{
}

void LineTTP::startMove(const Coordinate& p, const ScreenInfo& si)
{
  pwwsm = p;
  assert ( pt1 && pt2 );
  pt1->startMove( p, si );
  pt2->startMove( p, si );
}

void LineTTP::moveTo(const Coordinate& p)
{
  pt1->moveTo( p );
  pt2->moveTo( p );
}

void LineTTP::stopMove()
{
}

void LineTTP::calc()
{
  if( !pt1->valid() || !pt2->valid() )
  {
    mvalid = false;
  }
  else
  {
    mvalid = true;
    mpa = pt1->getCoord();
    mpb = pt2->getCoord();
  };
};

LinePerpend::~LinePerpend()
{
}

void LinePerpend::calc()
{
  assert (point && mdir );
  mvalid = true;
  mvalid &= point->valid();
  mpa = point->getCoord();
  Coordinate t1;
  Coordinate t2;

  mvalid &= mdir->valid();
  const Coordinate dir = mdir->direction();

  mpb = calcPointOnPerpend( dir, point->getCoord());
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
  objs.push_back( mdir );
  objs.push_back( point );
  return objs;
}

Objects LineParallel::getParents() const
{
  Objects objs;
  objs.push_back( mdir );
  objs.push_back( point );
  return objs;
}

void LineParallel::calc()
{
  assert (point && mdir );
  mvalid = true;
  mvalid &= point->valid();
  mpa = point->getCoord();
  mvalid &= mdir->valid();
  Coordinate dir = mdir->direction();
  mpb = calcPointOnParallel( dir, mpa );
}

LineTTP::LineTTP(const LineTTP& l)
  : Line( l ), pt1( l.pt1 ), pt2( l.pt2 )
{
  pt1->addChild(this);
  pt2->addChild(this);
}

LineParallel::LineParallel(const LineParallel& l)
  : Line( l ), mdir( l.mdir ), point( l.point )
{
  mdir->addChild(this);
  point->addChild(this);
}

LinePerpend::LinePerpend(const LinePerpend& l)
  : Line( l ), mdir( l.mdir ), point( l.point )
{
  mdir->addChild(this);
  point->addChild(this);
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
  return "objects_new_lineradical";
}

Line::Line( const Line& l )
  : AbstractLine( l ), mpa( l.mpa ), mpb( l.mpb ), pwwsm( l.pwwsm )
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
  : mdir( 0 ), point( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! point ) point = (*i)->toPoint();
    if ( ! mdir ) mdir = (*i)->toAbstractLine();
  };
  assert( point && mdir );
  point->addChild( this );
  mdir->addChild( this );
}

void LinePerpend::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Point* pt = 0;
  AbstractLine* line = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( !pt ) pt = (*i)->toPoint();
    if ( !line ) line = (*i)->toAbstractLine();
  };
  assert( pt && line );

  Coordinate pc = pt->getCoord();

  Coordinate dir = line->direction();
  // now we have what we need, so we draw things...
  p.setPen (QPen (Qt::red,1));
  p.drawLine( pc, calcPointOnPerpend( dir, pc ) );
}

LineParallel::LineParallel( const Objects& o )
 : mdir(0), point(0)
{
  assert( o.size() == 2 );
  for ( Objects::const_iterator i = o.begin(); i != o.end(); ++i )
  {
    if ( ! point ) point = (*i)->toPoint();
    if ( ! mdir ) mdir = (*i)->toAbstractLine();
  };
  assert( point && mdir );
  point->addChild( this );
  mdir->addChild( this );
}

void LineParallel::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Point* q = 0;
  AbstractLine* l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( !q ) q = (*i)->toPoint();
    if ( !l ) l = (*i)->toAbstractLine();
  };
  assert( q && l );
  Coordinate pt = q->getCoord();
  Coordinate dir = l->direction();

  p.setPen( QPen (Qt::red,1) );
  p.drawLine( pt, calcPointOnParallel( dir, pt ) );
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
  uint l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( (*i)->toPoint() ) ++pt;
    else if ( (*i)->toAbstractLine() ) ++l;
    else return NotGood;
  };
  if ( pt > 1 ) return NotGood;
  if ( l > 1 ) return NotGood;
  return size == 2 ? Complete : NotComplete;
}

QString LinePerpend::sUseText( const Objects&, const Object* o )
{
  if ( o->toPoint() ) return i18n( "Perpendicular line through this point" );
  else if ( o->toSegment() ) return i18n( "Line perpendicular on this segment" );
  else if ( o->toLine() ) return i18n( "Line perpendicular on this line" );
  else if ( o->toRay() ) return i18n( "Line perpendicular on this ray" );
  else assert( false );
}

QString LineParallel::sUseText( const Objects&, const Object* o )
{
  if ( o->toPoint() ) return i18n( "Parallel line through this point" );
  else if ( o->toSegment() ) return i18n( "Line parallel on this segment" );
  else if ( o->toLine() ) return i18n( "Line parallel on this line" );
  else if ( o->toRay() ) return i18n( "Line parallel on this ray" );
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

void LineRadical::calc()
{
  Coordinate ce1 = c1->center();
  Coordinate ce2 = c2->center();
  // the radical line is not defined if the centers are the same...
  if( ce1 == ce2 || !c1->valid() || !c2->valid() )
  {
    mvalid = false;
  }
  else
  {
    mvalid = true; // else always defined...
    mpa = calcCircleRadicalStartPoint( ce1, ce2, c1->squareRadius(), c2->squareRadius() );
    mpb = calcPointOnPerpend( LineData( ce1, ce2 ), mpa );
  };
}

void LineRadical::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Circle* ca = os[0]->toCircle();
  Circle* cb = os[1]->toCircle();
  assert ( ca && cb );
  Coordinate pa = ca->center();
  Coordinate pb = cb->center();
  Coordinate s = calcCircleRadicalStartPoint( pa, pb, ca->squareRadius(), cb->squareRadius() );
  p.setPen( QPen (Qt::red,1) );
  p.drawLine( s, calcPointOnPerpend( LineData( pa, pb ), s ) );
}

Object::WantArgsResult LineRadical::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  // we don't want the same circle twice...
  Circle* c = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! (*i)->toCircle() ) return NotGood;
    else if( c == (*i)->toCircle() ) return NotGood;
    else c = (*i)->toCircle();
  };
  return size == 2 ? Complete : NotComplete;
}

QString LineRadical::sUseText( const Objects&, const Object* )
{
  return i18n("Radical Line of this circle");
}

const Coordinate Line::p1() const
{
  return mpa;
}

const Coordinate Line::p2() const
{
  return mpb;
}
