// midpoint.cc
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

#include "midpoint.h"

#include "segment.h"

#include "../misc/kigpainter.h"

#include <klocale.h>

MidPoint::MidPoint(const MidPoint& m)
  : Point( m ), p1( m.p1 ), p2( m.p2 ), s( m.s )
{
  if ( p1 ) p1->addChild(this);
  if ( p2 ) p2->addChild(this);
  if ( s ) s->addChild( this );
}

MidPoint::~MidPoint()
{
}

void MidPoint::startMove(const Coordinate& p)
{
  if (contains(p,false) && p1 )
  {
    assert( p1 && p2 );
    howm = howmMoving;
    p2->startMove(p2->getCoord());
  }
  else howm = howmFollowing;
}

void MidPoint::moveTo(const Coordinate& p)
{
  if (howm == howmFollowing)
    return;
  mC = p;
  p2->moveTo( mC * 2 - p1->getCoord() );
}

void MidPoint::stopMove()
{
};

void MidPoint::calc( const ScreenInfo& )
{
  if ( p1 || p2 )
  {
    assert( p1 && p2 );
    assert( ! s );
    mvalid = p1->valid() && p2->valid();
    if ( mvalid ) mC = ( p1->getCoord() + p2->getCoord() ) / 2;
  }
  else
  {
    assert( s );
    assert( !p1 && ! p2 );
    mvalid = s->valid();
    if ( mvalid ) mC = ( s->p1() + s->p2() ) / 2;
  };
}

const QString MidPoint::sDescriptiveName()
{
  return i18n("Midpoint");
}

const QString MidPoint::sDescription()
{
  return i18n( "The midpoint of a segment or two other points" );
}

const char* MidPoint::sActionName()
{
  return "objects_new_midpoint";
}

MidPoint::MidPoint( const Objects& os )
  : p1( 0 ), p2( 0 ), s( 0 )
{
  uint size = os.size();
  if ( size == 2 )
  {
    p1 = os[0]->toPoint();
    assert( p1 );
    p1->addChild( this );
    p2 = os[1]->toPoint();
    assert( p2 );
    p2->addChild( this );
  }
  else
  {
    assert( size == 1 );
    s = os[0]->toSegment();
    assert( s );
    s->addChild( this );
  };
}

Objects MidPoint::getParents() const
{
  Objects tmp;
  if ( p1 ) tmp.push_back(p1);
  if ( p2 ) tmp.push_back(p2);
  if ( s ) tmp.push_back( s );
  return tmp;
}

Object::WantArgsResult MidPoint::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size == 2 )
  {
    if ( os[0]->toPoint() && os[1]->toPoint() ) return Complete;
    return NotGood;
  }
  if ( size != 1 ) return NotGood;
  if ( os[0]->toSegment() ) return Complete;
  if ( os[0]->toPoint() ) return NotComplete;
  return NotGood;
}

QString MidPoint::sUseText( const Objects& os, const Object* o )
{
  if ( o->toPoint() ) return os.empty() ? i18n( "First point" )
                        : i18n( "Second point" );
  else if ( o->toSegment() ) return i18n( "Midpoint of this segment" );
  else assert( false );
}

void MidPoint::sDrawPrelim( KigPainter& p, const Objects& args )
{
  uint size = args.size();
  Coordinate m;
  if ( size == 1 )
  {
    Segment* s;
    if ( args[0]->toPoint() ) return;
    else if ( ( s = args[0]->toSegment() ) )
    {
      Coordinate a = s->p1(), b = s->p2();
      m = (a+b)/2;
    }
    else assert( false );
  }
  else
  {
    assert( size == 2 );
    Point* p = args[0]->toPoint();
    Point* q = args[1]->toPoint();
    assert( p && q );
    Coordinate a = p->getCoord(), b = q->getCoord();
    m = ( a + b ) / 2;
  };
  p.drawPoint( m, false );
}
