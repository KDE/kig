// normalpoint.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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
#include "../kig/kig_view.h"

#include "../modes/constructing.h"
#include "../modes/moving.h"
#include "../modes/popup.h"

QString i18n( const char* );

NormalPoint::NormalPoint( const Objects& os )
  : mimp( 0 ), tparents( os )
{

}

NormalPoint::NormalPoint( const NormalPoint& p )
  : Point( p ), mimp( p.mimp->copy( this ) )
{
}

NormalPoint::~NormalPoint()
{
  mimp->unselectArgs( this );
  delete mimp;
}

void NormalPoint::redefine( const Coordinate& c,
                            const KigDocument& d,
                            const KigWidget& w )
{
  Objects o = d.whatAmIOn( c, w.screenInfo() );
  Curve* v = 0;
  // we don't want one of our children as a parent...
  Objects children = getAllChildren();
  for ( Objects::iterator i = o.begin(); i != o.end(); ++i )
  {
    if ( ( v = (*i)->toCurve() ) && ! children.contains( v ) )
      break;
    else v = 0;
  };
  if ( v )
  {
    // we need a ConstrainedPointImp...
    if ( mimp->toConstrained() )
    {
      // we already have a ConstrainedPointImp...
      // so we use it...
      mimp->toConstrained()->redefine( v, c, this );
      return;
    }
    // we already have a FixedImp...
    // so we delete it...
    mimp->unselectArgs( this );
    delete mimp;
    mimp = 0;
    tparents = Objects( v );
    // we build a new ConstrainedPointImp...
    setImp( new ConstrainedPointImp( c, v ) );
    return;
  };

  // else ( we returned before, so we leave out the else...

  // we need a FixedPointImp...
  // we already have something...
  if ( mimp->toFixed() )
  {
    // we already have a FixedPointImp..
    // so we use it...
    mimp->toFixed()->setCoord( c );
    return;
  };
  // we have a ConstrainedPointImp..
  // we don't need this, so we delete it
  mimp->unselectArgs( this );
  delete mimp;
  mimp = 0;
  // we build a new FixedPointImp...
  mimp = new FixedPointImp( c );
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

void FixedPointImp::startMove( const Coordinate& c, NormalPoint* p, const ScreenInfo& )
{
  pwwca = p->getCoord();
  pwwlmt = c;
}

void FixedPointImp::moveTo( const Coordinate& c, NormalPoint* )
{
  pwwca += c - pwwlmt;
  pwwlmt = c;
}

void FixedPointImp::stopMove( NormalPoint* )
{
}

Object::prop_map NormalPoint::getParams()
{
  prop_map m = Point::getParams();
  m["implementation-type"] = mimp->type();
  mimp->writeParams( m, this );
  return m;
}

void NormalPoint::setParams( const prop_map& m )
{
  Point::setParams( m );
  if ( mimp ) mimp->unselectArgs( this );
  delete mimp;
  prop_map::const_iterator p = m.find("implementation-type");
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
  mimp->setParents( tparents, this );
  tparents.clear();
  mimp->readParams( m, this );
};

void FixedPointImp::writeParams( Object::prop_map& m,
                                 NormalPoint* )
{
  m["x"] = QString::number( pwwca.x );
  m["y"] = QString::number( pwwca.y );
}

void FixedPointImp::readParams( const Object::prop_map& m,
                                NormalPoint* p )
{
  bool ok = true;
  pwwca = Coordinate(
      m.find("x")->second.toDouble(&ok),
      m.find("y")->second.toDouble(&ok) );
  Q_ASSERT( ok );
  p->setCoord( pwwca );
}

ConstrainedPointImp::ConstrainedPointImp( const Coordinate& d, Curve* c )
  : NormalPointImp(), mparam( c->getParam( d ) ), mcurve( c )
{
}

ConstrainedPointImp::ConstrainedPointImp()
  : NormalPointImp(), mparam( 0.5 ), mcurve( 0 )
{
}

void ConstrainedPointImp::calc( NormalPoint* p )
{
  p->setCoord( mcurve->getPoint( mparam ) );
}

void ConstrainedPointImp::startMove( const Coordinate&, NormalPoint*, const ScreenInfo& )
{
}

void ConstrainedPointImp::moveTo( const Coordinate& c, NormalPoint* )
{
  mparam = mcurve->getParam( c );
}

void ConstrainedPointImp::stopMove( NormalPoint* )
{
}

void ConstrainedPointImp::writeParams( Object::prop_map& m,
                                       NormalPoint* )
{
  m["param"] = QString::number( mparam );
}

void ConstrainedPointImp::readParams( const Object::prop_map& m,
                                      NormalPoint* )
{
  bool ok;
  mparam = m.find("param")->second.toDouble(&ok);
  Q_ASSERT(ok);
}

NormalPointImp* FixedPointImp::copy( NormalPoint* p )
{
  return new FixedPointImp( *this, p );
}

FixedPointImp::FixedPointImp( const FixedPointImp& p, NormalPoint* )
  : NormalPointImp(), pwwca( p.pwwca )
{
}

ConstrainedPointImp::ConstrainedPointImp( const ConstrainedPointImp& p,
                                          NormalPoint* np )
  : NormalPointImp( p )
{
  mparam = p.mparam;
  mcurve = p.mcurve;
  mcurve->addChild( np );
}

void NormalPoint::calc()
{
  mvalid = true;
  Objects p = getParents();
  for ( Objects::iterator i = p.begin(); i != p.end(); ++i )
    mvalid &= (*i)->valid();
  if ( mvalid ) mimp->calc( this );
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

KigMode* NormalPoint::sConstructMode( Type*, KigDocument* d, NormalMode* p )
{
  return new PointConstructionMode( p, d );
}

void FixedPointImp::setParents( const Objects& os, NormalPoint* )
{
  assert( os.empty() );
}

void ConstrainedPointImp::setParents( const Objects& os, NormalPoint* p )
{
  assert( os.size() == 1 );
  Curve* c = os.front()->toCurve();
  assert( c );
  mcurve = c;
  c->addChild( p );
}

void NormalPoint::stopMove()
{
  mimp->stopMove( this );
}

void NormalPoint::startMove( const Coordinate& c, const ScreenInfo& si )
{
  mimp->startMove( c, this, si );
}

void NormalPoint::moveTo( const Coordinate& c )
{
  mimp->moveTo( c, this );
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
    "A normal point, i.e. one that is either independent or attached "
    "to a line, circle, segment..."
    );
}

const QCString NormalPoint::vFullTypeName() const
{
  return sFullTypeName();
}

const QCString NormalPoint::sFullTypeName()
{
  return "NormalPoint";
}

const QString NormalPoint::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString NormalPoint::vDescription() const
{
  return sDescription();
}

const QCString NormalPoint::vIconFileName() const
{
  return sIconFileName();
}

const QCString NormalPoint::sIconFileName()
{
  return "point4";
}

const int NormalPoint::vShortCut() const
{
  return sShortCut();
}

const int NormalPoint::sShortCut()
{
  return 0;
}

FixedPointImp* NormalPointImp::toFixed()
{
  return 0;
}

ConstrainedPointImp* NormalPointImp::toConstrained()
{
  return 0;
}

const FixedPointImp* NormalPointImp::toFixed() const
{
  return 0;
}

const ConstrainedPointImp* NormalPointImp::toConstrained() const
{
  return 0;
}

NormalPoint* NormalPoint::toNormalPoint()
{
  return this;
}

FixedPointImp* FixedPointImp::toFixed()
{
  return this;
}

const FixedPointImp* FixedPointImp::toFixed() const
{
  return this;
}

QString FixedPointImp::type()
{
  return sType();
}

QString FixedPointImp::sType()
{
  return QString::fromUtf8( "Fixed" );
}

ConstrainedPointImp* ConstrainedPointImp::toConstrained()
{
  return this;
}

const ConstrainedPointImp* ConstrainedPointImp::toConstrained() const
{
  return this;
}

void ConstrainedPointImp::setP( const double p )
{
  mparam = p;
}

double ConstrainedPointImp::getP()
{
  return mparam;
}

QString ConstrainedPointImp::type()
{
  return sType();
}

NormalPoint* NormalPoint::fixedPoint( const Coordinate& c )
{
  NormalPoint* p = new NormalPoint( Objects() );
  p->setImp( new FixedPointImp( c ) );
  return p;
}

NormalPoint* NormalPoint::constrainedPoint( Curve* c, const Coordinate& d )
{
  NormalPoint* p = new NormalPoint( Objects( c ) );
  p->setImp( new ConstrainedPointImp( d, c ) );
  return p;
}

void ConstrainedPointImp::redefine( Curve* c, const Coordinate& p,
                                    NormalPoint* np )
{
  if ( c != mcurve )
    unselectArgs( np );
  mcurve = c;
  mparam = c->getParam( p );
}

void ConstrainedPointImp::unselectArgs( NormalPoint* np )
{
  mcurve->delChild( np );
  mcurve = 0;
}

void FixedPointImp::unselectArgs( NormalPoint* )
{
  // noop
}

void NormalPoint::setImp( NormalPointImp* i )
{
  mimp = i;
  i->setParents( tparents, this );
  tparents.clear();
}

NormalPoint* NormalPoint::sensiblePoint( const Coordinate& c,
                                         const KigDocument& d,
                                         const KigWidget& w )
{
  NormalPoint* p = fixedPoint( c );
  p->redefine( c, d, w );
  return p;
}

const NormalPoint* NormalPoint::toNormalPoint() const
{
  return this;
}

const char* NormalPoint::sActionName()
{
  return "objects_new_normalpoint";
}

void NormalPoint::addActions( NormalModePopupObjects& popup )
{
  Point::addActions( popup );
  popup.addNormalAction( NormalModePopupObjects::virtualActionsOffset + 10, i18n( "Redefine this point..." ) );
}

void NormalPoint::doNormalAction( int which, KigDocument* d, KigWidget* v, NormalMode* m, const Coordinate& )
{
  assert( which == NormalModePopupObjects::virtualActionsOffset + 10 );
  d->setMode( new NormalPointRedefineMode( this, d, v, m ) );
}
