// midpoint.cc
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

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

#include "midpoint.h"

#include "segment.h"

#include <klocale.h>

MidPoint::MidPoint()
 :p1(0), p2(0)
{
};

MidPoint::MidPoint(const MidPoint& m)
  : Point()
{
  p1 = m.p1;
  p1->addChild(this);
  p2 = m.p2;
  p2->addChild(this);
  complete = m.complete;
  if (complete) calc();
}

MidPoint::~MidPoint()
{
}

QString MidPoint::wantArg(const Object* o) const
{
  if ( o->toSegment() )
    {
      if(!p1 && !p2) return i18n("On segment");
      else return 0;
    };
  if ( ! o->toPoint() ) return 0;
  if (!p1) return i18n("First point");
  else if (!p2) return i18n("Second point");
  return 0;
};

bool MidPoint::selectArg(Object* o)
{
  Segment* s;
  if ((s = o->toSegment() ) )
    {
      assert (!(p1 ||p2));
      selectArg(s->getPoint1());
      return selectArg(s->getPoint2());
    };
  // if we get here, o should be a point
  Point* p = o->toPoint();
  assert (p);

  if (!p1) p1 = p;
  else if (!p2)
    {
      p2 = p;
      complete = true;
    }
  else { kdError() << k_funcinfo << " selectArg on a complete midpoint... " << endl; return true; };
  o->addChild(this);
  return complete;
};

void MidPoint::startMove(const Coordinate& p)
{
  if (contains(p,false))
    {
      howm = howmMoving;
      p2->startMove(p2->getCoord());
    }
  else howm = howmFollowing;
}

void MidPoint::moveTo(const Coordinate& p)
{
  if (howm == howmFollowing)
    {
      calc();
      return;
    }
  mC = p;
  p2->moveTo( mC*2-p1->getCoord() );
}

void MidPoint::stopMove()
{
};

void MidPoint::calc()
{
  assert (p1 && p2);
  setX(((p1->getX() + p2->getX())/2));
  setY(((p1->getY() + p2->getY())/2));
}

const QString MidPoint::sDescriptiveName()
{
  return i18n("Midpoint");
}

const QString MidPoint::sDescription()
{
  return i18n( "The midpoint of a segment or of two other points" );
}

const char* MidPoint::sActionName()
{
  return "objects_new_midpoint";
}
