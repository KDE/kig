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

#include "ray.h"

#include "point.h"

#include "../misc/i18n.h"
#include "../misc/common.h"
#include "../misc/kigpainter.h"

#include <kdebug.h>
#include <qpen.h>
#include <math.h>

RayAB::RayAB( const Objects& os )
  : pt1( 0 ), pt2( 0 )
{
  assert( os.size() == 2 );
  pt1 = os[0]->toPoint();
  pt2 = os[1]->toPoint();
  assert( pt1 && pt2 );
  pt1->addChild( this );
  pt2->addChild( this );
}

RayAB::RayAB(const RayAB& s)
  : Ray( s ), pt1( s.pt1 ), pt2( s.pt2 )
{
  pt1->addChild(this);
  pt2->addChild(this);
}

RayAB::~RayAB()
{
}

bool Ray::contains(const Coordinate& o, const ScreenInfo& si ) const
{
  return isOnRay( o, mpa, mpb, si.normalMiss() );
}

void Ray::draw(KigPainter& p, bool ss) const
{
  p.setPen( QPen( selected && ss ? Qt::red : mColor, 1 ));
  p.setWidth ( mWidth );
  p.drawRay( mpa, mpb );
}

bool Ray::inRect(const Rect& p) const
{
  // TODO: implement for real...
//  if ( mpa->inRect( p ) || mpb->inRect( p ) ) return true;
  if ( p.contains( mpa ) || p.contains( mpb ) ) return true;
  return false;
}

Object::WantArgsResult RayAB::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  if ( !os[0]->toPoint() || ( size == 2 && !os[1]->toPoint() ) )
      return NotGood;
  return size == 2 ? Complete : NotComplete;
};

QString RayAB::sUseText( const Objects& os, const Object* o )
{
  if ( os.size() == 2 || ( os.size() == 1 && o ) ) return i18n( "Ray through this point" );
  return i18n( "Ray starting at this point" );
}

void RayAB::startMove(const Coordinate& p, const ScreenInfo& si )
{
  pwwsm = p;
  assert( pt1 && pt2 );
  pt1->startMove( p, si );
  pt2->startMove( p, si );
}

void RayAB::moveTo(const Coordinate& p)
{
  pt1->moveTo( p );
  pt2->moveTo( p );
}

void RayAB::stopMove()
{
}

void RayAB::calc()
{
  mvalid = pt1->valid() && pt2->valid();
  mpa = pt1->getCoord();
  mpb = pt2->getCoord();
}

Coordinate Ray::getPoint(double param) const
{
  Coordinate dir = mpb - mpa;
  param = 1.0/param - 1.0;
  return mpa + dir*param;
}

double Ray::getParam(const Coordinate& p) const
{
//  const LineData ld = lineData();
//  Coordinate pt = calcPointOnPerpend( ld, p );
//  pt = calcIntersectionPoint( ld, LineData( p, pt ));
//  // if pt is over the end of the ray ( i.e. it's on the line
//  // which the ray is a part of, but not of the ray itself..;
//  // ) we set it to the start point of the ray...
//  Coordinate dir = ld.b - ld.a;
//  pt -= ld.a;
  Coordinate pa = p - mpa;
  Coordinate ba = mpb - mpa;
  double balsq = ba.x*ba.x + ba.y*ba.y;
  assert (balsq > 0);

  double param = (pa.x*ba.x + pa.y*ba.y)/balsq;

//  double param;
//  if ( dir.x != 0 ) param = pt.x / dir.x;
//  else if ( dir.y != 0 ) param = pt.y / dir.y;
//  else param = 0.;
  if ( param < 0. ) param = 0.;

  // mp:  let's try with 1/(x+1),  this reverses the mapping, but
  // should allow to take advantage of the tightness of floating point
  // numbers near zero, in order to get more possible positions near
  // infinity

  param = 1./( param + 1. );

  assert( param >= 0. && param <= 1. );
  return param;
}

Objects RayAB::getParents() const
{
  Objects objs;
  objs.push_back( pt1 );
  objs.push_back( pt2 );
  return objs;
}

void RayAB::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  assert( os[0]->toPoint() && os[1]->toPoint() );
  Coordinate a = os[0]->toPoint()->getCoord();
  Coordinate b = os[1]->toPoint()->getCoord();
  p.setPen( QPen( Qt::red, 1));
  p.drawRay( a, b );
}

const QCString Ray::vBaseTypeName() const
{
  return sBaseTypeName();
}

QCString Ray::sBaseTypeName()
{
  return I18N_NOOP("Ray");
}

const QCString RayAB::vFullTypeName() const
{
  return sFullTypeName();
}

const QString RayAB::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString RayAB::sDescriptiveName()
{
  return i18n( "Ray" );
}

const QString RayAB::vDescription() const
{
  return sDescription();
}

const QString RayAB::sDescription()
{
  return i18n( "A ray by its start point, and another point somewhere on it." );
}

const QCString RayAB::vIconFileName() const
{
  return sIconFileName();
}

const QCString RayAB::sIconFileName()
{
  return "ray";
}

const int RayAB::vShortCut() const
{
  return sShortCut();
}

const int RayAB::sShortCut()
{
  return 0;
}

const Coordinate Ray::p1() const
{
  return mpa;
}

const Coordinate Ray::p2() const
{
  return mpb;
}

const char* RayAB::sActionName()
{
  return "objects_new_ray";
}

bool Ray::isa( int type ) const
{
  return type == RayT ? true : Parent::isa( type );
}
