/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#include "conicsextra.h"

#include "conic.h"
#include "point.h"

#include "../misc/common.h"
#include "../misc/conic-common.h"
#include "../misc/kigpainter.h"

#include <klocale.h>
#include <kdebug.h>
#include <qpen.h>
#include <math.h>

LinePolar::~LinePolar()
{
}

void LinePolar::calc()
{
  const ConicCartesianEquationData& data = conic->cartesianEquationData();
  Coordinate cpole=pole->getCoord();

  assert ( conic && pole );
  const LineData l =
    calcConicPolarLine( data, cpole, mvalid );

  if( mvalid )
  {
    mpa = l.a;
    mpb = l.b;
  };
}

Objects LinePolar::getParents() const
{
  Objects objs;
  objs.push_back( conic );
  objs.push_back( pole );
  return objs;
}

LinePolar::LinePolar(const LinePolar& l)
  : Line( l ), conic( l.conic ), pole( l.pole )
{
  conic->addChild(this);
  pole->addChild(this);
}

const QString LinePolar::sDescriptiveName()
{
  return i18n("Polar line of a point");
}

QString LinePolar::sDescription()
{
  return i18n( "The polar line of a point with respect to a conic." );
}

const char* LinePolar::sActionName()
{
  return "objects_new_linepolar";
}

LinePolar::LinePolar( const Objects& os )
  : conic( 0 ), pole( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! conic ) conic = (*i)->toConic();
    if ( ! pole ) pole = (*i)->toPoint();
  }
  assert( conic && pole );
  conic->addChild( this );
  pole->addChild( this );
}

void LinePolar::sDrawPrelim( KigPainter& p, const Objects& os )
{
  assert( os.size() <= 2 );
  if ( os.size() != 2 ) return;
  Conic* c = 0;
  Point* pole = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! c ) c = (*i)->toConic();
    if ( ! pole ) pole = (*i)->toPoint();
  };
  assert( c && pole );

  const ConicCartesianEquationData data=c->cartesianEquationData();
  Coordinate cpole=pole->getCoord();
  bool valid = false;

  const LineData l = calcConicPolarLine( data, cpole, valid );

  if ( ! valid ) return;

  p.setPen (QPen (Qt::red, 1) );
  p.drawLine( l );
  return;
}

Object::WantArgsResult LinePolar::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  bool gotc = false;
  bool gotpole = false;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! gotc ) gotc = (*i)->toConic();
    if ( ! gotpole ) gotpole = (*i)->toPoint();
  }
  if ( size == 1 ) return (gotc || gotpole ) ? NotComplete : NotGood;
  if ( size == 2 ) return (gotc && gotpole ) ? Complete : NotGood;
  assert ( false );
}

QString LinePolar::sUseText( const Objects&, const Object* o )
{
  if ( o->toConic() ) return i18n( "Polar line with respect to this conic" );
  if ( o->toPoint() ) return i18n( "Polar line of this point" );
  else assert( false );
}

PointPolar::~PointPolar()
{
}

void PointPolar::calc()
{
  const ConicCartesianEquationData data=conic->cartesianEquationData();

  assert ( conic && polar );
  Coordinate c =
    calcConicPolarPoint(
      data, polar->lineData(), mvalid
      );

  if ( mvalid ) mC = c;
}

Objects PointPolar::getParents() const
{
  Objects objs;
  objs.push_back( conic );
  objs.push_back( polar );
  return objs;
}

PointPolar::PointPolar(const PointPolar& p)
  : Point( p ), conic( p.conic ), polar( p.polar )
{
  conic->addChild(this);
  polar->addChild(this);
}

const QString PointPolar::sDescriptiveName()
{
  return i18n("Polar point of a line");
}

QString PointPolar::sDescription()
{
  return i18n( "The polar point of a line with respect to a conic." );
}

const char* PointPolar::sActionName()
{
  return "objects_new_pointpolar";
}

PointPolar::PointPolar( const Objects& os )
  : Point(), conic( 0 ), polar( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! conic ) conic = (*i)->toConic();
    if ( ! polar ) polar = (*i)->toAbstractLine();
  }
  assert( conic && polar );
  conic->addChild( this );
  polar->addChild( this );
}

void PointPolar::sDrawPrelim( KigPainter& p, const Objects& os )
{
  assert( os.size() <= 2 );
  if ( os.size() != 2 ) return;
  Conic* c = 0;
  AbstractLine* polar = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! c ) c = (*i)->toConic();
    if ( ! polar) polar = (*i)->toAbstractLine();
  };
  assert( c && polar );

  const ConicCartesianEquationData data=c->cartesianEquationData();
  bool valid = false;

  Coordinate ce = calcConicPolarPoint(
    data, polar->lineData(), valid
    );

  if ( valid ) sDrawPrelimPoint( p, ce );
  return;
}

Object::WantArgsResult PointPolar::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  bool gotc = false;
  bool gotpolar = false;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! gotc ) gotc = (*i)->toConic();
    if ( ! gotpolar ) gotpolar = (*i)->toAbstractLine();
  }
  if ( size == 1 ) return ( gotc || gotpolar ) ? NotComplete : NotGood;
  if ( size == 2 ) return ( gotc && gotpolar ) ? Complete : NotGood;
  assert( false );
}

QString PointPolar::sUseText( const Objects&, const Object* o )
{
  if ( o->toConic() ) return i18n( "Polar point with respect to this conic" );
  if ( o->toLine() ) return i18n( "Polar point of this line" );
  if ( o->toSegment() ) return i18n( "Polar point of this segment" );
  if ( o->toRay() ) return i18n( "Polar point of this ray" );
  else assert( false );
}

