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

#include "segment.h"

#include "point.h"
#include "../misc/common.h"
#include "../misc/kigpainter.h"

#include <kdebug.h>
#include <qpen.h>

#include "../misc/i18n.h"

SegmentAB::SegmentAB( const Objects& os )
 : pt1( 0 ), pt2( 0 )
{
  assert( os.size() == 2 );
  pt1 = os[0]->toPoint();
  pt2 = os[1]->toPoint();
  assert( pt1 && pt2 );
  pt1->addChild( this );
  pt2->addChild( this );
}

SegmentAB::~SegmentAB()
{
}

bool Segment::contains(const Coordinate& o, const ScreenInfo& si ) const
{
  return isOnSegment( o, mpa, mpb, si.normalMiss() );
}

void Segment::draw(KigPainter& p, bool ss) const
{
  p.setPen( QPen( selected && ss ? Qt::red : mColor, 1 ));
  p.drawSegment( mpa, mpb );
}

bool Segment::inRect(const Rect& p) const
{
  // TODO: implement for real...
//  if ( mpa->inRect( p ) || mpb->inRect( p ) ) return true;
  if ( p.contains( mpa ) || p.contains( mpb ) ) return true;
  return false;
}

Object::WantArgsResult SegmentAB::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  if ( !os[0]->toPoint() || ( size == 2 && !os[1]->toPoint() ) )
      return NotGood;
  return size == 2 ? Complete : NotComplete;
};

QString SegmentAB::sUseText( const Objects& os, const Object* o )
{
  if ( os.size() == 2 || ( os.size() == 1 && o ) ) return i18n( "End point" );
  return i18n( "Start point" );
}

void SegmentAB::startMove(const Coordinate& p, const ScreenInfo& si )
{
  pwwsm = p;
  assert( pt1 && pt2 );
  pt1->startMove( p, si );
  pt2->startMove( p, si );
}

void SegmentAB::moveTo(const Coordinate& p)
{
  pt1->moveTo( p );
  pt2->moveTo( p );
}

void SegmentAB::stopMove()
{
}

void SegmentAB::calc()
{
  mvalid = pt1->valid() && pt2->valid();
  mpa = pt1->getCoord();
  mpb = pt2->getCoord();
}

Coordinate Segment::getPoint(double param) const
{
  Coordinate dir = mpb - mpa;
  return mpa + dir*param;
}

double Segment::getParam(const Coordinate& p) const
{
  Coordinate pa = p - mpa;
  Coordinate ba = mpb - mpa;
  double balsq = ba.x*ba.x + ba.y*ba.y;
  assert (balsq > 0);

  double t = (pa.x*ba.x + pa.y*ba.y)/balsq;
  if ( t < 0 ) return 0.;
  if ( t > 1 ) return 1.;
  return t;
}

Objects SegmentAB::getParents() const
{
  Objects objs;
  objs.push_back( pt1 );
  objs.push_back( pt2 );
  return objs;
}

void SegmentAB::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  assert( os[0]->toPoint() && os[1]->toPoint() );
  Coordinate a = os[0]->toPoint()->getCoord();
  Coordinate b = os[1]->toPoint()->getCoord();
  p.setPen( QPen( Qt::red, 1));
  p.drawSegment( a, b );
}

Segment::Segment(const Segment& s)
  : AbstractLine( s ), mpa( s.mpa ), mpb( s.mpb )
{
}

SegmentAB::SegmentAB(const SegmentAB& s)
  : Segment( s ), pt1( s.pt1 ), pt2( s.pt2 )
{
  pt1->addChild(this);
  pt2->addChild(this);
}

const QCString Segment::vBaseTypeName() const
{
  return sBaseTypeName();
}

QCString Segment::sBaseTypeName()
{
  return I18N_NOOP("Segment");
}

const QCString SegmentAB::vFullTypeName() const
{
  return sFullTypeName();
}

QCString SegmentAB::sFullTypeName()
{
  return I18N_NOOP("Segment");
}

const QString SegmentAB::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString SegmentAB::sDescriptiveName()
{
  return i18n( "Segment" );
}

const QString SegmentAB::vDescription() const
{
  return sDescription();
}

const QString SegmentAB::sDescription()
{
  return i18n( "A segment constructed from its start and end point" );
}

const QCString SegmentAB::vIconFileName() const
{
  return sIconFileName();
}

const QCString SegmentAB::sIconFileName()
{
  return "segment";
}

const int SegmentAB::vShortCut() const
{
  return sShortCut();
}

const int SegmentAB::sShortCut()
{
  return CTRL+Key_S;
}

const char* SegmentAB::sActionName()
{
  return "objects_new_segment";
}

const Coordinate Segment::p1() const
{
  return mpa;
}

const Coordinate Segment::p2() const
{
  return mpb;
}

const uint Segment::numberOfProperties() const
{
  return AbstractLine::numberOfProperties() + 1;
}

const Property Segment::property( uint which, const KigWidget& w ) const
{
  if ( which < AbstractLine::numberOfProperties() )
    return AbstractLine::property( which, w );
  if ( which == AbstractLine::numberOfProperties() )
  {
    return Property( ( p2() - p1() ).length() );
  }
  else assert( false );
}

const QCStringList Segment::properties() const
{
  QCStringList s = AbstractLine::properties();
  s << I18N_NOOP( "Length" );
  assert( s.size() == Segment::numberOfProperties() );
  return s;
}

bool Segment::isa( int type ) const
{
  return type == SegmentT ? true : Parent::isa( type );
}
