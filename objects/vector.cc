// vector.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "vector.h"

#include "point.h"

#include "../misc/i18n.h"
#include "../misc/common.h"
#include "../misc/kigpainter.h"
#include "../misc/objects.h"

#include <qpen.h>

Vector::Vector( const Objects& os )
 : p1( 0 ), p2( 0 )
{
  assert( os.size() == 2 );
  p1 = os[0]->toPoint();
  p2 = os[1]->toPoint();
  assert( p1 && p2 );
  p1->addChild( this );
  p2->addChild( this );
}

Vector::Vector( const Vector& s )
  : Object( s ), p1( s.p1 ), p2( s.p2 )
{
  p1->addChild(this);
  p2->addChild(this);
}

Vector::~Vector()
{
};

const QCString Vector::vBaseTypeName() const
{
  return sBaseTypeName();
}

QCString Vector::sBaseTypeName()
{
  return I18N_NOOP( "Vector" );
}

const QCString Vector::vFullTypeName() const
{
  return sFullTypeName();
}

QCString Vector::sFullTypeName()
{
  return "Vector";
}

const QString Vector::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString Vector::sDescriptiveName()
{
  return i18n( "Vector" );
}

const QString Vector::vDescription() const
{
  return sDescription();
}

const QString Vector::sDescription()
{
  return i18n( "Construct a vector from two given points." );
}

const QCString Vector::vIconFileName() const
{
  return sIconFileName();
}

const QCString Vector::sIconFileName()
{
  return "vector";
}

const int Vector::vShortCut() const
{
  return sShortCut();
}

const int Vector::sShortCut()
{
  return 0;
}

const char* Vector::sActionName()
{
  return "objects_new_vector";
}

bool Vector::contains(const Coordinate& o, const ScreenInfo& si ) const
{
  return isOnSegment( o, p1->getCoord(), p2->getCoord(), si.normalMiss() );
}

void Vector::draw( KigPainter& p, bool ss ) const
{
  Coordinate a = p1->getCoord();
  Coordinate b = p2->getCoord();
  p.setPen( QPen( selected && ss ? Qt::red : mColor, 1 ));
  drawVector( p, a, b );
};

void Vector::drawVector( KigPainter& p, const Coordinate& a,
                         const Coordinate& b )
{
  // bugfix...
  if ( a == b ) return;
  // the segment
  p.drawSegment( a, b );
  // the arrows...
  Coordinate dir = b - a;
  Coordinate perp( dir.y, -dir.x );
  double length = perp.length();
  perp *= 10* p.pixelWidth();
  perp /= length;
  dir *= 10 * p.pixelWidth();
  dir /= length;
  Coordinate c = b - dir + perp;
  Coordinate d = b - dir - perp;
  p.drawSegment( b, c );
  p.drawSegment( b, d );
}

bool Vector::inRect( const Rect& ) const
{
  // TODO: implement :)
  // @see Segment::inRect, Line::inRect
  return false;
}

Object::WantArgsResult Vector::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  if ( !os[0]->toPoint() || ( size == 2 && !os[1]->toPoint() ) )
      return NotGood;
  return size == 2 ? Complete : NotComplete;
}

QString Vector::sUseText( const Objects& os, const Object* o )
{
  if ( os.size() == 2 || ( os.size() == 1 && o ) ) return i18n( "End point" );
  return i18n( "Start point" );
}

void Vector::sDrawPrelim( KigPainter& p, const Objects& o )
{
  assert( o.size() <= 2 );
  if ( o.size() != 2 ) return;
  Point* a = o[0]->toPoint();
  Point* b = o[1]->toPoint();
  assert( a && b );
  p.setPen( QPen( Qt::red, 1 ) );
  drawVector( p, a->getCoord(), b->getCoord() );
}

Objects Vector::getParents() const
{
  Objects ret;
  ret.push_back( p1 );
  ret.push_back( p2 );
  return ret;
}

void Vector::calc()
{
  mvalid = p1->valid() && p2->valid();
}

const Coordinate Vector::getP1() const
{
  return p1->getCoord();
}

const Coordinate Vector::getP2() const
{
  return p2->getCoord();
}

const Coordinate Vector::getDir() const
{
  return getP2() - getP1();
}

bool Vector::isa( int type ) const
{
  return type == VectorT ? true : Parent::isa( type );
}
