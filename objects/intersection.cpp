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

#include <kdebug.h>
#include <klocale.h>

QString IntersectionPoint::wantArg(const Object* o) const
{
  if (o->vBaseTypeName() == Point::sBaseTypeName()) return 0;
  int tmp = 0;
  if (segment1) ++tmp;
  if (segment2) ++tmp;
  if (line1) ++tmp;
  if (line2) ++tmp;
  if (circle1) ++tmp;
  if (circle2) ++tmp;
  if (tmp < 2) return i18n("On %1").arg(o->vTBaseTypeName());
  else return 0;
}

bool IntersectionPoint::selectArg(Object* o)
{
  Segment* s;
  Line* l;
  Circle* c;
  if ((s = o->toSegment())) {
    if (segment1) segment2 = s;
    else segment1 = s;
  }
  else if ((l = o->toLine())) {
    if (line1) line2 = l;
    else line1 = l;
  }
  else if ((c = o->toCircle())) {
    if (circle1) circle2 = c;
    else circle1 = c;
  }
  else {
    kdError() << k_funcinfo << " wrong argument: " << o->vBaseTypeName() <<endl;
    // shouldn't happen...
    assert( false );
  };
  o->addChild(this);
  int tmp = 0;
  if (segment1) ++tmp;
  if (segment2) ++tmp;
  if (line1) ++tmp;
  if (line2) ++tmp;
  if (circle1) ++tmp;
  if (circle2) ++tmp;
  if (tmp > 1) return complete = true;
  return complete = false;
}

void IntersectionPoint::unselectArg(Object* o)
{
  Segment* s;
  Line* l;
  Circle* c;
  if ((s = o->toSegment())) {
    if (segment1 == s) segment1 = segment2;
    segment2 = 0;
  }
  else if ((l = o->toLine())) {
    if (line1 == l) line1 = line2;
    line2 = 0;
  }
  else if ((c = o->toCircle())) {
    if (circle1 == c) circle1 = circle2;
    circle2 = 0;
  }
  else {
    kdError() << k_funcinfo << "wrong argument: " << o->vBaseTypeName() <<endl;
    return;
  };
  complete = false;
}
Objects IntersectionPoint::getParents() const
{
  Objects tmp;
  if (segment1) tmp.push_back(segment1);
  if (segment2) tmp.push_back(segment2);
  if (line1) tmp.push_back(line1);
  if (line2) tmp.push_back(line2);
  if (circle1) tmp.push_back(circle1);
  if (circle2) tmp.push_back(circle2);
  return tmp;
};

void IntersectionPoint::calc()
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
    line1( p.line1 ), line2( p.line2 ),
    circle1( p.circle1 ), circle2( p.circle2 )
{
};

IntersectionPoint::IntersectionPoint()
  : segment1(0), segment2(0),
    line1(0), line2(0),
    circle1(0), circle2(0)
{
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

void IntersectionPoint::cancelMove()
{
}

void IntersectionPoint::drawPrelim( KigPainter&, const Object* ) const
{
}

