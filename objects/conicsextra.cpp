/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#include "conicsextra.h"
#include "normalpoint.h"

#include "../misc/common.h"
#include "../misc/kigpainter.h"

#include <klocale.h>
#include <kdebug.h>
#include <qpen.h>
#include <math.h>

LinePolar::~LinePolar()
{
}

void LinePolar::calc()
{
  const ConicCartesianEquationData data=conic->cartesianEquationData();
  Coordinate cpole=pole->getCoord();

  assert ( conic && pole );
  std::pair<Coordinate, Coordinate> coords = linepolarcalc
             (data, cpole, mvalid);

  if (mvalid == false) return;

  mpa = coords.first;
  mpb = coords.second;
}

std::pair<Coordinate, Coordinate> linepolarcalc(
  const ConicCartesianEquationData& data, const Coordinate& cpole, bool& valid)
{
  std::pair<Coordinate, Coordinate> ret;
  double x = cpole.x;
  double y = cpole.y;
  double a = data.coeffs[0];
  double b = data.coeffs[1];
  double c = data.coeffs[2];
  double d = data.coeffs[3];
  double e = data.coeffs[4];
  double f = data.coeffs[5];

  double alpha = 2*a*x + c*y + d;
  double beta = c*x + 2*b*y + e;
  double gamma = d*x + e*y + 2*f;

  double normsq = alpha*alpha + beta*beta;

  if (normsq < 1e-10)          // line at infinity
  {
    valid = false;
    ret.first = Coordinate (0,0);
    ret.second = Coordinate (0,0);
    return ret;
  }
  valid = true;

  ret.first = -gamma/normsq * Coordinate (alpha, beta);
  ret.second = ret.first + Coordinate (-beta, alpha);
  return ret;
}

Objects LinePolar::getParents() const
{
  Objects objs;
  objs.push_back( conic );
  objs.push_back( pole );
  return objs;
}

LinePolar::LinePolar(const LinePolar& l)
  : Line( l ), conic( l.conic ), pole( l.pole )
{
  conic->addChild(this);
  pole->addChild(this);
}

const QString LinePolar::sDescriptiveName()
{
  return i18n("Polar line of a point");
}

QString LinePolar::sDescription()
{
  return i18n( "The polar line of a point with respect to a conic." );
}

const char* LinePolar::sActionName()
{
  return "objects_new_linepolar";
}

LinePolar::LinePolar( const Objects& os )
  : conic( 0 ), pole( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! conic ) conic = (*i)->toConic();
    if ( ! pole ) pole = (*i)->toPoint();
  }
  assert( conic && pole );
  conic->addChild( this );
  pole->addChild( this );
}

void LinePolar::sDrawPrelim( KigPainter& p, const Objects& os )
{
  assert( os.size() <= 2 );
  if ( os.size() != 2 ) return;
  Conic* c = 0;
  Point* pole = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! c ) c = (*i)->toConic();
    if ( ! pole ) pole = (*i)->toPoint();
  };
  assert( c && pole );

  const ConicCartesianEquationData data=c->cartesianEquationData();
  Coordinate cpole=pole->getCoord();
  bool valid = false;

  std::pair<Coordinate, Coordinate> coords = linepolarcalc (data, cpole, valid);

  if ( ! valid ) return;

  p.setPen (QPen (Qt::red, 1) );
  p.drawLine( coords.first, coords.second );
  return;
}

Object::WantArgsResult LinePolar::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  bool gotc = false;
  bool gotpole = false;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! gotc ) gotc = (*i)->toConic();
    if ( ! gotpole ) gotpole = (*i)->toPoint();
  }
  if ( size == 1 ) return (gotc || gotpole ) ? NotComplete : NotGood;
  if ( size == 2 ) return (gotc && gotpole ) ? Complete : NotGood;
  assert ( false );
}

QString LinePolar::sUseText( const Objects&, const Object* o )
{
  if ( o->toConic() ) return i18n( "Polar line with respect to this conic" );
  if ( o->toPoint() ) return i18n( "Polar line of this point" );
  else assert( false );
}

PointPolar::~PointPolar()
{
}

void PointPolar::calc()
{
  const ConicCartesianEquationData data=conic->cartesianEquationData();

  assert ( conic && polar );
  Coordinate coords = pointpolarcalc (data, polar->p1(), polar->p2(), mvalid);

  if ( mvalid ) mC = coords;
}

Coordinate pointpolarcalc ( const ConicCartesianEquationData& data, const Coordinate& p1,
                            const Coordinate& p2, bool& valid )
{
  double alpha = p2.y - p1.y;
  double beta = p1.x - p2.x;
  double gamma = p1.y*p2.x - p1.x*p2.y;

  double a11 = data.coeffs[0];
  double a22 = data.coeffs[1];
  double a12 = data.coeffs[2]/2.0;
  double a13 = data.coeffs[3]/2.0;
  double a23 = data.coeffs[4]/2.0;
  double a33 = data.coeffs[5];

//  double detA = a11*a22*a33 - a11*a23*a23 - a22*a13*a13 - a33*a12*a12 +
//                2*a12*a23*a13;

  double a11inv = a22*a33 - a23*a23;
  double a22inv = a11*a33 - a13*a13;
  double a33inv = a11*a22 - a12*a12;
  double a12inv = a23*a13 - a12*a33;
  double a23inv = a12*a13 - a23*a11;
  double a13inv = a12*a23 - a13*a22;

  double x = a11inv*alpha + a12inv*beta + a13inv*gamma;
  double y = a12inv*alpha + a22inv*beta + a23inv*gamma;
  double z = a13inv*alpha + a23inv*beta + a33inv*gamma;

  if (fabs(z) < 1e-10)          // point at infinity
  {
    valid = false;
    return Coordinate (0,0);
  }
  valid = true;

  x /= z;
  y /= z;
  return Coordinate (x, y);
}

Objects PointPolar::getParents() const
{
  Objects objs;
  objs.push_back( conic );
  objs.push_back( polar );
  return objs;
}

PointPolar::PointPolar(const PointPolar& p)
  : Point( p ), conic( p.conic ), polar( p.polar )
{
  conic->addChild(this);
  polar->addChild(this);
}

const QString PointPolar::sDescriptiveName()
{
  return i18n("Polar point of a line");
}

QString PointPolar::sDescription()
{
  return i18n( "The polar point of a line with respect to a conic." );
}

const char* PointPolar::sActionName()
{
  return "objects_new_pointpolar";
}

PointPolar::PointPolar( const Objects& os )
  : Point(), conic( 0 ), polar( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! conic ) conic = (*i)->toConic();
    if ( ! polar ) polar = (*i)->toAbstractLine();
  }
  assert( conic && polar );
  conic->addChild( this );
  polar->addChild( this );
}

void PointPolar::sDrawPrelim( KigPainter& p, const Objects& os )
{
  assert( os.size() <= 2 );
  if ( os.size() != 2 ) return;
  Conic* c = 0;
  AbstractLine* polar = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! c ) c = (*i)->toConic();
    if ( ! polar) polar = (*i)->toAbstractLine();
  };
  assert( c && polar );

  const ConicCartesianEquationData data=c->cartesianEquationData();
  bool valid = false;

  Coordinate coords = pointpolarcalc (data, polar->p1(), polar->p2(), valid);

  if ( valid ) sDrawPrelimPoint( p, coords );
  return;
}

Object::WantArgsResult PointPolar::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  bool gotc = false;
  bool gotpolar = false;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! gotc ) gotc = (*i)->toConic();
    if ( ! gotpolar ) gotpolar = (*i)->toAbstractLine();
  }
  if ( size == 1 ) return ( gotc || gotpolar ) ? NotComplete : NotGood;
  if ( size == 2 ) return ( gotc && gotpolar ) ? Complete : NotGood;
  assert( false );
}

QString PointPolar::sUseText( const Objects&, const Object* o )
{
  if ( o->toConic() ) return i18n( "Polar point with respect to this conic" );
  if ( o->toLine() ) return i18n( "Polar point of this line" );
  if ( o->toSegment() ) return i18n( "Polar point of this segment" );
  if ( o->toRay() ) return i18n( "Polar point of this ray" );
  else assert( false );
}

/*
 * equilateral hyperbola by 4 points
 */

void EquilateralHyperbolaB4P::calc()
{
  std::vector<Coordinate> points;

  mvalid = true;
  for ( Point** ipt = pts; ipt < pts + 4; ++ipt )
    mvalid &= (*ipt)->valid();
  if ( mvalid )
  {
    std::transform( pts, pts + 4, std::back_inserter( points ),
                    std::mem_fun( &Point::getCoord ) );
    cequation = calcCartesian( points, equilateral );
    pequation = calcPolar( cequation );
  }
}

const char* EquilateralHyperbolaB4P::sActionName()
{
  return "objects_new_equilateralhyperbolab4p";
}

Objects EquilateralHyperbolaB4P::getParents() const
{
  Objects objs ( pts, pts+4 );
  return objs;
}

EquilateralHyperbolaB4P::EquilateralHyperbolaB4P(const EquilateralHyperbolaB4P& c)
  : Conic( c ), cequation( c.cequation ), pequation( c.pequation )
{
  for ( int i = 0; i != 4; ++i )
  {
    pts[i]=c.pts[i];
    pts[i]->addChild(this);
  }
}

const QString EquilateralHyperbolaB4P::sDescriptiveName()
{
  return i18n( "Equilateral hyperbola by four points" );
}

const QString EquilateralHyperbolaB4P::sDescription()
{
  return i18n( "An equilateral hyperbola constructed through four points" );
}

Object::WantArgsResult EquilateralHyperbolaB4P::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 4 || size < 1 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toPoint() ) return NotGood;
  return size == 4 ? Complete : NotComplete;
}

QString EquilateralHyperbolaB4P::sUseText( const Objects&, const Object* )
{
  return i18n( "Through point" );
}

void EquilateralHyperbolaB4P::sDrawPrelim( KigPainter& p, const Objects& os )
{
  std::vector<Coordinate> points;

  uint size = os.size();
  assert( size > 0 && size < 5 );
  if ( size < 2 ) return;  // don't drawprelim if too few points
  for ( uint i = 0; i < size; ++i )
  {
    assert( os[i]->toPoint() );
    points.push_back( os[i]->toPoint()->getCoord() );
  };

  p.setPen(QPen (Qt::red, 1));
  p.drawConic( calcPolar( calcCartesian( points,
      equilateral, zerotilt, ysymmetry ) ) );
  return;
}

EquilateralHyperbolaB4P::EquilateralHyperbolaB4P( const Objects& os )
  : Conic()
{
  assert( os.size() == 4 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( (*i)->toPoint() );
    (*i)->addChild( this );
    pts[i-os.begin()] = static_cast<Point*>( *i );
  };
}

const ConicCartesianEquationData EquilateralHyperbolaB4P::cartesianEquationData() const
{
  return cequation;
}

const ConicPolarEquationData EquilateralHyperbolaB4P::polarEquationData() const
{
  return pequation;
}

/*
 * this function tests if a point lies on a line.
 * - it is a constrained point
 * - it is an intersection between two lines
 * - it is an intersection of a line and a circle
 * - it is an intersection of a line and a conic
 */
bool belongsPointLine ( Object* point, Object* line)
{
  if ( ! point->toPoint() ) return false;
  if ( ! line->toAbstractLine() ) return false;
  Objects parents = point->getParents();

  switch (parents.size())
  {
    case 1:
      if (parents[0]->toCurve() != line->toCurve()) return false;
      if ( ! point->toNormalPoint() ) return false;
      if ( ! point->toNormalPoint()->constrainedImp() ) return false;
      return true;

    case 2:
      if (parents[0]->toCurve() != line->toCurve() &&
          parents[1]->toCurve() != line->toCurve()) return false;
      else return true;

    default:
    return false;
  }
}

// /*
//  * conic by diameter and focus
//  */

// ConicBDF::ConicBDF( const Objects& os )
//   : Conic(), pequation()
// {
//   assert( os.size() == 2 );
//   assert( os[0]->toAbstractLine() );
//   diameter = os[0]->toAbstractLine();
//   assert( os[1]->toPoint() );
//   focus1 = os[1]->toPoint();
//   diameter->addChild( this );
//   focus1->addChild( this );
// }

// void ConicBDF::calc()
// {
//   mvalid = diameter->valid() && focus1->valid();

//   if ( mvalid )
//   {
//     Coordinate focus = focus1->getCoord();
//     pequation = calcConicBDF (diameter->p1(), diameter->p2(), &focus);
//   }
// }

// const ConicPolarEquationData calcConicBDF( const Coordinate& p1, const Coordinate& p2,
//                                             bool wantParabola, const Coordinate* focus )
// {
//   double eccentricity, a, b;
//   ConicPolarEquationData mequation;

//   mequation.focus1 = 0.3*p1 + 0.7*p2;
//   if ( focus )
//   {
//     mequation.focus1 = *focus;
//   }

//   if (fabs(p1.x - p2.x) > fabs(p1.y - p2.y))
//   {
//     a = mequation.focus1.x - p1.x;
//     b = p2.x - mequation.focus1.x;
//   } else
//   {
//     a = mequation.focus1.y - p1.y;
//     b = p2.y - mequation.focus1.y;
//   }
//   if ( wantParabola ) eccentricity = 1.0;
//     else eccentricity = (b - a)/(b + a);

//   Coordinate p2p1 = p2 - p1;
//   double p2p1l = p2p1.length();

//   a *= p2p1l/(a+b);
//   mequation.pdimen = a*(1+eccentricity);
//   mequation.ecostheta0 = eccentricity * p2p1.x / p2p1l;
//   mequation.esintheta0 = eccentricity * p2p1.y / p2p1l;

//   return mequation;
// }

// const ConicPolarEquationData ConicBDF::polarEquationData() const
// {
//   return pequation;
// }

// Objects ConicBDF::getParents() const
// {
//   Objects objs;
//   objs.push_back( diameter );
//   objs.push_back( focus1 );
//   return objs;
// }

// ConicBDF::ConicBDF(const ConicBDF& c)
//   : Conic( c ), diameter( c.diameter ), focus1( c.focus1 )
// {
//   diameter->addChild(this);
//   focus1->addChild(this);
// }

// const QString ConicBDF::sDescriptiveName()
// {
//   return i18n("Conic by diameter and focus");
// }

// QString ConicBDF::sDescription()
// {
//   return i18n(
//     "A conic constructed by its diameter (segment, ray or line) and a focus"
//     );
// }

// const char* ConicBDF::sActionName()
// {
//   return "objects_new_conicbdf";
// }

// Object::WantArgsResult ConicBDF::sWantArgs( const Objects& os )
// {
//   uint size = os.size();
//   assert( size > 0 );
//   if ( size > 2 ) return NotGood;
//   if ( ! os[0]->toAbstractLine() ) return NotGood;
//   if ( size == 1 ) return NotComplete;
//   if ( belongsPointLine ( os[1], os[0] ) )
//       return Complete;

//   return NotGood;
// }

// QString ConicBDF::sUseText( const Objects&, const Object* o )
// {
//   if ( o->toRay() ) return i18n( "Parabola with this ray as axis" );
//   if ( o->toSegment() ) return i18n( "Ellipse with this segment as diameter" );
//   if ( o->toLine() ) return i18n( "Ellipse or Hyperbola with this line as diameter" );
//   if ( o->toPoint() ) return i18n( "Conic with this focus" );
//   assert( false );
// }


// void ConicBDF::sDrawPrelim( KigPainter& p, const Objects& args )
// {
//   ConicPolarEquationData data;

//   if ( args.size() < 1 ) return;
//   assert( args.size() < 3 );

//   assert( args[0]->toAbstractLine() );
//   if ( args.size() == 1 )
//   {
//     AbstractLine* l = args[0]->toAbstractLine();
//     bool wantPar = l->toRay();
//     data = calcConicBDF ( l->p1(), l->p2(), wantPar );
//   }
//   else
//   {
//     Point* p = args[1]->toPoint();
//     AbstractLine* l = args[0]->toAbstractLine();
//     if ( p == 0 )
//     {
//       p = args[0]->toPoint();
//       l = args[1]->toAbstractLine();
//     };
//     assert( p && l );
//     Coordinate focus = p->getCoord();
//     bool wantPar = l->toRay();
//     data = calcConicBDF ( l->p1(), l->p2(), wantPar, &focus );
//   }

//   p.setPen(QPen (Qt::red, 1));
//   p.drawConic( data );
// }

// ConicBDF::~ConicBDF()
// {
// }
