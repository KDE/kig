// arc.cc
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


#include "arc.h"
#include "point.h"

#include "../misc/coordinate.h"
#include "../misc/common.h"
#include "../misc/i18n.h"
#include "../misc/screeninfo.h"
#include "../misc/kigpainter.h"

#include <qpen.h>

#include <algorithm>
#include <cmath>

Arc::Arc( const Objects& os )
  : Object()
{
  assert( os.size() == 3 );
  mpts[0] = os[0]->toPoint();
  mpts[1] = os[1]->toPoint();
  mpts[2] = os[2]->toPoint();
  std::for_each( mpts, mpts+3, std::bind2nd( std::mem_fun( &Object::addChild ), this ) );
}

Arc::Arc( const Arc& a )
  : Object()
{
  std::copy( a.mpts, a.mpts+3, mpts );
  std::for_each( mpts, mpts+3, std::bind2nd( std::mem_fun( &Object::addChild ), this ) );
}

Arc* Arc::copy()
{
  return new Arc( *this );
}

Arc::~Arc()
{
}

Arc* Arc::toArc()
{
  return this;
}

const Arc* Arc::toArc() const
{
  return this;
}

const QCString Arc::vBaseTypeName() const
{
  return sBaseTypeName();
}

QCString Arc::sBaseTypeName()
{
  return "arc";
}

const QCString Arc::vFullTypeName() const
{
  return sFullTypeName();
}

QCString Arc::sFullTypeName()
{
  return "Arc";
}

const QString Arc::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString Arc::sDescriptiveName()
{
  return i18n( "Arc by three points" );
}

const QString Arc::vDescription() const
{
  return sDescription();
}

const QString Arc::sDescription()
{
  return i18n( "An arc defined by three points" );
}

const QCString Arc::vIconFileName() const
{
  return sIconFileName();
}

const QCString Arc::sIconFileName()
{
  return "arc";
}

const int Arc::vShortCut() const
{
  return sShortCut();
}

const int Arc::sShortCut()
{
  return 0;
}

const char* Arc::sActionName()
{
  return "objects_new_arc";
}

Object::WantArgsResult Arc::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 3 ) return NotGood;
  if ( size < 1 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! (*i)->toPoint() ) return NotGood;
  };
  return size == 3 ? Complete : NotComplete;
}

QString Arc::sUseText( const Objects& os, const Object* )
{
  return os.size() == 1 ? i18n( "Center point" )
                    : i18n( "Point on arc" );
}

Objects Arc::getParents() const
{
  Objects ret;
  std::copy( mpts, mpts+3, std::back_inserter( ret ) );
  return ret;
}

void Arc::startMove(const Coordinate&)
{
}

void Arc::moveTo(const Coordinate&)
{
}

void Arc::stopMove()
{
}

void Arc::calc( const ScreenInfo& si )
{
  mvalid = true;
  for ( Point** i = mpts; i < mpts + 3; ++i )
    mvalid &= (*i)->valid();
  Coordinate cds[3];
  std::transform( mpts, mpts+3, cds, std::mem_fun( &Point::getCoord ) );

  Coordinate lvect = cds[0] - cds[1];
  Coordinate rvect = cds[2] - cds[1];

  double radius = kigMin( lvect.length(), rvect.length() ) / 2.;

  // the arrow:
  {
    marrow.clear();

    lvect = lvect.normalize( radius );
    rvect = rvect.normalize( radius );

    Coordinate sega = -rvect;
    sega = sega.normalize( 6*si.pixelWidth() );
    Coordinate segb = sega.orthogonal();

    marrow.push_back( rvect + cds[1] );
    marrow.push_back( marrow[0] + segb + sega );
    marrow.push_back( marrow[0] + segb - sega );
  };

  // the angles:
  {
    lvect = lvect.normalize();
    double dstartangle = std::acos( lvect.x ) * 2880 / M_PI;
    if ( lvect.y < 0 ) dstartangle = 5760 - dstartangle;
    kdDebug() << k_funcinfo << dstartangle << endl;
    rvect = rvect.normalize();
    double dendangle = std::acos( -rvect.x ) * 2880 / M_PI;
    if ( rvect.y < 0 ) dendangle = -dendangle;
    double danglelength = 2880 - dendangle - dstartangle;
    kdDebug() << k_funcinfo << danglelength << endl;
    startangle = static_cast<int>( dstartangle );
    if ( startangle < 0 ) startangle += 5760;
    anglelength = static_cast<int>( danglelength );
    if ( anglelength < 0 ) anglelength += 5760;
  }

  // the bounding rect:
  {
    mr.setWidth( radius * 2 );
    mr.setHeight( radius * 2 );
    mr.setCenter( cds[1] );
  };
}

bool Arc::contains(const Coordinate&, const double ) const
{
  return false;
  // TODO
}

void Arc::draw( KigPainter& p, bool ss ) const
{
  kdDebug() << k_funcinfo << mr << "; " << startangle << "; " << anglelength << endl;
  Coordinate cds[3];
  std::transform( mpts, mpts+3, cds, std::mem_fun( &Point::getCoord ) );

  p.setPen( QPen( selected && ss ? Qt::red : mColor, 1 ) );
  p.setBrush( QBrush( Qt::black, Qt::SolidPattern ) );
  p.drawArc( mr, startangle, anglelength );
  p.drawPolygon( marrow );
  p.drawRay( cds[1], cds[0] );
  p.drawRay( cds[1], cds[2] );
}

bool Arc::inRect( const Rect& ) const
{
  return false;
  // TODO
}

void Arc::sDrawPrelim( KigPainter&, const Objects& )
{
  // TODO
}

