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

#include "circle.h"

#include "point.h"

#include "../misc/common.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "../misc/coordinate_system.h"
#include "../kig/kig_view.h"
#include "../kig/kig_part.h"

#include <math.h>

Circle::Circle()
{
};

Circle::~Circle()
{
};

bool Circle::contains (const Coordinate& o, const ScreenInfo& si ) const
{
  return fabs((qpc - Coordinate(o)).length() - mradius) <= si.normalMiss();
};

void Circle::draw (KigPainter& p, bool ss) const
{
  p.setPen(QPen ( selected && ss ? Qt::red : mColor, 1));
  p.setBrush( Qt::NoBrush );
  p.drawCircle( qpc, mradius);
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
  return qpc + Coordinate (cos(p * 2 * M_PI), sin(p * 2 * M_PI)) * mradius;
};

double Circle::getParam (const Coordinate& p) const
{
  Coordinate tmp = p - qpc;
  return fmod(atan2(tmp.y, tmp.x) / ( 2 * M_PI ) + 1, 1);
};

CircleBCP::CircleBCP( const Objects& os )
  : poc (0), centre(0)
{
  assert( os.size() == 2 );
  centre = os[0]->toPoint();
  poc = os[1]->toPoint();
  assert( poc );
  assert( centre );
  poc->addChild( this );
  centre->addChild( this );
};

void CircleBCP::calc()
{
  if (poc->valid() && centre->valid())
  {
    mvalid = true;
    mradius = calcRadius(centre,poc);
    qpc = centre->getCoord();
  }
  else
    mvalid = false;
};

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
  Objects objs ( pts, pts+3 );
  return objs;
}

Objects CircleBCP::getParents() const
{
  Objects objs;
  objs.push_back( centre );
  objs.push_back( poc );
  return objs;
}

void CircleBTP::calc()
{
  mvalid = true;
  for ( Point** i = pts; i < pts + 3; ++i )
    mvalid &= (*i)->valid();
  if ( mvalid )
  {
    Coordinate a = pts[0]->getCoord();
    Coordinate b = pts[1]->getCoord();
    Coordinate c = pts[2]->getCoord();
    // the center coords:
    qpc = calcCenter( a, b, c );
    mradius = calcRadius( qpc, c );
  }
};

CircleBCP::CircleBCP(const CircleBCP& c)
  : Circle( c ), poc( c.poc ), centre( c.centre )
{
  poc->addChild(this);
  centre->addChild(this);
}

CircleBTP::CircleBTP(const CircleBTP& c)
  : Circle( c )
{
  for ( int i = 0; i != 3; ++i )
  {
    pts[i]=c.pts[i];
    pts[i]->addChild(this);
  }
}

double Circle::calcRadius( const Point* c, const Point* p )
{
  return calcRadius( c->getCoord(), p->getCoord() );
}

double Circle::calcRadius( const Coordinate& c, const Coordinate& p )
{
  return ( c - p ).length();
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

const char* CircleBCP::sActionName()
{
  return "objects_new_circlebcp";
}

const char* CircleBTP::sActionName()
{
  return "objects_new_circlebtp";
}

Circle::Circle( const Circle& c )
  : Conic( c ), qpc( c.qpc ), mradius( c.mradius )
{
}

Object::WantArgsResult CircleBCP::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 2 ) return NotGood;
  if ( size < 1 ) return NotGood;
  if ( !os[0]->toPoint() ) return NotGood;
  if ( size == 2 ) return os[1]->toPoint() ? Complete : NotGood;
  else return NotComplete;
}

QString CircleBCP::sUseText( const Objects& os, const Object* )
{
  switch ( os.size() )
  {
  case 0:
    return i18n( "Circle with this center" );
  case 1:
    return i18n( "Circle through this point" );
  default:
    return 0;
  };
}

void CircleBCP::sDrawPrelim( KigPainter& p, const Objects& args )
{
  if ( args.size() != 2 ) return;
  if ( ! ( args[0]->toPoint() && args[1]->toPoint() ) ) return;
  Coordinate c = args[0]->toPoint()->getCoord();
  Coordinate d = args[1]->toPoint()->getCoord();
  p.setPen(QPen (Qt::red, 1));
  p.drawCircle( c, calcRadius( c, d ) );
}

Object::WantArgsResult CircleBTP::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 3 || size < 1 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toPoint() ) return NotGood;
  return size == 3 ? Complete : NotComplete;
}

QString CircleBTP::sUseText( const Objects&, const Object* )
{
  return i18n("Through point");
}

void CircleBTP::sDrawPrelim( KigPainter& p, const Objects& os )
{
  uint size = os.size();
  if ( size != 2 && size != 3 ) return;
  assert( os[0]->toPoint() );
  assert( os[1]->toPoint() );
  Coordinate a = os[0]->toPoint()->getCoord();
  Coordinate b = os[1]->toPoint()->getCoord();
  Coordinate c;
  if ( size == 3 )
  {
    assert( os[2]->toPoint() );
    c = os[2]->toPoint()->getCoord();
  }
  else
  {
    // we pick the third point so that the three points form a
    // triangle with equal sides...

    // midpoint:
    Coordinate m = ( b + a ) / 2;
    if ( b.y != a.y )
    {
      // direction of the perpend:
      double d = -(b.x-a.x)/(b.y-a.y);

      // length:
      // sqrt( 3 ) == tan( 60° ) == sqrt( 2^2 - 1^2 )
      double l = sqrt(3) * (a-b).length() / 2;

      double d2 = sqr( d );
      double l2 = sqr( l );
      double dx = sqrt( l2 / ( d2 + 1 ) );
      double dy = sqrt( l2 * d2 / ( d2 + 1 ) );
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

CircleBTP::CircleBTP( const Objects& os )
{
  assert( os.size() == 3 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( (*i)->toPoint() );
    (*i)->addChild( this );
    pts[i-os.begin()] = static_cast<Point*>( *i );
  };
}

double Circle::squareRadius() const
{
  return sqr( mradius );
}

double Circle::radius() const
{
  return mradius;
}

Circle* Circle::toCircle()
{
  return this;
}

const Circle* Circle::toCircle() const
{
  return this;
}

const ConicCartesianEquationData Circle::cartesianEquationData() const
{
  Coordinate c = center();
  double sqr = squareRadius();
  ConicCartesianEquationData data(1.0, 1.0, 0.0, -2*c.x, -2*c.y,
    c.x*c.x + c.y*c.y - sqr);
  return data;
}

const ConicPolarEquationData Circle::polarEquationData() const
{
  return ConicPolarEquationData( center(), radius(), 0, 0 );
}

QString Circle::cartesianEquationString( const KigWidget& ) const
{
  QString ret = i18n( "x^2 + y^2 + %1 x + %2 y + %3 = 0" );
  ConicCartesianEquationData data = cartesianEquationData();
  ret = ret.arg( data.coeffs[3], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[4], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[5], 0, 'g', 3 );
  return ret;
}

QString Circle::polarEquationString( const KigWidget& w ) const
{
  QString ret = i18n( "rho = %1   [centered at %2]" );
  ConicPolarEquationData data = polarEquationData();
  ret = ret.arg( data.pdimen, 0, 'g', 3 );
  ret = ret.arg( w.document().coordinateSystem().fromScreen( data.focus1, w ) );
  return ret;
}

const uint Circle::numberOfProperties() const
{
  return Curve::numberOfProperties() + 6;
}

const Property Circle::property( uint which, const KigWidget& w ) const
{
  assert( which < Circle::numberOfProperties() );
  if ( which < Curve::numberOfProperties() ) return Curve::property( which, w );
  if ( which == Curve::numberOfProperties() )
    return Property( surface() );
  else if ( which == Curve::numberOfProperties() + 1 )
    return Property( circumference() );
  else if ( which == Curve::numberOfProperties() + 2 )
    return Property( radius() );
  else if ( which == Curve::numberOfProperties() + 3 )
    return Property( center() );
  else if ( which == Curve::numberOfProperties() + 4 )
    return Property( cartesianEquationString( w ) );
  else if ( which == Curve::numberOfProperties() + 5 )
    return Property( polarEquationString( w ) );
  else assert( false );
}

const QCStringList Circle::properties() const
{
  QCStringList l = Curve::properties();
  l << I18N_NOOP( "Surface" );
  l << I18N_NOOP( "Circumference" );
  l << I18N_NOOP( "Radius" );
  l << I18N_NOOP( "Center" );
  l << I18N_NOOP( "Cartesian equation" );
  l << I18N_NOOP( "Polar equation" );
  assert( l.size() == Circle::numberOfProperties() );
  return l;
}

double Circle::surface() const
{
  return M_PI * squareRadius();
}

double Circle::circumference() const
{
  return 2 * M_PI * radius();
}

Coordinate Circle::center() const
{
  return qpc;
}

int Circle::conicType() const
{
  return 1;
}

Coordinate Circle::focus1() const
{
  return center();
}

Coordinate Circle::focus2() const
{
  return focus1();
}
