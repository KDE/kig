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


#include "locus.h"

#include "normalpoint.h"

#include "../kig/kig_view.h"
#include "../misc/calcpaths.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"

#include <stack>

#include <kdebug.h>

void Locus::draw(KigPainter& p, bool ss) const
{
  // here, we simply draw the set of points we calced in the
  // calcForWidget() function...
  p.setPen( QPen( selected && ss ? Qt::red : mColor ) );
  for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
    p.drawPoint( i->pt );
}

bool Locus::contains(const Coordinate& o, const ScreenInfo& si ) const
{
  for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
    if( ( i->pt - o ).length() < si.normalMiss() ) return true;
  return false;
}

bool Locus::inRect(const Rect& r) const
{
  for (CPts::const_iterator i = pts.begin(); i != pts.end(); ++i)
    if( r.contains( i->pt ) ) return true;
  return false;
}

void Locus::calcForWidget( const KigWidget& w )
{
  mvalid = cp->valid() && mp->valid();
  if ( mvalid )
  {
    // i exchanged the previous recursing algorithm for a stack-based
    // one that should be faster...  Here is the stack we use...
    typedef std::pair<CPts::iterator, CPts::iterator> iterpair;
    std::stack<iterpair> stack;

    // some initial work...
    // clear the previous state
    pts.clear();
    pts.reserve( numberOfSamples );
    // save cp's old parameter, as we'll be changing it...
    double oldP = cp->constrainedImp()->getP();

    // add 17 initial points...
    for ( double i = 0; i <= 1; i += 1./16 )
    {
      Coordinate c = internalGetCoord( i );
      pts.push_back( CPt( c, i ) );
      if ( i != 0 ) stack.push( iterpair( pts.end() - 2, pts.end() - 1 ) );
    };

    int i = 17;
    // maxlength is the square of the maximum size that we allow
    // between two points..
    double maxlength = 1.5 * w.pixelWidth();
    maxlength *= maxlength;

    while ( ! stack.empty() && i < numberOfSamples )
    {
      iterpair current = stack.top();
      stack.pop();
      const Rect& sr = w.screenInfo().shownRect();
      for( ; i < numberOfSamples; ++i )
      {
        double p = ( current.first->pm + current.second->pm ) / 2;
        Coordinate n = internalGetCoord( p );
        bool addn = sr.contains( n );
        bool followfirst = addn &&
                           ( n - current.first->pt ).squareLength() > maxlength &&
                           sr.contains( current.first->pt );
        bool followlast = addn &&
                          ( n - current.second->pt ).squareLength() > maxlength &&
                          sr.contains( current.second->pt );
        if ( addn ) pts.push_back( CPt( n, p ) );
        if ( followfirst ) stack.push( iterpair( current.first, pts.end() - 1 ) );
        if ( followlast ) current.first = pts.end() - 1;
        else break;
      };
    };

    // reset cp and its children to their former state...
    cp->constrainedImp()->setP(oldP);
    cp->calcForWidget( w );
    calcpath.calcForWidget( w );
  };
}

Coordinate Locus::getPoint( double param ) const
{
  if ( ! mvalid ) return Coordinate();

  // save the old param..
  double tmp = cp->constrainedImp()->getP();

  // calc the new coord..
  cp->constrainedImp()->setP(param);
  cp->calc();
  calcpath.calc();
  Coordinate t = mp->getCoord();

  // restore the old param...
  cp->constrainedImp()->setP(tmp);
  cp->calc();
  calcpath.calc();
  return t;
}

double Locus::getParam( const Coordinate& p ) const
{
  double optimalparam = 0.5;
  double optimaldistance = 10000000;
  for ( CPts::const_iterator i = pts.begin(); i != pts.end(); ++i )
  {
    double dist = ( i->pt - p ).length();
    if ( dist < optimaldistance )
    {
      optimalparam = i->pm;
      optimaldistance = dist;
    };
  };
  return optimalparam;
}

Coordinate Locus::internalGetCoord( double param )
{
  cp->constrainedImp()->setP(param);
  cp->calc();
  calcpath.calc();
  return mp->getCoord();
}

Locus::Locus(const Locus& loc)
  : Curve(), cp(loc.cp), mp(loc.mp)
{
  cp->addChild(this);
  mp->addChild(this);
  calcpath = calcPath( Objects( cp ), mp );
  // we always want to calc obj after its arguments...
  calcpath.push_back( mp );
}

Objects Locus::getParents() const
{
  Objects tmp;
  tmp.push_back(cp);
  tmp.push_back(mp);
  return tmp;
}

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
  return "Locus";
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

const char* Locus::sActionName()
{
  return "objects_new_locus";
}

Locus::Locus( const Objects& os )
  : Curve(), cp( 0 ), mp( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( !cp && (*i)->toNormalPoint() && (*i)->toNormalPoint()->constrainedImp() )
      cp = (*i)->toNormalPoint();
    else
    {
      mp = (*i)->toPoint();
      assert( mp );
    }
  };
  assert ( cp && mp );
  cp->addChild( this );
  mp->addChild( this );

  calcpath = calcPath( Objects( cp ), mp );
  calcpath.push_back( mp );
};

Object::WantArgsResult Locus::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  bool gotcp = false;
  bool gotmp = false;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( !gotcp && (*i)->toNormalPoint()
         && (*i)->toNormalPoint()->constrainedImp() )
      gotcp = true;
    else if ( (*i)->toPoint() ) gotmp = true;
  };
  if ( size == 1 ) return ( gotmp || gotcp ) ? NotComplete : NotGood;
  assert( size == 2 );
  return ( gotmp && gotcp ) ? Complete : NotGood;
}

QString Locus::sUseText( const Objects&, const Object* o )
{
  if (o->toNormalPoint() && o->toNormalPoint()->constrainedImp() )
    return i18n("Moving point");
  return i18n("Dependent object");
}

void Locus::sDrawPrelim( KigPainter&, const Objects& )
{
}

void Locus::calc()
{
}
