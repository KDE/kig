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

#include "../modes/constructing.h"

QString i18n( const char* );

NormalPoint* NormalPoint::copy()
{
  return new NormalPoint( *this );
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
  mimp->unselectArgs( this );
  delete mimp;
}

void NormalPoint::redefine( const Coordinate& c,
                            const KigDocument& d,
                            double fault )
{
  Objects o = d.whatAmIOn( c, fault );
  Curve* v = 0;
  for ( Objects::iterator i = o.begin(); i != o.end(); ++i )
    if ( ( v = (*i)->toCurve() ) )
      break;
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
    // we build a new ConstrainedPointImp...
    mimp = new ConstrainedPointImp( c, v, this );
    mimp->calc( this );
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
    mimp->calc( this );
    return;
  };
  // we have a ConstrainedPointImp..
  // we don't need this, so we delete it
  mimp->unselectArgs( this );
  delete mimp;
  mimp = 0;
  // we build a new FixedPointImp...
  mimp = new FixedPointImp( c );
  mimp->calc( this );
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
  m["implementation-type"] = mimp->type();
  mimp->writeParams( m, this );
  return m;
}

void NormalPoint::setParams( const std::map<QCString, QString>& m )
{
  Point::setParams( m );
  if ( mimp ) mimp->unselectArgs( this );
  delete mimp;
  std::map<QCString,QString>::const_iterator p = m.find("implementation-type");
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

void FixedPointImp::writeParams( std::map<QCString, QString>& m,
                                 NormalPoint* )
{
  m["x"] = QString::number( pwwca.x );
  m["y"] = QString::number( pwwca.y );
}

void FixedPointImp::readParams( const std::map<QCString, QString>& m,
                                NormalPoint* p )
{
  bool ok = true;
  pwwca = Coordinate(
      m.find("x")->second.toDouble(&ok),
      pwwca.y = m.find("y")->second.toDouble(&ok) );
  Q_ASSERT( ok );
  p->setCoord( pwwca );
}

ConstrainedPointImp::ConstrainedPointImp( const Coordinate& d, Curve* c,
                                          NormalPoint* np )
  : NormalPointImp(), mparam( c->getParam( d ) ), mcurve( c )
{
  mcurve->addChild( np );
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

void ConstrainedPointImp::writeParams( std::map<QCString, QString>& m,
                                       NormalPoint* )
{
  m["param"] = QString::number( mparam );
}

void ConstrainedPointImp::readParams( const std::map<QCString, QString>& m,
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

KigMode* NormalPoint::sConstructMode( Type*, KigDocument* d, NormalMode* p )
{
  return new PointConstructionMode( p, d );
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
  if ( !c ) return 0;
  return QString::fromUtf8( "The curve" );
}

bool ConstrainedPointImp::selectArg( Object* o, NormalPoint* p )
{
  Curve* c = o->toCurve();
  assert( c );
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

void NormalPoint::drawPrelim( KigPainter &, const Object* ) const
{
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
  NormalPoint* p = new NormalPoint();
  p->setImp( new FixedPointImp( c ) );
  return p;
}

NormalPoint* NormalPoint::constrainedPoint( Curve* c, const Coordinate& d )
{
  NormalPoint* p = new NormalPoint();
  p->setImp( new ConstrainedPointImp( d, c, p ) );
  return p;
}

void ConstrainedPointImp::redefine( Curve* c, const Coordinate& p,
                                    NormalPoint* np )
{
  if ( c != mcurve )
    unselectArgs( np );
  mcurve = c;
  mparam = c->getParam( p );
  calc( np );
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
  i->calc( this );
}

NormalPoint* NormalPoint::sensiblePoint( const Coordinate& c,
                                         const KigDocument& d,
                                         double fault )
{
  NormalPoint* p = fixedPoint( c );
  p->redefine( c, d, fault );
  return p;
}

const NormalPoint* NormalPoint::toNormalPoint() const
{
  return this;
}
