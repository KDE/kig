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

#include "../modes/constructing.h"
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
  return i18n("Polar Line of a Point");
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
  return i18n("Polar Point of a Line");
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

Objects LineConicAsymptotes::getParents() const
{
  Objects ret;
  ret.push_back( conic );
  return ret;
}

LineConicAsymptotes::LineConicAsymptotes(const LineConicAsymptotes& l)
  : Line( l ), conic( l.conic ), mwhich( l.mwhich )
{
  assert( mwhich == -1 || mwhich == 1 );
  assert( conic );
  conic->addChild( this );
}

const QString LineConicAsymptotes::sDescriptiveName()
{
  return i18n("Asymptotes of a Hyperbola");
}

const QString LineConicAsymptotes::sDescription()
{
  return i18n( "The two asymptotes of a hyperbola." );
}

const char* LineConicAsymptotes::sActionName()
{
  return "objects_new_lineconicasymptotes";
}

LineConicAsymptotes::LineConicAsymptotes( const Objects& os )
{
  assert( os.size() == 1 );
  conic = os[0]->toConic();
  assert( conic );
  conic->addChild( this );
}

Object::prop_map LineConicAsymptotes::getParams()
{
  prop_map map = Object::getParams();
  map["lineconicasymptotes-branch"] = QString::number( mwhich );
  return map;
}

void LineConicAsymptotes::setParams( const prop_map& map )
{
  Line::setParams( map );
  prop_map::const_iterator p = map.find("lineconicasymptotes-branch");
  bool ok = true;
  if( p == map.end() ) mwhich = 1;   // fixme...
  else
    mwhich = p->second.toInt( &ok );
  assert( ok && ( mwhich == 1 || mwhich == -1 ) );
}

KigMode* LineConicAsymptotes::sConstructMode(
  MultiConstructibleType* ourtype,
  KigDocument* theDoc,
  NormalMode* previousMode )
{
  return new MultiConstructionMode( ourtype, previousMode, theDoc );
}

Objects LineConicAsymptotes::sMultiBuild( const Objects& args )
{
  LineConicAsymptotes* a = new LineConicAsymptotes( args );
  LineConicAsymptotes* b = new LineConicAsymptotes( args );

  a->mwhich = 1;
  b->mwhich = -1;

  Objects o;
  o.push_back( a );
  o.push_back( b );
  return o;
}

void LineConicAsymptotes::calc()
{
  const ConicCartesianEquationData cequation = conic->cartesianEquationData();
  mvalid = true;
  LineData line = calcConicAsymptote( cequation, mwhich, mvalid );
  if ( ! mvalid ) return;
  mpa = line.a;
  mpb = line.b;
}

void LineConicAsymptotes::sDrawPrelim( KigPainter& p, const Objects& os )
{
  assert( os.size() < 2 );
  if ( os.size() != 1 ) return;

  Conic* conic = os[0]->toConic();
  assert( conic );

  const ConicCartesianEquationData cequation = conic->cartesianEquationData();

  bool mvalid = true;
  const LineData line1 = calcConicAsymptote( cequation, 1, mvalid );
  const LineData line2 = calcConicAsymptote( cequation, -1, mvalid );

  if ( mvalid )
  {
    p.setPen( QPen (Qt::red,1) );
    p.drawLine( line1 );
    p.drawLine( line2 );
  };
}

Object::WantArgsResult LineConicAsymptotes::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 ) return NotGood;
  Conic* conic = os[0]->toConic();
  if ( ! conic ) return NotGood;
  if ( conic->conicType() != -1 ) return NotGood;
  return Complete;
}

QString LineConicAsymptotes::sUseText( const Objects&, const Object* )
{
  return i18n("Asymptotes of This Hyperbola");
}
