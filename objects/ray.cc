/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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

Ray::Ray( const Objects& os )
  : mpa( 0 ), mpb( 0 )
{
  assert( os.size() == 2 );
  mpa = os[0]->toPoint();
  mpb = os[1]->toPoint();
  assert( mpa && mpb );
  mpa->addChild( this );
  mpb->addChild( this );
}

Ray::Ray(const Ray& s)
  : AbstractLine( s ), mpa( s.mpa ), mpb( s.mpb )
{
  mpa->addChild(this);
  mpb->addChild(this);
}

Ray::~Ray()
{
}

bool Ray::contains(const Coordinate& o, const double fault ) const
{
  return isOnRay( o, mpa->getCoord(), mpb->getCoord(), fault );
}

void Ray::draw(KigPainter& p, bool ss) const
{
  p.setPen( QPen( selected && ss ? Qt::red : mColor, 1 ));
  p.drawRay( mpa->getCoord(), mpb->getCoord() );
}

bool Ray::inRect(const Rect& p) const
{
  // TODO: implement for real...
  if ( mpa->inRect( p ) || mpb->inRect( p ) ) return true;
  return false;
}

Object::WantArgsResult Ray::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  if ( !os[0]->toPoint() || ( size == 2 && !os[1]->toPoint() ) )
      return NotGood;
  return size == 2 ? Complete : NotComplete;
};

QString Ray::sUseText( const Objects& os, const Object* o )
{
  if ( os.size() == 2 || ( os.size() == 1 && o ) ) return i18n( "Ray through this point" );
  return i18n( "Ray starting at this point" );
}

void Ray::startMove(const Coordinate& p)
{
  pwwsm = p;
  assert( mpa && mpb );
  mpa->startMove( p );
  mpb->startMove( p );
}

void Ray::moveTo(const Coordinate& p)
{
  mpa->moveTo( p );
  mpb->moveTo( p );
}

void Ray::stopMove()
{
}

void Ray::calc( const ScreenInfo& )
{
  mvalid = mpa->valid() && mpb->valid();
}

Coordinate Ray::getPoint(double param) const
{
  Coordinate dir = mpb->getCoord() - mpa->getCoord();
  param += 1;
  param = pow( param, 12 ) - 1;
  return mpa->getCoord() + dir*param;
}

double Ray::getParam(const Coordinate& p) const
{
  Coordinate a = mpa->getCoord();
  Coordinate b = mpb->getCoord();
  Coordinate pt = calcPointOnPerpend( a, b, p );
  pt = calcIntersectionPoint( a, b, p, pt);
  // if pt is over the end of the ray ( i.e. it's on the line
  // which the ray is a part of, but not of the ray itself..;
  // ) we set it to the start point of the ray...
  if ( ( a.x - b.x ) == ( a.x - pt.x ) ) pt = a;
  Coordinate dir = b - a;
  pt -= a;
  double param;
  if ( dir.x != 0 ) param = pt.x / dir.x;
  else if ( dir.y != 0 ) param = pt.y / dir.y;
  else param = 0;
  param += 1;
  param = pow( param, 1./12  );
  param -= 1;
  assert( param >= 0 && param <= 1 );
  return param;
}

Objects Ray::getParents() const
{
  Objects objs;
  objs.push_back( mpa );
  objs.push_back( mpb );
  return objs;
}

void Ray::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  assert( os[0]->toPoint() && os[1]->toPoint() );
  Coordinate a = os[0]->toPoint()->getCoord();
  Coordinate b = os[1]->toPoint()->getCoord();
  p.setPen( QPen( Qt::red, 1));
  p.drawRay( a, b );
}

Ray* Ray::copy()
{
  return new Ray(*this);
}

Ray* Ray::toRay()
{
  return this;
}

const Ray* Ray::toRay() const
{
  return this;
}

const QCString Ray::vBaseTypeName() const
{
  return sBaseTypeName();
}

QCString Ray::sBaseTypeName()
{
  return I18N_NOOP("ray");
}

const QCString Ray::vFullTypeName() const
{
  return sFullTypeName();
}

QCString Ray::sFullTypeName()
{
  return I18N_NOOP("Ray");
}

const QString Ray::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString Ray::sDescriptiveName()
{
  return i18n( "Ray" );
}

const QString Ray::vDescription() const
{
  return sDescription();
}

const QString Ray::sDescription()
{
  return i18n( "A ray by its start point, and another point somewhere on "
               "it..." );
}

const QCString Ray::vIconFileName() const
{
  return sIconFileName();
}

const QCString Ray::sIconFileName()
{
  return "ray";
}

const int Ray::vShortCut() const
{
  return sShortCut();
}

const int Ray::sShortCut()
{
  return 0;
}

const Coordinate Ray::p1() const
{
  return mpa->getCoord();
}

const Coordinate Ray::p2() const
{
  return mpb->getCoord();
}

const char* Ray::sActionName()
{
  return "objects_new_ray";
}
