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

Segment::Segment( const Objects& os )
 : mpa( 0 ), mpb( 0 )
{
  assert( os.size() == 2 );
  mpa = os[0]->toPoint();
  mpb = os[1]->toPoint();
  assert( mpa && mpb );
  mpa->addChild( this );
  mpb->addChild( this );
}

Segment::~Segment()
{
}

bool Segment::contains(const Coordinate& o, const ScreenInfo& si ) const
{
  return isOnSegment( o, mpa->getCoord(), mpb->getCoord(), si.normalMiss() );
}

void Segment::draw(KigPainter& p, bool ss) const
{
  p.setPen( QPen( selected && ss ? Qt::red : mColor, 1 ));
  p.drawSegment( mpa->getCoord(), mpb->getCoord() );
}

bool Segment::inRect(const Rect& p) const
{
  // TODO: implement for real...
  if ( mpa->inRect( p ) || mpb->inRect( p ) ) return true;
  return false;
}

Object::WantArgsResult Segment::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  if ( !os[0]->toPoint() || ( size == 2 && !os[1]->toPoint() ) )
      return NotGood;
  return size == 2 ? Complete : NotComplete;
};

QString Segment::sUseText( const Objects& os, const Object* o )
{
  if ( os.size() == 2 || ( os.size() == 1 && o ) ) return i18n( "End point" );
  return i18n( "Start point" );
}

void Segment::startMove(const Coordinate& p, const ScreenInfo& si )
{
  pwwsm = p;
  assert( mpa && mpb );
  mpa->startMove( p, si );
  mpb->startMove( p, si );
}

void Segment::moveTo(const Coordinate& p)
{
  mpa->moveTo( p );
  mpb->moveTo( p );
}

void Segment::stopMove()
{
}

void Segment::calc( const ScreenInfo& )
{
  mvalid = mpa->valid() && mpb->valid();
}

Coordinate Segment::getPoint(double param) const
{
  Coordinate dir = mpb->getCoord() - mpa->getCoord();
  return mpa->getCoord() + dir*param;
}

double Segment::getParam(const Coordinate& p) const
{
  Coordinate pt = calcPointOnPerpend(mpa->getCoord(), mpb->getCoord(), p);
  pt = calcIntersectionPoint(mpa->getCoord(), mpb->getCoord(), p, pt);
  // if pt is over the end of the segment ( i.e. it's on the line
  // which the segment is a part of, but not of the segment itself..;
  // ) we set it to one of the end points of the segment...
  if ((pt - mpa->getCoord()).length() > (mpb->getCoord() - mpa->getCoord()).length() )
    pt = mpb->getCoord();
  else if ( (pt- mpb->getCoord()).length() > (mpb->getCoord() - mpa->getCoord()).length() )
    pt = mpa->getCoord();
  if (mpb->getCoord() == mpa->getCoord()) return 0;
  return ((pt - mpa->getCoord()).length())/((mpb->getCoord()-mpa->getCoord()).length());
}

Objects Segment::getParents() const
{
  Objects objs;
  objs.push_back( mpa );
  objs.push_back( mpb );
  return objs;
}

void Segment::sDrawPrelim( KigPainter& p, const Objects& os )
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
  mpa->addChild(this);
  mpb->addChild(this);
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

const char* Segment::sActionName()
{
  return "objects_new_segment";
}

const Coordinate Segment::p1() const
{
  return mpa->getCoord();
}

const Coordinate Segment::p2() const
{
  return mpb->getCoord();
}

const uint Segment::numberOfProperties()
{
  return AbstractLine::numberOfProperties() + 3;
}

const Property Segment::property( uint which )
{
  if ( which < AbstractLine::numberOfProperties() )
    return AbstractLine::property( which );
  if ( which == AbstractLine::numberOfProperties() )
  {
    return Property( ( p2() - p1() ).length() );
  }
  else if ( which == AbstractLine::numberOfProperties() + 1 )
  {
    return Property( p1() );
  }
  else if ( which == AbstractLine::numberOfProperties() + 2 )
  {
    return Property( p2() );
  }
  else assert( false );
}

const QStringList Segment::properties()
{
  QStringList s = AbstractLine::properties();
  s << "Length";
  s << "Start point";
  s << "End point";
  assert( s.size() == Segment::numberOfProperties() );
  return s;
}
