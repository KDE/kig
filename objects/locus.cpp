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


#include "locus.h"

#include "../misc/hierarchy.h"

#include <kdebug.h>

/**
 * Locus: the calc routines of this class are a bit unusual:
 * if( !isPointLocus() ) all is as usual...
 * but otherwise, we try to find better points by keeping the window()
 * from the last draw, and only taking the points which are in that
 * rect...
 */

void Locus::draw(KigPainter& p, bool ss) const
{
  calcRect = p.window();
  // we don't let the points draw themselves, since that would get us
  // the big points (5x5) like we normally see them...
  // all objs are of the same type, so we only check the first
  if (isPointLocus())
    {
      kdDebug() << k_funcinfo << " at line no. " << __LINE__ << " - count: " << pts.size()<< endl;

      QPen pen (selected && ss ? Qt::red : Qt::blue);
      p.setPen(pen);
      for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
	{
	  p.drawPoint( i->pt );
	};
    }
  else
    {
      Object* i;
      for ( Objects::iterator it(objs); (i = it.current()); ++it)
	{
	  i->setSelected(selected);
	  i->draw(p, ss);
	}
    };
}

bool Locus::contains(const Coordinate& o, const double fault ) const
{
  if (!isPointLocus())
    {
      Object* i;
      for (Objects::iterator it(objs); (i = it.current()); ++it)
	{
	  if (i->contains(o, fault)) return true;
	};
    }
  else
    {
      for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
	{
	  if( ( i->pt - o ).length() < fault ) return true;
	};
    };
  return false;
}

bool Locus::inRect(const Rect& r) const
{
  if (!isPointLocus())
    {
      Object* i;
      for (Objects::iterator it(objs); (i = it.current()); ++it)
	{
	  if (i->inRect(r)) return true;
	};
    }
  else 
    {
      for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
	{
	  if( r.contains( i->pt ) ) return true;
	};
    };
  return false;
}

QString Locus::wantArg( const Object* o ) const
{
  if (toConstrainedPoint(o) && !cp)
    {
      return i18n("Moving point");
    };
  if (!obj)
    {
      return i18n("Dependent object");
    };
  return 0;
}

bool Locus::selectArg(Object* o)
{
  if (!cp && (cp = toConstrainedPoint(o)))
    {
      cp->addChild(this);
    }
  else 
    {
      assert (!obj);
      obj = o;
      obj->addChild(this);
      // if obj is a point, we are a pointLocus
      _pointLocus = (bool) toPoint(obj);
      kdDebug() << k_funcinfo << " at line no. " << __LINE__
		<< " - _pointLocus: " << _pointLocus << endl;
    };
  if (cp && obj)
    {
      complete = true;
      // construct our hierarchy...
      Objects given;
      given.append(cp);
      Objects final;
      final.append(obj);
      hierarchy = new ObjectHierarchy(given, final, 0);
      // calc...
      calc();
    }
  return complete;
}

void Locus::realCalc( const Rect& r )
{
  if( isPointLocus() ) calcPointLocus( r );
  else calcObjectLocus();
};

void Locus::calc()
{
  realCalc( calcRect );
}

Coordinate Locus::getPoint(double param) const
{
  Coordinate t;
  if (toPoint(obj))
    {
      double tmp = cp->getP();
      cp->setP(param);
      hierarchy->calc();
      t=toPoint(obj)->getCoord();
      cp->setP(tmp);
      hierarchy->calc();
    }
  else t = Coordinate();
  return t;
}

double Locus::getParam(const Coordinate&) const
{
  return 0.5;
}

void Locus::calcObjectLocus()
{
  objs.deleteAll();
  objs.clear();
  double oldP = cp->getP();
  double period = double(1)/numberOfSamples;
  for (double i = 0; i <= 1; i += period)
    {
      cp->setP(i);
      cp->calc();
      hierarchy->calc();
      objs.append(obj->copy());
    };
  cp->setP(oldP);
  cp->calc();
  hierarchy->calc();
}

inline Locus::CPts::iterator Locus::addPoint(double param)
{
  cp->setP(param);
  cp->calc();
  hierarchy->calc();
  Point* p = Object::toPoint(obj);
  pts.push_front(CPt(p->getCoord(), param));
  return pts.begin();
}

void Locus::recurse(CPts::iterator first, CPts::iterator last, int& i, const Rect& r )
{
  if ( i++ > numberOfSamples ) return;
  if( !( r.contains( first->pt ) || r.contains( last->pt ) ) ) return;
  double p = (first->pm+last->pm)/2;
  CPts::iterator n = addPoint(p);
  if( ( n->pt - first->pt ).length() > 3 ) recurse( n, first, i, r );
  if (i > numberOfSamples) return;
  if( ( n->pt - last->pt ).length() > 3 ) recurse( n, last, i, r );
  if (i > numberOfSamples) return;
}

void Locus::calcPointLocus( const Rect& r )
{
  kdDebug() << k_funcinfo << " at line no. " << __LINE__ << endl;
  pts.clear();
  double oldP = cp->getP();
  // TODO: improve (cf. KSeg: choose which samples we want...)
//   double period = double(1)/numberOfSamples;
//   for (double i = 0; i <= 1; i += period)
//     {
//       cp->setP(i);
//       cp->calc();
//       hierarchy->calc();
//       objs.append(obj->copy());
//     };
  CPts::iterator b = addPoint(0);
  CPts::iterator e = addPoint(1);
  int i = 2;
  recurse(b,e,i,r);
  // reset cp and its children to their former state...
  cp->setP(oldP);
  cp->calc();
  hierarchy->calc();
  kdDebug() << k_funcinfo << " at line no. " << __LINE__ << "  - count: " << pts.size()<< endl;
}

Locus::Locus(const Locus& loc)
  : Curve(), cp(loc.cp), obj(loc.obj), hierarchy(loc.hierarchy)
{
  cp->addChild(this);
  obj->addChild(this);
  complete = loc.complete;
}
