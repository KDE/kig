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

#include "normalpoint.h"

#include "../misc/hierarchy.h"
#include "../misc/kigpainter.h"

#include <kdebug.h>
#include <klocale.h>

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

      p.setPen( QPen( selected && ss ? Qt::red : mColor ) );
      for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
	{
	  p.drawPoint( i->pt );
	};
    }
  else
  {
    for ( Objects::const_iterator i = objs.begin(); i != objs.end(); ++i )
    {
      (*i)->setSelected(selected);
      (*i)->draw(p, ss);
    }
  };
}

bool Locus::contains(const Coordinate& o, const double fault ) const
{
  if (!isPointLocus())
  {
    for( Objects::const_iterator i = objs.begin(); i != objs.end(); ++i )
      if( (*i)->contains(o, fault)) return true;
  }
  else
  {
    for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
      if( ( i->pt - o ).length() < fault ) return true;
  };
  return false;
}

bool Locus::inRect(const Rect& r) const
{
  if (!isPointLocus())
  {
    for( Objects::const_iterator i = objs.begin(); i != objs.end(); ++i )
      if( (*i)->inRect(r) ) return true;
  }
  else
  {
    for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
      if( r.contains( i->pt ) ) return true;
  };
  return false;
}

QString Locus::wantArg( const Object* o ) const
{
  if (o->toNormalPoint() && o->toNormalPoint()->constrainedImp() && !cp)
    return i18n("Moving point");
  // hack, here wantPoint() also handles non-Point objects, but it
  // doesn't matter...
  return wantPoint();
}

QString Locus::wantPoint() const
{
  if (!obj)
    {
      return i18n("Dependent object");
    };
  return 0;
}

bool Locus::selectArg(Object* o)
{
  if (!cp && (cp = o->toNormalPoint() ) && o->toNormalPoint()->constrainedImp() )
    cp->addChild(this);
  else
  {
    assert (!obj);
    obj = o;
    obj->addChild(this);
    // if obj is a point, we are a pointLocus
    _pointLocus = (bool) obj->toPoint();
    kdDebug() << k_funcinfo << " at line no. " << __LINE__
              << " - _pointLocus: " << _pointLocus << endl;
  };
  if (cp && obj)
  {
    complete = true;
    // construct our hierarchy...
    Objects given;
    given.push_back(cp);
    Objects final;
    final.push_back(obj);
    hierarchy = new ObjectHierarchy( given, final );
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
  if ( ! valid ) return Coordinate();
  if (obj->toPoint())
  {
    double tmp = cp->constrainedImp()->getP();
    cp->constrainedImp()->setP(param);
    hierarchy->calc();
    t= obj->toPoint()->getCoord();
    cp->constrainedImp()->setP(tmp);
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
  if ( ! valid ) return;
  delete_all( objs.begin(), objs.end() );
  objs.clear();
  double oldP = cp->constrainedImp()->getP();
  double period = double(1)/numberOfSamples;
  for (double i = 0; i <= 1; i += period)
    {
      cp->constrainedImp()->setP(i);
      cp->calc();
      hierarchy->calc();
      objs.push_back(obj->copy());
    };
  cp->constrainedImp()->setP(oldP);
  cp->calc();
  hierarchy->calc();
}

inline Locus::CPts::iterator Locus::addPoint(double param)
{
  cp->constrainedImp()->setP(param);
  cp->calc();
  hierarchy->calc();
  Point* p = obj->toPoint();
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
  double oldP = cp->constrainedImp()->getP();
  // TODO: improve (cf. KSeg: choose which samples we want...)
//   double period = double(1)/numberOfSamples;
//   for (double i = 0; i <= 1; i += period)
//     {
//       cp->setP(i);
//       cp->calc();
//       hierarchy->calc();
//       objs.push_back(obj->copy());
//     };
  CPts::iterator b = addPoint(0);
  CPts::iterator e = addPoint(1);
  int i = 2;
  recurse(b,e,i,r);
  // reset cp and its children to their former state...
  cp->constrainedImp()->setP(oldP);
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

Objects Locus::getParents() const
{
  Objects tmp;
  tmp.push_back(cp);
  tmp.push_back(obj);
  return tmp;
}

Locus::Locus()
 : cp(0), obj(0), hierarchy(0)
{
};

Locus::~Locus()
{
  delete_all( objs.begin(), objs.end() );
}

Locus* Locus::copy()
{
  return new Locus(*this);
}

const QCString Locus::vBaseTypeName() const
{
  return sBaseTypeName();
}

const QCString Locus::sBaseTypeName()
{
  return I18N_NOOP("curve");
}

const QCString Locus::vFullTypeName() const
{
  return sFullTypeName();
}

const QCString Locus::sFullTypeName()
{
  return "Curve";
}

const QString Locus::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString Locus::sDescriptiveName()
{
  return i18n("Locus");
}

const QString Locus::vDescription() const
{
  return sDescription();
}

const QString Locus::sDescription()
{
  return i18n( "Construct a locus: let one point move around, and record "
               "the places another object passes through. These combined "
               "form a new object: the locus..." );
}

const QCString Locus::vIconFileName() const
{
  return sIconFileName();
}

const QCString Locus::sIconFileName()
{
  return "locus";
}

const int Locus::vShortCut() const
{
  return sShortCut();
}

const int Locus::sShortCut()
{
  return 0;
}

void Locus::drawPrelim( KigPainter&, const Object* ) const
{
}

void Locus::startMove(const Coordinate&)
{
}

void Locus::moveTo(const Coordinate&)
{
}

void Locus::stopMove()
{
}

bool Locus::isPointLocus() const
{
  return _pointLocus;
}
