// translatedpoint.cc
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

#include "rotatedpoint.h"

#include "arc.h"

#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "../misc/common.h"

#include <kdebug.h>

Objects RotatedPoint::getParents() const
{
  Objects tmp;
  tmp.push_back( mp );
  tmp.push_back( mc );
  tmp.push_back( ma );
  return tmp;
};

void RotatedPoint::calc( const ScreenInfo& )
{
  mvalid = mp->valid() && mc->valid() && ma->valid();
  mC = calcRotatedPoint( mp->getCoord(), mc->getCoord(), ma->size() );
}

RotatedPoint::RotatedPoint( const RotatedPoint& p )
  : Point( p ), mp( p.mp ), mc( p.mc ), ma( p.ma )
{
  mp->addChild( this );
  mc->addChild( this );
  ma->addChild( this );
};

RotatedPoint::~RotatedPoint()
{
}

RotatedPoint* RotatedPoint::copy()
{
  return new RotatedPoint( *this );
}

const QCString RotatedPoint::vFullTypeName() const
{
  return sFullTypeName();
}

const QCString RotatedPoint::sFullTypeName()
{
  return "RotatedPoint";
}

const QString RotatedPoint::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString RotatedPoint::sDescriptiveName()
{
  return i18n("Rotated Point");
}

const QString RotatedPoint::vDescription() const
{
  return sDescription();
}

const QString RotatedPoint::sDescription()
{
  return i18n( "A point rotated around another point by a certain arc" );
}

const QCString RotatedPoint::vIconFileName() const
{
  return sIconFileName();
}

const QCString RotatedPoint::sIconFileName()
{
  return "rotation";
}

const int RotatedPoint::vShortCut() const
{
  return sShortCut();
}

const int RotatedPoint::sShortCut()
{
  return 0;
}

void RotatedPoint::startMove(const Coordinate& )
{
}

void RotatedPoint::moveTo(const Coordinate& )
{
}

void RotatedPoint::stopMove()
{
}

const char* RotatedPoint::sActionName()
{
  return "objects_new_rotatedpoint";
}

RotatedPoint::RotatedPoint( const Objects& os )
  : Point(), mp( 0 ), mc( 0 ), ma( 0 )
{
  assert( os.size() == 3 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! mp ) mp = (*i)->toPoint();
    if ( ! mc ) mc = (*i)->toPoint();
    if ( ! ma ) ma = (*i)->toArc();
  };
  assert( mp && mc && ma );
  mp->addChild( this );
  mc->addChild( this );
  ma->addChild( this );
}

void RotatedPoint::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Point* mp, *mc;
  Arc* ma;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! mp ) mp = (*i)->toPoint();
    if ( ! mc ) mc = (*i)->toPoint();
    if ( ! ma ) ma = (*i)->toArc();
  };
  assert( mp && mc && ma );

  Coordinate c = calcRotatedPoint( mp->getCoord(), mc->getCoord(), ma->size() );

  p.setPen( QPen( Qt::red, 1 ) );
  p.drawPoint( c, false );
}

Object::WantArgsResult RotatedPoint::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size < 1 || size > 3 ) return NotGood;
  int p = 0;
  int a = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( (*i)->toPoint() ) ++p;
    else if ( (*i)->toArc() ) ++a;
    else return NotGood;
  };
  if ( p > 2 || a > 1 ) return NotGood;
  return size == 3 ? Complete : NotComplete;
}

QString RotatedPoint::sUseText( const Objects& os, const Object* o )
{
  bool gotpoint = false;
  if ( os.size() >= 1 && os[0]->toPoint() ) gotpoint = true;
  if ( os.size() >= 2 && os[1]->toPoint() ) gotpoint = true;
  if ( o->toPoint() && ! gotpoint ) return i18n( "Rotate this point" );
  if ( o->toPoint() ) return i18n( "Rotate point around this point" );
  if ( o->toArc() ) return i18n( "Rotate point by this arc" );
  assert( false );
}
