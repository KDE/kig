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

#include "intersection.h"

#include "segment.h"
#include "line.h"
#include "circle.h"

#include "../modes/constructing.h"
#include "../misc/kigpainter.h"
#include "../misc/common.h"
#include "../misc/i18n.h"

#include <kdebug.h>

Objects IntersectionPoint::getParents() const
{
  Objects tmp;
  if (segment1) tmp.push_back(segment1);
  if (segment2) tmp.push_back(segment2);
  if (line1) tmp.push_back(line1);
  if (line2) tmp.push_back(line2);
  return tmp;
};

void IntersectionPoint::calc( const ScreenInfo& )
{
  Coordinate t1, t2, t3, t4;
  bool gotfirst = false;
  if ( segment1 )
  {
    if ( gotfirst )
    {
      t3 = segment1->getP1();
      t4 = segment1->getP2();
    }
    else
    {
      t1 = segment1->getP1();
      t2 = segment1->getP2();
      gotfirst = true;
    };
  };
  if ( segment2 )
  {
    if ( gotfirst )
    {
      t3 = segment2->getP1();
      t4 = segment2->getP2();
    }
    else
    {
      t1 = segment2->getP1();
      t2 = segment2->getP2();
      gotfirst = true;
    };
  };
  if ( line1 )
  {
    if ( gotfirst )
    {
      t3 = line1->getP1();
      t4 = line1->getP2();
    }
    else
    {
      t1 = line1->getP1();
      t2 = line1->getP2();
      gotfirst = true;
    };
  };
  if ( line2 )
  {
    if ( gotfirst )
    {
      t3 = line2->getP1();
      t4 = line2->getP2();
    }
    else
    {
      t1 = line2->getP1();
      t2 = line2->getP2();
      gotfirst = true;
    };
  };
  mC = calcIntersectionPoint( t1, t2, t3, t4 );
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
    segment1( p.segment1 ), segment2( p.segment2 ),
    line1( p.line1 ), line2( p.line2 )
{
  if ( segment1 ) segment1->addChild( this );
  if ( segment2 ) segment2->addChild( this );
  if ( line1 ) line1->addChild( this );
  if ( line2 ) line2->addChild( this );
};

IntersectionPoint::~IntersectionPoint()
{
}

IntersectionPoint* IntersectionPoint::copy()
{
  return new IntersectionPoint( *this );
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

void IntersectionPoint::startMove(const Coordinate& )
{
}

void IntersectionPoint::moveTo(const Coordinate& )
{
}

void IntersectionPoint::stopMove()
{
}

const char* IntersectionPoint::sActionName()
{
  return "objects_new_intersectionpoint";
}

IntersectionPoint::IntersectionPoint( const Objects& os )
  : segment1( 0 ), segment2( 0 ), line1( 0), line2( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( (*i)->toSegment() )
    {
      if ( ! segment1 ) segment1 = (*i)->toSegment();
      else if ( ! segment2 ) segment2 = (*i)->toSegment();
      else assert( false );
    }
    else if ( (*i)->toLine() )
    {
      if ( ! line1 ) line1 = (*i)->toLine();
      else if ( ! line2 ) line2 = (*i)->toLine();
      else assert( false );
    }
    else assert( false );
  };
  if ( segment1 ) segment1->addChild( this );
  if ( segment2 ) segment2->addChild( this );
  if ( line1 ) line1->addChild( this );
  if ( line2 ) line2->addChild( this );
}

void IntersectionPoint::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Segment* s = 0;
  Line* l = 0;
  Coordinate pa, pb, pc, pd;
  bool gota = false;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ( s = (*i)->toSegment() ) )
    {
      if ( gota )
      {
        pc = s->getP1();
        pd = s->getP2();
      }
      else
      {
        pa = s->getP1();
        pb = s->getP2();
        gota = true;
      }
    }
    else if ( ( l = (*i)->toLine() ) )
    {
      if ( gota )
      {
        pc = l->getP1();
        pd = l->getP2();
      }
      else
      {
        pa = l->getP1();
        pb = l->getP2();
        gota = true;
      }
    }
    else assert( false );
  };
  p.setPen( QPen( Qt::red, 1 ) );
  p.drawPoint( calcIntersectionPoint( pa, pb, pc, pd ), false );
}

Object::WantArgsResult IntersectionPoint::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if( ! ( (*i)->toSegment() || (*i)->toLine() ) ) return NotGood;
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
    if ( ! mline ) mline = (*i)->toLine();
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

CircleLineIntersectionPoint* CircleLineIntersectionPoint::copy()
{
  return new CircleLineIntersectionPoint( *this );
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
  return 0;
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
  Line* l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! c ) c = (*i)->toCircle();
    if ( ! l ) l = (*i)->toLine();
  };
  assert( c && l );
  const Coordinate center = c->getCenter();
  const double sqr = c->squareRadius();
  const Coordinate a = l->getP1();
  const Coordinate b = l->getP2();
  bool valid = true;
  const Coordinate d = calcCircleLineIntersect( center, sqr, a, b,
                                                true, valid );
  const Coordinate e = calcCircleLineIntersect( center, sqr, a, b,
                                                false, valid );
  if ( valid )
  {
    p.drawPoint( d, false );
    p.drawPoint( e, false );
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
    if ( ! gotl ) gotl = (*i)->toLine();
  };
  if ( size == 1 ) return ( gotc || gotl ) ? NotComplete : NotGood;
  if ( size == 2 ) return ( gotc && gotl ) ? Complete : NotGood;
  assert( false );
}

QString CircleLineIntersectionPoint::sUseText( const Objects&, const Object* o )
{
  if ( o->toCircle() ) return i18n( "Intersection of a line and this circle" );
  if ( o->toLine() ) return i18n( "Intersection of a circle and this line" );
  assert( false );
}

Objects CircleLineIntersectionPoint::getParents() const
{
  Objects o;
  o.push_back( mcircle );
  o.push_back( mline );
  return o;
}

void CircleLineIntersectionPoint::startMove(const Coordinate& )
{
}

void CircleLineIntersectionPoint::moveTo(const Coordinate& )
{
}

void CircleLineIntersectionPoint::stopMove()
{
}

void CircleLineIntersectionPoint::calc( const ScreenInfo& )
{
  assert( mcircle && mline );
  Coordinate t;
  t = calcCircleLineIntersect( mcircle->getCenter(), mcircle->squareRadius(),
                               mline->getP1(), mline->getP2(), mside, valid );
  if ( valid ) mC = t;
}

std::map<QCString,QString> CircleLineIntersectionPoint::getParams()
{
  std::map<QCString,QString> map = Point::getParams();
  map["circlelineintersect-side"] = mside ? QString::fromUtf8( "first" )
                                          : QString::fromUtf8( "second" );
  return map;
}

void CircleLineIntersectionPoint::setParams( const std::map<QCString,QString>& map )
{
  Point::setParams( map );
  std::map<QCString,QString>::const_iterator p = map.find("circlelineintersect-side");
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
