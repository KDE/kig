// normalpoint.cc
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

#include "normalpoint.h"

#include "curve.h"

#include "../kig/kig_part.h"

#include <klocale.h>

NormalPoint* NormalPoint::copy()
{
  return new NormalPoint( *this );
}

NormalPoint::NormalPoint( NormalPointImp* i )
  : mimp( 0 )
{
  setImp( i );
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
                                            double fault,
                                            NormalPointImp* prev )
{
  // prev is a NormalPointImp that has already been
  // constructed... here we try to avoid constructing new Imp's
  // unless it's necessary...
  Objects o = d->whatAmIOn( c, fault );
  Curve* v = 0;
  for ( Objects::iterator i = o.begin(); i != o.end(); ++i )
    if ( ( v = (*i)->toCurve() ) )
      break;
  if ( v )
  {
    // we need a ConstrainedPointImp...
    if ( prev )
    {
      // we already have something...
      if ( prev->toConstrained() )
      {
        // we already have a ConstrainedPointImp...
        // so we use it...
        // UPDATE: or we don't cause this leads to trouble... :(
//         prev->toConstrained()->redefine( v, c );
//         return prev;
      }
      // we have a FixedPointImp...
      // we don't need this, but we don't delete it, cause this is
      // done in setImp();
    };
    // we build a new ConstrainedPointImp...
    return new ConstrainedPointImp( c, v );
  }
  // else ( we returned before, so i can leave out the else...

  // we need a FixedPointImp...
  if ( prev )
  {
    // we already have something...
    if ( prev->toFixed() )
    {
      // we already have a FixedPointImp..
      // so we use it...
      prev->toFixed()->setCoord( c );
      return prev;
    };
    // we have a ConstrainedPointImp..
    // we don't need this, but we don't delete it, cause this is
    // done in setImp();
  };
  // we build a new FixedPointImp...
  return new FixedPointImp( c );
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

ConstrainedPointImp::ConstrainedPointImp( const ConstrainedPointImp& p )
  : NormalPointImp( p )
{
  mparam = p.mparam;
  mcurve = p.mcurve;
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

NormalPointImp* ConstrainedPointImp::copy( NormalPoint* )
{
  return new ConstrainedPointImp( *this );
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
  c->addChild( p );
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

void NormalPoint::setImp( NormalPointImp* imp )
{
  if ( mimp != imp )
  {
    delete mimp;
  };
  mimp = imp;
  mimp->calc( this );
}

void FixedPointImp::setCoord( const Coordinate& c )
{
  pwwca = c;
}

const Coordinate FixedPointImp::getCoord()
{
  return pwwca;
}

const QString NormalPoint::sDescriptiveName()
{
  return i18n("Normal Point");
}

const QString NormalPoint::sDescription()
{
  return i18n(
    "A normal point, that is either independent or attached to a line, "
    "circle, segment..."
    );
}
