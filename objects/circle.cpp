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

#include "circle.h"

#include "point.h"

#include "../misc/common.h"
#include "../misc/kigpainter.h"

#include <klocale.h>
#include <kdebug.h>
#include <qpen.h>
#include <math.h>

Circle::Circle()
{
};

Circle::~Circle()
{
};

bool Circle::contains (const Coordinate& o, const double miss) const
{
  return fabs((qpc - Coordinate(o)).length() - radius) <= miss;
};

void Circle::draw (KigPainter& p, bool ss) const
{
  p.setPen(QPen ( selected && ss ? Qt::red : mColor, 1));
  p.drawCircle( qpc, radius);
};

bool Circle::inRect (const Rect& /*r*/) const
{
  // not implemented yet, i'm thinking: take the diagonals of the
  // rect, their intersections with the circle, and check their
  // positions...
  return false;
};

Coordinate Circle::getPoint (double p) const
{
  return qpc + Coordinate (cos(p * 2 * M_PI), sin(p * 2 * M_PI)) * radius;
};

double Circle::getParam (const Coordinate& p) const
{
  Coordinate tmp = p - qpc;
  return fmod(atan2(tmp.y, tmp.x) / ( 2 * M_PI ) + 1, 1);
};

CircleBCP::CircleBCP()
  : poc (0), centre(0)
{
};

QString CircleBCP::wantArg (const Object* o) const
{
  if (complete) return 0;
  if (! o->toPoint()) return 0;
  if (!centre) return i18n("Center point");
  assert (!poc); // we're not completed, and do have a center...
  return i18n("Through point");
};

bool CircleBCP::selectArg (Object* o)
{
  assert (!complete);
  Point* p;
  assert ((p=o->toPoint()));
  if(centre) poc = p;
  else centre = p;
  o->addChild(this);
  if(poc) { return complete = true;};
  return false;
};

void CircleBCP::unselectArg(Object* o)
{
  assert (centre == o);
  o->delChild(this);
  centre = 0;
};

void CircleBCP::startMove(const Coordinate& p)
{
  if (centre->contains(p, false))
    {
      wawm = notMoving;
    }
  else if (poc->contains(p, false)) // we let poc do the work, which will be faster too
    {
      wawm = lettingPocMove;
      poc->startMove(p);
    }
  else
    {
      wawm=movingPoc;
      pwpsm = Coordinate(poc->getX(), poc->getY());
      poc->startMove(pwpsm);
    };
};

void CircleBCP::moveTo(const Coordinate& p)
{
  if (wawm == lettingPocMove)
    {
      poc->moveTo(p);
    }
  else if (wawm == movingPoc)
    {
      double nRadius = calcRadius(centre,p);
      Coordinate nPoc= centre->getCoord() + (poc->getCoord()-centre->getCoord())*(nRadius/radius);
      poc->moveTo(nPoc);
    };
  calc();
};

void CircleBCP::stopMove()
{
  wawm = notMoving;
};

void CircleBCP::cancelMove()
{
};

void CircleBCP::calc()
{
  if (poc && centre)
    {
      radius = calcRadius(centre,poc);
      qpc = centre->getCoord();
    };
};

QString CircleBTP::wantArg(const Object* o) const
{
  if ( ! o->toPoint() ) return 0;
  if ( !p1 || !p2 || !p3 ) return i18n("Through point");
  else return 0;
}

bool CircleBTP::selectArg(Object* o)
{
  Point* p = o->toPoint();
  assert(p);
  if (!p1) p1 = p;
  else if (!p2) p2 = p;
  else p3 = p;
  o->addChild(this);
  if (p3) { return complete = true; };
  return false;
}

void CircleBTP::unselectArg(Object* o)
{
  assert (p1 == o || p2 == o || p3 == o);
  if (p1 == o)
    p1 = p3;
  else if (p2 == o)
    p2 = p3;
  p3 = 0;
  o->delChild(this);
}

inline double sqr (double x)
{
  return x*x;
};

inline double determinant (double a, double b, double c, double d)
{
  return a*d-b*c;
};

Coordinate CircleBTP::calcCenter( Coordinate a, Coordinate b, Coordinate c )
{
  // this algorithm is written by my brother, Christophe Devriese
  // <oelewapperke@ulyssis.org> ...
  // I don't get it myself :)

  double xdo = b.x-a.x;
  double ydo = b.y-a.y;

  double xao = c.x-a.x;
  double yao = c.y-a.y;

  double a2 = sqr(xdo) + sqr(ydo);
  double b2 = sqr(xao) + sqr(yao);

  double numerator = (xdo * yao - xao * ydo);
  if ( numerator == 0 )
  {
    // problem:  xdo * yao == xao * ydo <=> xdo/ydo == xao / yao
    // this means that the lines ac and ab have the same direction,
    // which means they're the same line..
    // FIXME: i would normally throw an error here, but KDE doesn't
    // use exceptions, so i'm returning a bogus point :(
    return (a+c)/2;
  };
  double denominator = 0.5 / numerator;

  double centerx = a.x - (ydo * b2 - yao * a2) * denominator;
  double centery = a.y + (xdo * b2 - xao * a2) * denominator;

  return Coordinate(centerx, centery);
}

Objects CircleBTP::getParents() const
{
  Objects objs;
  objs.push_back( p1 );
  objs.push_back( p2 );
  objs.push_back( p3 );
  return objs;
}

Objects CircleBCP::getParents() const
{
  Objects objs;
  objs.push_back( centre );
  objs.push_back( poc );
  return objs;
}

void CircleBCP::drawPrelim( KigPainter& p, const Object* arg ) const
{
  if (!centre || !shown) return;
  assert( arg->toPoint() );
  Coordinate c = arg->toPoint()->getCoord();
  p.setPen(QPen (Qt::red, 1));
  p.drawCircle( centre->getCoord(), calcRadius( centre,c ));
};

void CircleBTP::drawPrelim( KigPainter& p, const Object* o ) const
{
  if (!p1 || !shown) return;
  assert( o->toPoint() );
  Coordinate a = p1->getCoord();
  Coordinate b = o->toPoint()->getCoord();
  Coordinate c;
  if (p2) { c=p2->getCoord(); }
  else {
    // we pick the third point so that the three points form a
    // triangle with equal sides...

    // midpoint:
    Coordinate m = ( c + a ) / 2;
    if ( b.y != a.y )
    {
      // direction of the perpend:
      double d = -(b.y-a.x)/(b.y-a.y);

      // length:
      // sqrt( 3 ) == tan( 60° )
      // hypot( ... ) == half the length of the segment |ab|
      double l = sqrt(3) * hypot( a.x - b.x, a.y - b.y ) / 2;

      double dx = sqrt( l / ( pow( d, 2 ) + 1 ) );
      double dy = sqrt( l / ( pow( d, -2 ) + 1 ) );
      if( d < 0 ) dy = -dy;

      c.x = m.x + dx;
      c.y = m.y + dy;
    }
    else
    {
      c.x = m.x;
      c.y = m.y + ( a.x - b.x );
    };
  };
  p.setPen(QPen (Qt::red, 1));
  Coordinate nC = calcCenter( a, b, c );
  p.drawCircle(nC, calcRadius( nC, c ) );
}

void CircleBTP::calc()
{
  Coordinate a = p1->getCoord();
  Coordinate b = p2->getCoord();
  Coordinate c = p3->getCoord();
  // the center coords:
  qpc = calcCenter( a, b, c );
  radius = calcRadius( qpc, c );
};

CircleBCP::CircleBCP(const CircleBCP& c)
  : Circle()
{
  poc = c.poc;
  poc->addChild(this);
  centre = c.centre;
  centre->addChild(this);
  complete = c.complete;
  if (complete) calc();
}
CircleBTP::CircleBTP(const CircleBTP& c)
  : Circle()
{
  p1=c.p1;
  p1->addChild(this);
  p2=c.p2;
  p2->addChild(this);
  p3=c.p3;
  p3->addChild(this);
  complete = c.complete;
  if(complete) calc();
}

double Circle::calcRadius( const Point* c, const Point* p ) const
{
  return (c->getCoord() - p->getCoord()).length();
}

double Circle::calcRadius( const Coordinate& c, const Coordinate& p ) const
{
  return ( c-p).length();
}

double Circle::calcRadius( const Point* c, const Coordinate& p ) const
{
  return (c->getCoord() - p).length();
}

const QCString Circle::sBaseTypeName()
{
  return I18N_NOOP("circle");
}

const QString CircleBCP::sDescriptiveName()
{
  return i18n("Circle by center and point");
}

const QString CircleBCP::sDescription()
{
  return i18n(
    "A circle constructed by its center and a point on its border"
    );
}

const QString CircleBTP::sDescriptiveName()
{
  return i18n("Circle by three points");
}

const QString CircleBTP::sDescription()
{
  return i18n( "A circle constructed through three points" );
}

