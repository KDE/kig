// angle.cc
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


#include "angle.h"
#include "point.h"

#include "../misc/coordinate.h"
#include "../misc/common.h"
#include "../misc/i18n.h"
#include "../misc/kigpainter.h"
#include "../kig/kig_view.h"

#include <qpen.h>

#include <algorithm>
#include <cmath>

#include <assert.h>

Angle::Angle( const Objects& os )
  : Object()
{
  assert( os.size() == 3 );
  mpts[0] = os[0]->toPoint();
  mpts[1] = os[1]->toPoint();
  mpts[2] = os[2]->toPoint();
  std::for_each( mpts, mpts+3, std::bind2nd( std::mem_fun( &Object::addChild ), this ) );
}

Angle::Angle( const Angle& a )
  : Object( a )
{
  std::copy( a.mpts, a.mpts+3, mpts );
  std::for_each( mpts, mpts+3, std::bind2nd( std::mem_fun( &Object::addChild ), this ) );
}

Angle::~Angle()
{
}

const QCString Angle::vBaseTypeName() const
{
  return sBaseTypeName();
}

QCString Angle::sBaseTypeName()
{
  return I18N_NOOP("angle");
}

const QCString Angle::vFullTypeName() const
{
  return sFullTypeName();
}

QCString Angle::sFullTypeName()
{
  return "Angle";
}

const QString Angle::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString Angle::sDescriptiveName()
{
  return i18n( "Angle by three points" );
}

const QString Angle::vDescription() const
{
  return sDescription();
}

const QString Angle::sDescription()
{
  return i18n( "An angle defined by three points" );
}

const QCString Angle::vIconFileName() const
{
  return sIconFileName();
}

const QCString Angle::sIconFileName()
{
  return "angle";
}

const int Angle::vShortCut() const
{
  return sShortCut();
}

const int Angle::sShortCut()
{
  return 0;
}

const char* Angle::sActionName()
{
  return "objects_new_angle";
}

Object::WantArgsResult Angle::sWantArgs( const Objects& os )
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

QString Angle::sUseText( const Objects& os, const Object* )
{
  return os.size() == 1 ? i18n( "Center point" )
                    : i18n( "Point on angle" );
}

Objects Angle::getParents() const
{
  Objects ret;
  std::copy( mpts, mpts+3, std::back_inserter( ret ) );
  return ret;
}

void Angle::calcForWidget( const KigWidget& w )
{
  calc();

  Coordinate cds[3];
  std::transform( mpts, mpts+3, cds, std::mem_fun( &Point::getCoord ) );

  Coordinate lvect = cds[0] - cds[1];
  Coordinate rvect = cds[2] - cds[1];

  double radius = kigMin( lvect.length(), rvect.length() ) / 2.;

  // calc what the arrow looks like..
  marrow.clear();

  lvect = lvect.normalize( radius );
  rvect = rvect.normalize( radius );

  Coordinate sega = -rvect;
  sega = sega.normalize( 6*w.pixelWidth() );
  Coordinate segb = sega.orthogonal();

  marrow.push_back( rvect + cds[1] );
  marrow.push_back( marrow[0] + segb + sega );
  marrow.push_back( marrow[0] + segb - sega );
}

void Angle::calc()
{
  mvalid = true;
  for ( Point** i = mpts; i < mpts + 3; ++i )
    mvalid &= (*i)->valid();
  Coordinate cds[3];
  std::transform( mpts, mpts+3, cds, std::mem_fun( &Point::getCoord ) );

  Coordinate lvect = cds[0] - cds[1];
  Coordinate rvect = cds[2] - cds[1];

  double radius = kigMin( lvect.length(), rvect.length() ) / 2.;

  // the angles:
  {
    lvect = lvect.normalize();
    rvect = rvect.normalize();
    // we calc lots of angles... i don't really know how to explain
    // this in text but i'll try...  take point a( 0,0 ), b( 1,1 ), c(
    // -1, 1 ), d( 1,0 ), e( -1,0 )   and bac is the angle we want, then
    // dab is the startangle, and eac is the end angle..  we calc bac
    // by taking pi - startangle - endangle...
    mstartangle = std::acos( lvect.x );
    if ( lvect.y < 0 ) mstartangle = 2*M_PI - mstartangle;

    double endangle = std::acos( -rvect.x );
    if ( rvect.y < 0 ) endangle = -endangle;

    manglelength = M_PI - endangle - mstartangle;

    if ( manglelength < 0 ) manglelength += 2*M_PI;
    if ( mstartangle < 0 ) mstartangle += 2*M_PI;
  }

  // the bounding rect:
  {
    mr.setWidth( radius * 2 );
    mr.setHeight( radius * 2 );
    mr.setCenter( cds[1] );
  };
}

bool Angle::contains(const Coordinate& a, const ScreenInfo& si ) const
{
  // we check if the point is at the right distance from the center,
  Coordinate cds[3];
  std::transform( mpts, mpts+3, cds, std::mem_fun( &Point::getCoord ) );

  Coordinate lvect = cds[0] - cds[1];
  Coordinate rvect = cds[2] - cds[1];

  double radius = kigMin( lvect.length(), rvect.length() ) / 2.;
  if ( std::fabs( (a-cds[1]).length() - radius ) > si.normalMiss() ) return false;

  // and next we check if the angle is appropriate...
  Coordinate vect = a - cds[1];
// atan2 is more appropriate...
  double angle = std::atan2( vect.y, vect.x );
//  vect = vect.normalize();
//  double angle = std::acos( vect.x );
//  if ( vect.y < 0 ) angle = 2*M_PI-angle;
  while ( angle < mstartangle ) angle += 2*M_PI;
  return ( angle <= mstartangle + manglelength );
//  if ( angle < 0 ) angle += 2* M_PI;
//  if ( angle < mstartangle ) return false;
//  if ( angle > mstartangle + manglelength ) return false;
//  return true;
}

void Angle::draw( KigPainter& p, bool ss ) const
{
  Coordinate cds[3];
  std::transform( mpts, mpts+3, cds, std::mem_fun( &Point::getCoord ) );

  QColor color = selected && ss ? Qt::red : mColor;
  p.setPen( QPen( color, 1 ) );
  p.setBrush( QBrush( color, Qt::SolidPattern ) );
  int startangle = static_cast<int>( mstartangle * 2880. / M_PI );
  int anglelength = static_cast<int>( manglelength * 2880 / M_PI );
  p.drawAngle( mr, startangle, anglelength );
  p.drawPolygon( marrow );
//   p.drawRay( cds[1], cds[0] );
//   p.drawRay( cds[1], cds[2] );
}

bool Angle::inRect( const Rect& ) const
{
  return false;
  // TODO
}

void Angle::sDrawPrelim( KigPainter& p, const Objects& os )
{
  // some initialization...
  if ( os.size() != 3 ) return;
  const Point* pts[3];
  std::transform( os.begin(), os.end(), pts, std::mem_fun( &Object::toPoint ) );
//  std::for_each( pts, pts + 3, assert );
  assert( pts[0] && pts[1] && pts[2] );
  Coordinate cds[3];
  std::transform( pts, pts + 3, cds, std::mem_fun( &Point::getCoord ) );
  Coordinate lvect = cds[0] - cds[1];
  Coordinate rvect = cds[2] - cds[1];
  double radius = kigMin( lvect.length(), rvect.length() ) / 2.;

  // the arrow...
  std::vector<Coordinate> arrow;
  {
    lvect = lvect.normalize( radius );
    rvect = rvect.normalize( radius );
    Coordinate sega = -rvect;
    sega = sega.normalize( 6 * p.pixelWidth() );
    Coordinate segb = sega.orthogonal();
    arrow.push_back( rvect + cds[1] );
    arrow.push_back( arrow[0] + segb + sega );
    arrow.push_back( arrow[0] + segb - sega );
  }

  // the angles
  int ia;
  int is;
  {
    lvect = lvect.normalize();
    rvect = rvect.normalize();
    double startangle = std::acos( lvect.x );
    if ( lvect.y < 0 ) startangle = 2*M_PI-startangle;
    double endangle = std::acos( -rvect.x );
    if ( rvect.y < 0 ) endangle = -endangle;
    double anglelength = M_PI - endangle - startangle;
    if ( anglelength < 0 ) anglelength += 2* M_PI;
    if ( startangle < 0 ) startangle += 2*M_PI;
    ia = static_cast<int>( anglelength * 2880 / M_PI );
    is = static_cast<int>( startangle * 2880 / M_PI );
  };

  // the surrounding rect...
  Rect surr;
  surr.setWidth( radius * 2 );
  surr.setHeight( radius * 2 );
  surr.setCenter( cds[1] );

  p.setPen( QPen( Qt::red, 1 ) );
  p.setBrush( QBrush( Qt::red, Qt::SolidPattern ) );
//   p.drawRay( cds[1], cds[0] );
//   p.drawRay( cds[1], cds[2] );
  p.drawPolygon( arrow );
  p.drawAngle( surr, is, ia );
}

double Angle::size() const
{
  return manglelength;
}

const uint Angle::numberOfProperties() const
{
  return Object::numberOfProperties() + 2;
}

const Property Angle::property( uint which, const KigWidget& w ) const
{
  assert( which < numberOfProperties() );
  if ( which < Object::numberOfProperties() ) return Object::property( which, w );
  if ( which == Object::numberOfProperties() )
    return Property( size() );
  else if ( which == Object::numberOfProperties() + 1 )
    return Property( size() * 180 / M_PI );
  else assert( false );
}

const QCStringList Angle::properties() const
{
  QCStringList l = Object::properties();
  l << I18N_NOOP( "Angle in radians" );
  l << I18N_NOOP( "Angle in degrees" );
  assert( l.size() == Angle::numberOfProperties() );
  return l;
}

bool Angle::isa( int type ) const
{
  return type == AngleT ? true : Parent::isa( type );
}
