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

#include "../kig/kig_part.h"

bool Point::contains( const Coordinate& o, const double error ) const
{
  return (o - mC).length() <= error;
};

void Point::draw (KigPainter& p, bool ss) const
{
  if (!shown) return;
  bool s = selected && ss;
  p.setBrushStyle( Qt::SolidPattern );
  p.setBrushColor( s ? Qt::red : mColor );
  p.setPen( QPen ( s ? Qt::red : mColor, 1 ) );
  p.drawPoint( mC, false );
  p.setBrush (Qt::NoBrush);
};

QString MidPoint::wantArg(const Object* o) const
{
  if ( o->toSegment() )
    {
      if(!p1 && !p2) return i18n("On segment");
      else return 0;
    };
  if ( ! o->toPoint() ) return 0;
  return wantPoint();
};

QString MidPoint::wantPoint() const
{
  if (!p1) return i18n("First point");
  else if (!p2) return i18n("Second point");
  return 0;
}

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

NormalPoint* NormalPoint::copy()
{
  return new NormalPoint( *this );
}

NormalPoint::NormalPoint( NormalPointImp* i )
  : mimp( i )
{
  i->calc( this );
}

NormalPoint::NormalPoint()
  : mimp( 0 )
{
}

NormalPoint::NormalPoint( const NormalPoint& p )
  : Point( p ), mimp( p.mimp->copy( this ) )
{
  mimp->calc( this );
}

NormalPoint::~NormalPoint()
{
  delete mimp;
}

NormalPointImp* NormalPoint::NPImpForCoord( const Coordinate& c,
                                            const KigDocument* d,
                                            double fault )
{
  Objects o = d->whatAmIOn( c, fault );
  Curve* v = 0;
  for ( Objects::iterator i = o.begin(); i != o.end(); ++i )
    if ( ( v = (*i)->toCurve() ) )
      break;
  if ( v ) return new ConstrainedPointImp( c, v );
  else return new FixedPointImp( c );
}

NormalPointImp::~NormalPointImp()
{
}

NormalPointImp::NormalPointImp()
{
}

FixedPointImp::FixedPointImp( const Coordinate& c )
{
  pwwca = c;
}

void NormalPoint::setCoord( const Coordinate& c )
{
  mC = c;
}

void FixedPointImp::calc( NormalPoint* p )
{
  p->setCoord( pwwca );
}

void FixedPointImp::startMove( const Coordinate& c, NormalPoint* p )
{
  pwwca = p->getCoord();
  pwwlmt = c;
}

void FixedPointImp::moveTo( const Coordinate& c, NormalPoint* p )
{
  pwwca += c - pwwlmt;
  calc( p );
  pwwlmt = c;
}

void FixedPointImp::stopMove( NormalPoint* )
{
}

std::map<QCString, QString> NormalPoint::getParams()
{
  std::map<QCString, QString> m = Point::getParams();
  m["impType"] = mimp->type();
  mimp->writeParams( m, this );
  return m;
}

void NormalPoint::setParams( const std::map<QCString, QString>& m )
{
  Point::setParams( m );
  delete mimp;
  std::map<QCString,QString>::const_iterator p = m.find("impType");
  if( p == m.end() )
  {
    kdError() << k_funcinfo << "error in file !" << endl;
    return;
  }
  if ( p->second == FixedPointImp::sType() ) mimp = new FixedPointImp();
  else if ( p->second == ConstrainedPointImp::sType() )
    mimp = new ConstrainedPointImp();
  else
  {
    kdError() << k_funcinfo << "error in file !" << endl;
    mimp = new FixedPointImp();
  }
  mimp->readParams( m, this );
};

void FixedPointImp::writeParams( std::map<QCString, QString>& m, NormalPoint* p )
{
  pwwca = p->getCoord(); // should really not be necessary, but you
                         // never know ... :)
  m["x"] = QString::number( pwwca.x );
  m["y"] = QString::number( pwwca.y );
}

void FixedPointImp::readParams( const std::map<QCString, QString>& m, NormalPoint* p )
{
  bool ok;
  p->setCoord( Coordinate(
                 m.find("x")->second.toDouble(&ok),
                 pwwca.y = m.find("y")->second.toDouble(&ok) ) );
  Q_ASSERT( ok );
}

ConstrainedPointImp::ConstrainedPointImp( const Coordinate& d, Curve* c )
  : NormalPointImp(), mparam( c->getParam( d ) ), mcurve( c )
{
}

ConstrainedPointImp::ConstrainedPointImp()
  : NormalPointImp(), mparam( 0.5 ), mcurve( 0 )
{
}

void ConstrainedPointImp::calc( NormalPoint* p)
{
  p->setCoord( mcurve->getPoint( mparam ) );
}

void ConstrainedPointImp::startMove( const Coordinate&, NormalPoint* )
{
}

void ConstrainedPointImp::moveTo( const Coordinate& c, NormalPoint* p )
{
  mparam = mcurve->getParam( c );
  calc( p );
}

void ConstrainedPointImp::stopMove( NormalPoint* )
{
}

void ConstrainedPointImp::writeParams( std::map<QCString, QString>& m, NormalPoint* )
{
  m["param"] = QString::number( mparam );
}

void ConstrainedPointImp::readParams( const std::map<QCString, QString>& m, NormalPoint* p )
{
  bool ok;
  mparam = m.find("param")->second.toDouble(&ok);
  Q_ASSERT(ok);
  calc( p );
}

NormalPointImp* FixedPointImp::copy( NormalPoint* p )
{
  return new FixedPointImp( *this, p );
}

FixedPointImp::FixedPointImp( const FixedPointImp& p, NormalPoint* )
  : NormalPointImp(), pwwca( p.pwwca )
{
}

ConstrainedPointImp::ConstrainedPointImp( const ConstrainedPointImp& p, NormalPoint* d )
{
  mparam = p.mparam;
  mcurve = p.mcurve;
  mcurve->addChild( d );
}

void NormalPoint::calc()
{
  mimp->calc( this );
}

Objects NormalPoint::getParents() const
{
  return mimp->getParents();
}

Objects FixedPointImp::getParents()
{
  return Objects();
}

Objects ConstrainedPointImp::getParents()
{
  Objects tmp;
  tmp.push_back( mcurve );
  return tmp;
}

FixedPointImp* NormalPoint::fixedImp()
{
  return mimp->toFixed();
}

const FixedPointImp* NormalPoint::fixedImp() const
{
  return mimp->toFixed();
}

ConstrainedPointImp* NormalPoint::constrainedImp()
{
  return mimp->toConstrained();
}

const ConstrainedPointImp* NormalPoint::constrainedImp() const
{
  return mimp->toConstrained();
}
QString ConstrainedPointImp::sType()
{
  return QString::fromUtf8( "Constrained" );
}

NormalPointImp* ConstrainedPointImp::copy( NormalPoint* p )
{
  return new ConstrainedPointImp( *this, p );
}

KAction* NormalPoint::sConstructAction( KigDocument*, Type*, int )
{
  // TODO
  return 0;
}

QString FixedPointImp::wantArg(const Object*) const
{
  return 0;
}

bool FixedPointImp::selectArg( Object *, NormalPoint* )
{
  // not used...
  return false;
}

QString ConstrainedPointImp::wantArg( const Object* o ) const
{
  // not used...
  const Curve* c = o->toCurve();
  if ( ! c ) return 0;
  return QString::fromUtf8( "The curve" );
}

bool ConstrainedPointImp::selectArg( Object * o, NormalPoint* p )
{
  Curve* c = o->toCurve();
  if ( ! c ) return false;
  mcurve = c;
  calc( p );
  return true;
}

void NormalPoint::stopMove()
{
  mimp->stopMove( this );
}

QString NormalPoint::wantArg( const Object* o ) const
{
  return mimp->wantArg( o );
}

bool NormalPoint::selectArg( Object * o )
{
  return mimp->selectArg( o, this );
}

void NormalPoint::startMove( const Coordinate& c )
{
  mimp->startMove( c, this );
}

void NormalPoint::moveTo( const Coordinate& c )
{
  mimp->moveTo( c, this );
}
