// mirrorpoint.cc
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

#include "mirrorpoint.h"

#include "segment.h"
#include "line.h"

#include "../misc/i18n.h"

#include <kdebug.h>

Objects MirrorPoint::getParents() const
{
  Objects tmp;
  tmp.push_back( mp );
  if ( ml ) tmp.push_back( ml );
  if ( mc ) tmp.push_back( mc );
  return tmp;
};

void MirrorPoint::calc()
{
  mvalid = mp->valid();
  Coordinate a = mp->getCoord();
  if ( ! mc )
  {
    assert( ml );
    mvalid &= ml->valid();
    LineData l = ml->lineData();
    if ( mvalid ) mC = calcMirrorPoint( l, a );
  }
  else
  {
    assert( mc );
    mvalid &= mc->valid();
    if ( mvalid ) mC = 2* mc->getCoord() - a;
  };
}

MirrorPoint::MirrorPoint( const MirrorPoint& p )
  : Point( p ), mp( p.mp ), ml( p.ml ), mc( p.mc )
{
  mp->addChild( this );
  if ( ml ) ml->addChild( this );
  if ( mc ) mc->addChild( this );
};

MirrorPoint::~MirrorPoint()
{
}

const QCString MirrorPoint::vFullTypeName() const
{
  return sFullTypeName();
}

const QCString MirrorPoint::sFullTypeName()
{
  return "MirrorPoint";
}

const QString MirrorPoint::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString MirrorPoint::sDescriptiveName()
{
  return i18n("Mirrored Point");
}

const QString MirrorPoint::vDescription() const
{
  return sDescription();
}

const QString MirrorPoint::sDescription()
{
  return i18n( "Mirror a point over a line or segment" );
}

const QCString MirrorPoint::vIconFileName() const
{
  return sIconFileName();
}

const QCString MirrorPoint::sIconFileName()
{
  return "mirrorpoint";
}

const int MirrorPoint::vShortCut() const
{
  return sShortCut();
}

const int MirrorPoint::sShortCut()
{
  return 0;
}

const char* MirrorPoint::sActionName()
{
  return "objects_new_mirrorpoint";
}

MirrorPoint::MirrorPoint( const Objects& os )
  : Point(), mp( 0 ), ml( 0 ), mc( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! mp ) mp = (*i)->toPoint();
    else if ( ! mc ) mc = (*i)->toPoint();
    if ( ! ml ) ml = (*i)->toAbstractLine();
  };
  assert( mp && ( ml || mc ) );
  mp->addChild( this );
  if ( ml ) ml->addChild( this );
  if ( mc ) mc->addChild( this );
}

void MirrorPoint::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Point* q = 0;
  AbstractLine* l = 0;
  Point* c = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! q ) q = (*i)->toPoint();
    else if ( ! c ) c = (*i)->toPoint();
    if ( ! l ) l = (*i)->toAbstractLine();
  };
  assert( q && ( l || c ) );
  Coordinate d;
  if ( ! c )
  {
    LineData ld = l->lineData();
    Coordinate e = q->getCoord();
    d = calcMirrorPoint( ld, e );
  }
  else
  {
    d = 2* c->getCoord() - q->getCoord();
  };
  sDrawPrelimPoint( p, d );
}

Object::WantArgsResult MirrorPoint::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  int v = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( (*i)->toAbstractLine() ) ++v;
    else if ( ! (*i)->toPoint() ) return NotGood;
  };
  if ( v > 1 ) return NotGood;
  return size == 2 ? Complete : NotComplete;
}

QString MirrorPoint::sUseText( const Objects& os, const Object* o )
{
  if ( o->toPoint() )
    return os.empty() ? i18n( "Mirror point of this point" )
      : i18n( "Mirror over this point" );
  if ( o->toLine() ) return i18n( "Mirror over this line" );
  if ( o->toSegment() ) return i18n( "Mirror point over this segment" );
  if ( o->toRay() ) return i18n( "Mirror point over this ray" );
  assert( false );
}
