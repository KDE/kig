/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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

#include "intersection.h"

#include "abstractline.h"

#include "circle.h"

#include "../modes/constructing.h"
#include "../misc/common.h"
#include "../misc/i18n.h"

#include <kdebug.h>

Objects IntersectionPoint::getParents() const
{
  Objects tmp;
  tmp.push_back( mla );
  tmp.push_back( mlb );
  return tmp;
};

void IntersectionPoint::calc()
{
  mvalid = true;
  mvalid &= mla->valid();
  mvalid &= mlb->valid();
  if ( mvalid ) mC = calcIntersectionPoint( mla->lineData(), mlb->lineData() );
}

const QString IntersectionPoint::sDescriptiveName()
{
  return i18n("Intersection Point");
}

const QString IntersectionPoint::sDescription()
{
  return i18n( "The point where two lines or segments intersect" );
}

IntersectionPoint::IntersectionPoint( const IntersectionPoint& p )
  : Point( p ),
    mla( p.mla ), mlb( p.mlb )
{
  mla->addChild( this );
  mlb->addChild( this );
};

IntersectionPoint::~IntersectionPoint()
{
}

const QCString IntersectionPoint::vFullTypeName() const
{
  return sFullTypeName();
}

const QCString IntersectionPoint::sFullTypeName()
{
  return "IntersectionPoint";
}

const QString IntersectionPoint::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString IntersectionPoint::vDescription() const
{
  return sDescription();
}

const QCString IntersectionPoint::vIconFileName() const
{
  return sIconFileName();
}

const QCString IntersectionPoint::sIconFileName()
{
  return "intersection";
}

const int IntersectionPoint::vShortCut() const
{
  return sShortCut();
}

const int IntersectionPoint::sShortCut()
{
  return CTRL+Key_I;
}

const char* IntersectionPoint::sActionName()
{
  return "objects_new_intersectionpoint";
}

IntersectionPoint::IntersectionPoint( const Objects& os )
  : mla( 0 ), mlb( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    AbstractLine* l = (*i)->toAbstractLine();
    assert( l );
    if ( ! mla ) mla = l;
    else mlb = l;
  };
  mla->addChild( this );
  mlb->addChild( this );
}

void IntersectionPoint::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  LineData la, lb;
  bool gota = false;
  AbstractLine* l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    l = (*i)->toAbstractLine();
    assert( l );
    if ( gota )
      lb = l->lineData();
    else
    {
      gota = true;
      la = l->lineData();
    };
  };
  sDrawPrelimPoint( p, calcIntersectionPoint( la, lb ) );
}

Object::WantArgsResult IntersectionPoint::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if( ! (*i)->toAbstractLine() ) return NotGood;
  };
  return size == 2 ? Complete : NotComplete;
}

QString IntersectionPoint::sUseText( const Objects&, const Object* o )
{
  return i18n("Point on this %1").arg(o->vTBaseTypeName());
}

CircleLineIntersectionPoint::CircleLineIntersectionPoint( const Objects& os )
    : Point(), mcircle( 0 ), mline( 0 ), mside( false )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! mcircle ) mcircle = (*i)->toCircle();
    if ( ! mline ) mline = (*i)->toAbstractLine();
  };
  assert( mcircle && mline );
  mcircle->addChild( this );
  mline->addChild( this );
}

CircleLineIntersectionPoint::CircleLineIntersectionPoint( const CircleLineIntersectionPoint& p )
  : Point( p ), mcircle( p.mcircle ), mline( p.mline ), mside( p.mside )
{
  assert( mcircle && mline );
  mcircle->addChild( this );
  mline->addChild( this );
}

CircleLineIntersectionPoint::~CircleLineIntersectionPoint()
{
}

const QCString CircleLineIntersectionPoint::vFullTypeName() const
{
  return sFullTypeName();
}

const QCString CircleLineIntersectionPoint::sFullTypeName()
{
  return "CircleLineIntersectionPoint";
}

const QString CircleLineIntersectionPoint::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString CircleLineIntersectionPoint::sDescriptiveName()
{
  return i18n("The intersection of a line and a circle");
}

const QString CircleLineIntersectionPoint::vDescription() const
{
  return sDescription();
}

const QCString CircleLineIntersectionPoint::vIconFileName() const
{
  return sIconFileName();
}

const QCString CircleLineIntersectionPoint::sIconFileName()
{
  return "circlelineintersection";
}

const int CircleLineIntersectionPoint::vShortCut() const
{
  return sShortCut();
}

const int CircleLineIntersectionPoint::sShortCut()
{
  return 0;
}

const char* CircleLineIntersectionPoint::sActionName()
{
  return "objects_new_circlelineintersect";
}

void CircleLineIntersectionPoint::sDrawPrelim( KigPainter& p, const Objects& os )
{
  assert( os.size() <= 2 );
  if ( os.size() != 2 ) return;
  Circle* c = 0;
  AbstractLine* l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! c ) c = (*i)->toCircle();
    if ( ! l ) l = (*i)->toAbstractLine();
  };
  assert( c && l );
  const Coordinate center = c->center();
  const double sqr = c->squareRadius();
  const LineData ld = l->lineData();
  bool valid = true;
  const Coordinate d = calcCircleLineIntersect( center, sqr, ld,
                                                true, valid );
  const Coordinate e = calcCircleLineIntersect( center, sqr, ld,
                                                false, valid );
  if ( valid )
  {
    sDrawPrelimPoint( p, d );
    sDrawPrelimPoint( p, e );
  };
}

Object::WantArgsResult CircleLineIntersectionPoint::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  bool gotc = false;
  bool gotl = false;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! gotc ) gotc = (*i)->toCircle();
    if ( ! gotl ) gotl = (*i)->toAbstractLine();
  };
  if ( size == 1 ) return ( gotc || gotl ) ? NotComplete : NotGood;
  if ( size == 2 ) return ( gotc && gotl ) ? Complete : NotGood;
  assert( false );
}

QString CircleLineIntersectionPoint::sUseText( const Objects&, const Object* o )
{
  if ( o->toCircle() ) return i18n( "Intersection of a line and this circle" );
  if ( o->toLine() ) return i18n( "Intersection of a circle and this line" );
  if ( o->toSegment() ) return i18n( "Intersection of a circle and this segment" );
  if ( o->toRay() ) return i18n( "Intersection of a circle and this ray" );
  assert( false );
}

Objects CircleLineIntersectionPoint::getParents() const
{
  Objects o;
  o.push_back( mcircle );
  o.push_back( mline );
  return o;
}

void CircleLineIntersectionPoint::calc()
{
  assert( mcircle && mline );
  Coordinate t;
  t = calcCircleLineIntersect( mcircle->center(), mcircle->squareRadius(),
                               mline->lineData(), mside, mvalid );
  if ( mvalid ) mC = t;
}

Object::prop_map CircleLineIntersectionPoint::getParams()
{
  prop_map map = Point::getParams();
  map["circlelineintersect-side"] = mside ? QString::fromUtf8( "first" )
                                          : QString::fromUtf8( "second" );
  return map;
}

void CircleLineIntersectionPoint::setParams( const prop_map& map )
{
  Point::setParams( map );
  prop_map::const_iterator p = map.find("circlelineintersect-side");
  if( p == map.end() ) mside = true;  // fixme: i'd like to throw an
                                    // error here..
  else
    mside = p->second == QString::fromUtf8( "first" ) ? true : false;
}

Objects CircleLineIntersectionPoint::sMultiBuild( const Objects& args )
{
  CircleLineIntersectionPoint* a;
  CircleLineIntersectionPoint* b;
  a = new CircleLineIntersectionPoint( args );
  b = new CircleLineIntersectionPoint( args );
  a->mside = true;
  b->mside = false;
  Objects o;
  o.push_back( a );
  o.push_back( b );
  return o;
}

KigMode*
CircleLineIntersectionPoint::sConstructMode( MultiConstructibleType* ourtype,
                                             KigDocument* theDoc,
                                             NormalMode* previousMode )
{
  return new MultiConstructionMode( ourtype, previousMode, theDoc );
}

const QString CircleLineIntersectionPoint::sDescription()
{
  return i18n("Construct the intersection of a line and a circle...");
}
