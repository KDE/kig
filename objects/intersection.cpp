/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese

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
#include "circle.h"
#include "line.h"

#include "../misc/kigpainter.h"

#include <kdebug.h>
#include <klocale.h>

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
