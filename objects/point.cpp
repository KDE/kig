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


#include "point.h"

#include <kdebug.h>

#include <typeinfo>

#include "segment.h"
#include "curve.h"

bool Point::contains( const Coordinate& o, const double error ) const
{
  return (o - mC).length() <= error;
};

void Point::draw (KigPainter& p, bool ss) const
{
  kdDebug() << k_funcinfo << mC.x << " " << mC.y << endl;
  if (!shown) return;
  bool s = selected && ss;
  p.setBrushStyle( Qt::SolidPattern );
  p.setBrushColor( s ? Qt::red : mColor );
  p.setPen( QPen ( s ? Qt::red : mColor, 1 ) );
  p.drawPoint( mC, false );
  p.setBrush (Qt::NoBrush);
};

void Point::startMove(const Coordinate& p)
{
  pwwlmt = p;
};

void Point::moveTo(const Coordinate& p)
{
  mC+=p-pwwlmt;
  pwwlmt = p;
};

void Point::stopMove()
{
};

QString MidPoint::wantArg(const Object* o) const
{
  if (toSegment(o))
    {
      if(!p1 && !p2) return i18n("On segment");
      else return 0;
    };
  if (!toPoint(o)) return 0;
  if (!p1) return i18n("First point");
  else if (!p2) return i18n("Second point");
  return 0;
};

bool MidPoint::selectArg(Object* o)
{
  Segment* s;
  if ((s = toSegment(o)))
    {
      assert (!(p1 ||p2));
      selectArg(s->getPoint1());
      return selectArg(s->getPoint2());
    };
  // if we get here, o should be a point
  Point* p = toPoint(o);
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

void MidPoint::unselectArg(Object* o)
{
  if (o == p1)
    {
      p1=p2;
      p2=0;
    }
  else if (o == p2)
    {
      p2 = 0;
    }
  o->delChild(this);
};

void MidPoint::startMove(const Coordinate& p)
{
  pwwlmt = p;
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

ConstrainedPoint::ConstrainedPoint(Curve* inC, const Coordinate& inPt)
  : c(inC)
{
  c->addChild(this);
  p = c->getParam(inPt);
  kdDebug() << k_funcinfo << "param: "<< p << endl;
  calc();
}

void ConstrainedPoint::calc()
{
  if (!c) return;
  Point::operator=(c->getPoint(p));
}

Objects ConstrainedPoint::getParents() const
{
  Objects tmp;
  tmp.append(c);
  return tmp;
}

void ConstrainedPoint::moveTo(const Coordinate& pt)
{
  p = c->getParam(pt);
  calc();
}

bool ConstrainedPoint::selectArg( Object* o)
{
  if (!c) c = Object::toCurve(o);
  c->addChild( this );
  return c;
}
QString ConstrainedPoint::wantArg(const Object* o) const
{
  if (!c && Object::toCurve(o)) return i18n("On Curve");
  return 0;
}
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
ConstrainedPoint::ConstrainedPoint( const ConstrainedPoint& cp)
  : Point()
{
  p = cp.p;
  c = cp.c;
  c->addChild(this);
  complete = cp.complete;
  if (complete) calc();
}

std::map<QCString,QString> Point::getParams()
{
  std::map<QCString,QString> tmp = Object::getParams();
  tmp["x"] = QString::number(mC.x);
  tmp["y"] = QString::number(mC.y);
  return tmp;
}

void Point::setParams(const std::map<QCString,QString>& m)
{
  Object::setParams( m );
  bool ok;
  mC.x = m.find("x")->second.toDouble(&ok);
  mC.y = m.find("y")->second.toDouble(&ok);
  Q_ASSERT( ok );
};
std::map<QCString,QString> ConstrainedPoint::getParams()
{
  std::map<QCString,QString> tmp = Object::getParams();
  tmp["param"] = QString::number(p);
  return tmp;
}
void ConstrainedPoint::setParams(const std::map<QCString,QString>& m)
{
  Object::setParams( m );
  bool ok;
  p = m.find("param")->second.toDouble(&ok);
  Q_ASSERT(ok);
}

ConstrainedPoint::ConstrainedPoint(const double inP)
  : p(inP), c(0)
{
};

ConstrainedPoint::ConstrainedPoint()
  : p(0.5), c(0)
{
    
}

