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
  : p1( 0 ), p2( 0 )
{
  assert( os.size() == 2 );
  p1 = os[0]->toPoint();
  p2 = os[1]->toPoint();
  assert( p1 && p2 );
  p1->addChild( this );
  p2->addChild( this );
}

Ray::Ray(const Ray& s)
  : Curve( s ), p1( s.p1 ), p2( s.p2 )
{
  p1->addChild(this);
  p2->addChild(this);
}

Ray::~Ray()
{
}

bool Ray::contains(const Coordinate& o, const double fault ) const
{
  return isOnRay( o, p1->getCoord(), p2->getCoord(), fault );
}

void Ray::draw(KigPainter& p, bool ss) const
{
  p.setPen( QPen( selected && ss ? Qt::red : mColor, 1 ));
  p.drawRay( p1->getCoord(), p2->getCoord() );
}

bool Ray::inRect(const Rect& p) const
{
  // TODO: implement for real...
  if ( p1->inRect( p ) || p2->inRect( p ) ) return true;
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
  assert( p1 && p2 );
  p1->startMove( p );
  p2->startMove( p );
}

void Ray::moveTo(const Coordinate& p)
{
  p1->moveTo( p );
  p2->moveTo( p );
}

void Ray::stopMove()
{
}

void Ray::calc( const ScreenInfo& )
{
  mvalid = p1->valid() && p2->valid();
}

Coordinate Ray::getPoint(double param) const
{
  Coordinate dir = p2->getCoord() - p1->getCoord();
  param += 1;
  param = pow( param, 12 ) - 1;
  return p1->getCoord() + dir*param;
}

double Ray::getParam(const Coordinate& p) const
{
  Coordinate a = p1->getCoord();
  Coordinate b = p2->getCoord();
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
  objs.push_back( p1 );
  objs.push_back( p2 );
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

const Coordinate Ray::getP1() const
{
  return p1->getCoord();
}

const Coordinate Ray::getP2() const
{
  return p2->getCoord();
}

const char* Ray::sActionName()
{
  return "objects_new_ray";
}
