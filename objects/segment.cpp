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

#include "segment.h"

#include "point.h"
#include "../misc/common.h"
#include "../misc/kigpainter.h"

#include <kdebug.h>
#include <qpen.h>
#include "math.h"

#include "../misc/i18n.h"

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
    && (o.x - kigMax(p1->getX(),p2->getX()) < fault ) // not too far to the right
    && ( kigMin (p1->getX(), p2->getX()) - o.x < fault ) // not too far to the left
    && ( kigMin (p1->getY(), p2->getY()) - o.y < fault ) // not too high
    && ( o.y - kigMax (p1->getY(), p2->getY()) < fault ); // not too low
}

void Segment::draw(KigPainter& p, bool ss) const
{
  p.setPen( QPen( selected && ss ? Qt::red : mColor, 1 ));
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
  if ( !o->toPoint() ) return 0;
  if ( !p1 ) return i18n( "Start point" );
  if ( !p2 ) return i18n( "End point" );
  return 0;
}

bool Segment::selectArg(Object* o)
{
  Point* p = o->toPoint();
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
  assert( p1 && p2 );
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

void Segment::calc( const ScreenInfo& )
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
  // if pt is over the end of the segment ( i.e. it's on the line
  // which the segment is a part of, but not of the segment itself..;
  // ) we set it to one of the end points of the segment...
  if ((pt - p1->getCoord()).length() > (p2->getCoord() - p1->getCoord()).length() )
    pt = p2->getCoord();
  else if ( (pt- p2->getCoord()).length() > (p2->getCoord() - p1->getCoord()).length() )
    pt = p1->getCoord();
  if (p2->getCoord() == p1->getCoord()) return 0;
  return ((pt - p1->getCoord()).length())/((p2->getCoord()-p1->getCoord()).length());
}

Objects Segment::getParents() const
{
  Objects objs;
  objs.push_back( p1 );
  objs.push_back( p2 );
  return objs;
}

void Segment::drawPrelim( KigPainter& p, const Object* o ) const
{
  if( !(shown && p1 ) ) return;
  assert( o->toPoint() );
  Coordinate c = o->toPoint()->getCoord();
  p.setPen( QPen( Qt::red, 1));
  p.drawSegment( p1->getCoord(), c );
}

Segment::Segment(const Segment& s)
  : Curve( s ), p1( s.p1 ), p2( s.p2 )
{
  p1->addChild(this);
  p2->addChild(this);
}

Segment* Segment::copy()
{
  return new Segment(*this);
}

Segment* Segment::toSegment()
{
  return this;
}

const Segment* Segment::toSegment() const
{
  return this;
}

const QCString Segment::vBaseTypeName() const
{
  return sBaseTypeName();
}

QCString Segment::sBaseTypeName()
{
  return I18N_NOOP("segment");
}

const QCString Segment::vFullTypeName() const
{
  return sFullTypeName();
}

QCString Segment::sFullTypeName()
{
  return I18N_NOOP("Segment");
}

const QString Segment::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString Segment::sDescriptiveName()
{
  return i18n( "Segment" );
}

const QString Segment::vDescription() const
{
  return sDescription();
}

const QString Segment::sDescription()
{
  return i18n( "A segment constructed from its start and end point" );
}

const QCString Segment::vIconFileName() const
{
  return sIconFileName();
}

const QCString Segment::sIconFileName()
{
  return "segment";
}

const int Segment::vShortCut() const
{
  return sShortCut();
}

const int Segment::sShortCut()
{
  return CTRL+Key_S;
}

QCString Segment::iType() const
{
  return I18N_NOOP("segment");
}

const Coordinate Segment::getP1() const
{
  return p1->getCoord();
}

const Coordinate Segment::getP2() const
{
  return p2->getCoord();
}

Point* Segment::getPoint2()
{
  return p2;
}

Point* Segment::getPoint1()
{
  return p1;
}

const char* Segment::sActionName()
{
  return "objects_new_segment";
}
