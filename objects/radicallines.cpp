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

#include "conic.h"
#include "radicallines.h"
#include "../kig/kig_view.h"
#include "../modes/constructing.h"
#include "../modes/popup.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "../misc/common.h"

#include <kdebug.h>
#include <qpen.h>

#include <cmath>

Objects LineConicRadical::getParents() const
{
  Objects ret;
  ret.push_back( c1 );
  ret.push_back( c2 );
  return ret;
}

LineConicRadical::LineConicRadical(const LineConicRadical& l)
  : Line( l ), c1( l.c1 ), c2( l.c2 ), mwhich( l.mwhich ), mzeroindex( l.mzeroindex )
{
  assert( mwhich == -1 || mwhich == 1 );
  assert( mzeroindex > 0 && mzeroindex < 4 );
  assert( c1 && c2 );
  c1->addChild( this );
  c2->addChild( this );
}

const QString LineConicRadical::sDescriptiveName()
{
  return i18n("Radical Line for conics");
}

const QString LineConicRadical::sDescription()
{
  return i18n( "The lines constructed through the intersections of two conics.  This is also defined for non-intersecting conics..." );
}

const char* LineConicRadical::sActionName()
{
  return "objects_new_lineconicradical";
}

LineConicRadical* LineConicRadical::copy()
{
  return new LineConicRadical( *this );
}

LineConicRadical::LineConicRadical( const Objects& os )
{
  assert( os.size() == 2 );
  c1 = os[0]->toConic();
  c2 = os[1]->toConic();
  assert( c1 && c2 );
  c1->addChild( this );
  c2->addChild( this );
}

Object::prop_map LineConicRadical::getParams()
{
  prop_map map = Object::getParams();
  map["lineconicradical-branch"] = QString::number( mwhich );
  map["lineconicradical-zero"] = QString::number( mzeroindex );
  return map;
}

void LineConicRadical::setParams( const prop_map& map )
{
  Object::setParams( map );
  prop_map::const_iterator p = map.find("lineconicradical-branch");
  bool ok = true;
  if( p == map.end() ) mwhich = 1;   // fixme...
  else
    mwhich = p->second.toInt( &ok );
  assert( ok && ( mwhich == 1 || mwhich == -1 ) );

  p = map.find("lineconicradical-zero");
  if( p == map.end() ) mzeroindex = 1;   // fixme...
  else
  {
    mzeroindex = p->second.toInt( &ok );
    assert( ok && mzeroindex > 0 && mzeroindex < 4 );
  };
}

KigMode*
LineConicRadical::sConstructMode( MultiConstructibleType* ourtype,
                                             KigDocument* theDoc,
                                             NormalMode* previousMode )
{
  return new MultiConstructionMode( ourtype, previousMode, theDoc );
}

Objects LineConicRadical::sMultiBuild( const Objects& args )
{
  LineConicRadical* a;
  LineConicRadical* b;
  a = new LineConicRadical( args );
  b = new LineConicRadical( args );

  a->mwhich = 1;
  a->mzeroindex = 1;
  b->mwhich = -1;
  b->mzeroindex = 1;

  Objects o;
  o.push_back( a );
  o.push_back( b );
  return o;
}

void LineConicRadical::calc()
{
  assert( mwhich == -1 || mwhich == 1 );

  ConicCartesianEquationData cequation1=c1->cartesianEquationData();
  ConicCartesianEquationData cequation2=c2->cartesianEquationData();

  std::pair<Coordinate, Coordinate> coords = calcConicRadical( cequation1, cequation2, mwhich,
                                                   mzeroindex, mvalid );
  mpa = coords.first;
  mpb = coords.second;
};

std::pair<Coordinate, Coordinate> calcConicRadical( const ConicCartesianEquationData& cequation1,
                                                    const ConicCartesianEquationData& cequation2,
                                                    int which, int zeroindex, bool& valid )
{
  assert( which == 1 || which == -1 );
  assert( 0 < zeroindex && zeroindex < 4 );
  std::pair<Coordinate, Coordinate> ret;
  valid = true;

  double a = cequation1.coeffs[0];
  double b = cequation1.coeffs[1];
  double c = cequation1.coeffs[2];
  double d = cequation1.coeffs[3];
  double e = cequation1.coeffs[4];
  double f = cequation1.coeffs[5];

  double a2 = cequation2.coeffs[0];
  double b2 = cequation2.coeffs[1];
  double c2 = cequation2.coeffs[2];
  double d2 = cequation2.coeffs[3];
  double e2 = cequation2.coeffs[4];
  double f2 = cequation2.coeffs[5];

// background: the family of conics c + lambda*c2 has members that
// degenerate into a union of two lines. The values of lambda giving
// such degenerate conics is the solution of a third degree equation.
// The coefficients of such equation are given by:
// (Thanks to Dominique Devriese for the suggestion of this approach)
// domi: (And thanks to Maurizio for implementing it :)

  double df = 4*a*b*f - a*e*e - b*d*d - c*c*f + c*d*e;
  double cf = 4*a2*b*f + 4*a*b2*f + 4*a*b*f2
     - 2*a*e*e2 - 2*b*d*d2 - 2*f*c*c2
     - a2*e*e - b2*d*d - f2*c*c
     + c2*d*e + c*d2*e + c*d*e2;
  double bf = 4*a*b2*f2 + 4*a2*b*f2 + 4*a2*b2*f
     - 2*a2*e2*e - 2*b2*d2*d - 2*f2*c2*c
     - a*e2*e2 - b*d2*d2 - f*c2*c2
     + c*d2*e2 + c2*d*e2 + c2*d2*e;
  double af = 4*a2*b2*f2 - a2*e2*e2 - b2*d2*d2 - c2*c2*f2 + c2*d2*e2;

// assume both conics are nondegenerate, renormalize so that af = 1

  df /= af;
  cf /= af;
  bf /= af;
  af = 1.0;   // not needed, just for consistency

// computing the coefficients of the Sturm sequence

  double p1a = 2*bf*bf - 6*cf;
  double p1b = bf*cf - 9*df;
  double p0a = cf*p1a*p1a + p1b*(3*p1b - 2*bf*p1a);
  double fval, fpval, lambda;

  if (p0a < 0 && p1a < 0)
  {
    // -+--   ---+
    valid = false;
    return ret;
  }

  lambda = -bf/3.0;    //inflection point
  double displace = 1.0;
  if (p1a > 0)         // with two stationary points
  {
    displace += sqrt(p1a);  // should be enough.  The important
                            // thing is that it is larger than the
                            // semidistance between the stationary points
  }
  // compute the value at the inflection point using Horner scheme
  fval = bf + lambda;              // b + x
  fval = cf + lambda*fval;         // c + xb + xx
  fval = df + lambda*fval;         // d + xc + xxb + xxx

  if (fabs(p0a) < 1e-7)
  {   // this is the case if we intersect two vertical parabulas!
    p0a = 1e-7;  // fall back to the one zero case
  }
  if (p0a < 0)
  {
    // we have three roots..
    // we select the one we want ( defined by mzeroindex.. )
    lambda += ( 2 - zeroindex )* displace;
  }
  else
  {
    // we have just one root
    if( zeroindex > 1 )  // cannot find second and third root
    {
      valid = false;
      return ret;
    }

    if (fval > 0)      // zero on the left
    {
      lambda -= displace;
    } else {           // zero on the right
      lambda += displace;
    }

    // p0a = 0 means we have a root with multiplicity two
  }

//
// find a root of af*lambda^3 + bf*lambda^2 + cf*lambda + df = 0
// (use a Newton method starting from lambda = 0.  Hope...)
//

  double delta;

  int iterations = 0;
  const int maxiterations = 30;
  while (iterations++ < maxiterations)   // using Newton, iterations should be very few
  {
    // compute value of function and polinomial
    fval = fpval = af;
    fval = bf + lambda*fval;         // b + xa
    fpval = fval + lambda*fpval;     // b + 2xa
    fval = cf + lambda*fval;         // c + xb + xxa
    fpval = fval + lambda*fpval;     // c + 2xb + 3xxa
    fval = df + lambda*fval;         // d + xc + xxb + xxxa

    delta = fval/fpval;
    lambda -= delta;
    if (fabs(delta) < 1e-6) break;
  }
  if (iterations >= maxiterations) { valid = false; return ret; }

  // now we have the degenerate conic: a, b, c, d, e, f

  a += lambda*a2;
  b += lambda*b2;
  c += lambda*c2;
  d += lambda*d2;
  e += lambda*e2;
  f += lambda*f2;

  // domi:
  // this is the determinant of the matrix of the new conic.  It
  // should be zero, for the new conic to be degenerate.
  df = 4*a*b*f - a*e*e - b*d*d - c*c*f + c*d*e;

//lets work in homogeneous coordinates...

  double dis1 = e*e - 4*b*f;
  double maxval = fabs(dis1);
  int maxind = 1;
  double dis2 = d*d - 4*a*f;
  if (fabs(dis2) > maxval)
  {
    maxval = fabs(dis2);
    maxind = 2;
  }
  double dis3 = c*c - 4*a*b;
  if (fabs(dis3) > maxval)
  {
    maxval = fabs(dis3);
    maxind = 3;
  }
// one of these must be nonzero (otherwise the matrix is ...)
// exchange elements so that the largest is the determinant of the
// first 2x2 minor
  double temp;
  switch (maxind)
  {
    case 1:  // exchange 1 <-> 3
    temp = a; a = f; f = temp;
    temp = c; c = e; e = temp;
    temp = dis1; dis1 = dis3; dis3 = temp;
    break;

    case 2:  // exchange 2 <-> 3
    temp = b; b = f; f = temp;
    temp = c; c = d; d = temp;
    temp = dis2; dis2 = dis3; dis3 = temp;
    break;
  }


  // domi:
  // this is the negative of the determinant of the top left of the
  // matrix.  If it is 0, then the conic is a parabola, if it is < 0,
  // then the conic is an ellipse, if positive, the conic is a
  // hyperbola.  In this case, it should be positive, since we have a
  // degenerate conic, which is a degenerate case of a hyperbola..
//  double discrim = c*c - 4*a*b;

  if (dis3 >= 0)
  {
    double r[3];   // direction of the null space
    r[0] = 2*b*d - c*e;
    r[1] = 2*a*e - c*d;
    r[2] = dis3;

    double p[3];   // vector orthogonal to one of the two planes

    // there is still a stability problem here in the case of two
    // parabolas: in such case we get p[0] = p[1] = p[2] = 0
    //I must consider three cases
    if (fabs(a) >= fabs(b) && fabs(a) >= fabs(f))
    {
      p[0] = 2*a;
      p[1] = c + which*sqrt(dis3);
      p[2] = (-p[0]*r[0] - p[1]*r[1])/r[2];
    }
    else if (fabs(b) >= fabs(a) && fabs(b) >= fabs(f))
    {
      p[0] = c + which*sqrt(dis3);
      p[1] = 2*b;
      p[2] = (-p[0]*r[0] - p[1]*r[1])/r[2];
    }
    else
    {
      p[2] = 2*f;
      p[1] = d + which*sqrt(dis2);
      p[0] = (-p[1]*r[1] - p[2]*r[2])/r[0];
    }
// now remember the switch and we are almost done;
    switch (maxind)
    {
      case 1:  // exchange 1 <-> 3
      temp = r[0]; r[0] = r[2]; r[2] = temp;
      temp = p[0]; p[0] = p[2]; p[2] = temp;
      break;

      case 2:  // exchange 2 <-> 3
      temp = r[1]; r[1] = r[2]; r[2] = temp;
      temp = p[1]; p[1] = p[2]; p[2] = temp;
      break;

    }

    // "r" is the solution of the equation A*(x,y,z) = (0,0,0) where
    // A is the matrix of the degenerate conic.  This is what we
    // called in the conic theory we saw in high school a "double
    // point".  It has the unique property that any line going through
    // it is a "polar line" of the conic at hand.  It only exists for
    // degenerate conics.  It has another unique property that if you
    // take any other point on the conic, then the line between it and
    // the double point is part of the conic.
    // this is what we use here: we find the double point ( ret.first
    // ), and then find another points on the conic.

    // note: i think it would be better to dissolve the degenerate
    // conic into the two lines it consists of, and to return
    // calcConicLineIntersect with the first conic and those two
    // lines.. this would prevent problems when the double point is
    // infinite etc.

//    ret.first = (1/discrim)*Coordinate (2*b*d - c*e, 2*a*e - c*d);
//    ret.second = ret.first - Coordinate (-2*b, c + which*sqrt(discrim));
    ret.first = -p[2]/(p[0]*p[0] + p[1]*p[1]) * Coordinate (p[0],p[1]);
    ret.second = ret.first + Coordinate (-p[1],p[0]);
    valid = true;
  }
  else
  {
    // domi:
    // i would put an assertion here, but well, i guess it doesn't
    // really matter, and this prevents crashes if the math i still
    // recall from high school happens to be wrong :)
    valid = false;
  };

  return ret;
}

void LineConicRadical::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Conic* c;
  Conic* d;
  c = os[0]->toConic();
  d = os[1]->toConic();
  assert( c && d );
  ConicCartesianEquationData cda = c->cartesianEquationData();
  ConicCartesianEquationData cdb = d->cartesianEquationData();
  bool valid = true;
  std::pair<Coordinate, Coordinate> csa = calcConicRadical( cda, cdb, -1, 1, valid );
  std::pair<Coordinate, Coordinate> csb = calcConicRadical( cda, cdb, 1, 1, valid );
  if ( ! valid ) return;
  p.setPen( QPen (Qt::red,1) );
  p.drawLine( csa.first, csa.second );
  p.drawLine( csb.first, csb.second );
  return;
}

Object::WantArgsResult LineConicRadical::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  // we don't want the same conic twice...
  Conic* c = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! (*i)->toConic() ) return NotGood;
    else if( c == (*i)->toConic() ) return NotGood;
    else c = (*i)->toConic();
  };
  return size == 2 ? Complete : NotComplete;
}

QString LineConicRadical::sUseText( const Objects&, const Object* )
{
  return i18n("Radical Line of this conic");
}

ConicLineIntersectionPoint::ConicLineIntersectionPoint( const Objects& os )
    : Point(), mconic( 0 ), mline( 0 ), mwhich( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! mconic ) mconic = (*i)->toConic();
    if ( ! mline ) mline = (*i)->toAbstractLine();
  };
  assert( mconic && mline );
  mconic->addChild( this );
  mline->addChild( this );
}

ConicLineIntersectionPoint::ConicLineIntersectionPoint( const ConicLineIntersectionPoint& p )
  : Point( p ), mconic( p.mconic ), mline( p.mline ), mwhich( p.mwhich )
{
  assert( mconic && mline );
  mconic->addChild( this );
  mline->addChild( this );
}

ConicLineIntersectionPoint::~ConicLineIntersectionPoint()
{
}

ConicLineIntersectionPoint* ConicLineIntersectionPoint::copy()
{
  return new ConicLineIntersectionPoint( *this );
}

const QCString ConicLineIntersectionPoint::vFullTypeName() const
{
  return sFullTypeName();
}

const QCString ConicLineIntersectionPoint::sFullTypeName()
{
  return "ConicLineIntersectionPoint";
}

const QString ConicLineIntersectionPoint::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString ConicLineIntersectionPoint::sDescriptiveName()
{
  return i18n("The intersection of a line and a conic");
}

const QString ConicLineIntersectionPoint::vDescription() const
{
  return sDescription();
}

const QCString ConicLineIntersectionPoint::vIconFileName() const
{
  return sIconFileName();
}

const QCString ConicLineIntersectionPoint::sIconFileName()
{
  return 0;
}

const int ConicLineIntersectionPoint::vShortCut() const
{
  return sShortCut();
}

const int ConicLineIntersectionPoint::sShortCut()
{
  return 0;
}

const char* ConicLineIntersectionPoint::sActionName()
{
  return "objects_new_coniclineintersect";
}

void ConicLineIntersectionPoint::sDrawPrelim( KigPainter& p, const Objects& os )
{
  assert( os.size() <= 2 );
  if ( os.size() != 2 ) return;
  Conic* c = 0;
  AbstractLine* l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! c ) c = (*i)->toConic();
    if ( ! l ) l = (*i)->toAbstractLine();
  };
  assert( c && l );

  ConicCartesianEquationData ccd = c->cartesianEquationData();
  const LineData ld = l->lineData();

  bool valid = true;

  Coordinate cc = calcConicLineIntersect( ccd.coeffs, ld, -1, valid );
  Coordinate cd = calcConicLineIntersect( ccd.coeffs, ld, 1, valid );

  if ( valid )
  {
    sDrawPrelimPoint( p, cc );
    sDrawPrelimPoint( p, cd );
  };

  return;
}

Object::WantArgsResult ConicLineIntersectionPoint::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  bool gotc = false;
  bool gotl = false;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! gotc ) gotc = (*i)->toConic();
    if ( ! gotl ) gotl = (*i)->toAbstractLine();
  };
  if ( size == 1 ) return ( gotc || gotl ) ? NotComplete : NotGood;
  if ( size == 2 ) return ( gotc && gotl ) ? Complete : NotGood;
  assert( false );
}

QString ConicLineIntersectionPoint::sUseText( const Objects&, const Object* o )
{
  if ( o->toConic() ) return i18n( "Intersection of a line and this conic" );
  if ( o->toLine() ) return i18n( "Intersection of a conic and this line" );
  if ( o->toSegment() ) return i18n( "Intersection of a conic and this segment" );
  if ( o->toRay() ) return i18n( "Intersection of a conic and this ray" );
  assert( false );
}

Objects ConicLineIntersectionPoint::getParents() const
{
  Objects o;
  o.push_back( mconic );
  o.push_back( mline );
  return o;
}

void ConicLineIntersectionPoint::calc()
{
  ConicCartesianEquationData cequation(0,0,0,0,0,0);
  assert( mconic && mline );
  Coordinate t;
  cequation = mconic->cartesianEquationData();
  t = calcConicLineIntersect( cequation.coeffs,
                              mline->lineData(),
                              mwhich, mvalid );
  if ( mvalid ) mC = t;
}

Object::prop_map ConicLineIntersectionPoint::getParams()
{
  prop_map map = Point::getParams();
  map["coniclineintersect-side"] = QString::number( mwhich );
  return map;
}

void ConicLineIntersectionPoint::setParams( const prop_map& map )
{
  Point::setParams( map );
  prop_map::const_iterator p = map.find("coniclineintersect-side");
  if( p == map.end() ) mwhich = 1; // this is an error in the kig
                                   // file, but we ignore it..
  else
  {
    bool ok = true;
    mwhich = p->second.toInt( &ok );
    assert( ok && ( mwhich == 1 || mwhich == -1 ) );
  };
}

Objects ConicLineIntersectionPoint::sMultiBuild( const Objects& args )
{
  ConicLineIntersectionPoint* a;
  ConicLineIntersectionPoint* b;
  a = new ConicLineIntersectionPoint( args );
  b = new ConicLineIntersectionPoint( args );
  a->mwhich = 1;
  b->mwhich = -1;
  Objects o;
  o.push_back( a );
  o.push_back( b );
  return o;
}

KigMode*
ConicLineIntersectionPoint::sConstructMode( MultiConstructibleType* ourtype,
                                             KigDocument* theDoc,
                                             NormalMode* previousMode )
{
  return new MultiConstructionMode( ourtype, previousMode, theDoc );
}

const QString ConicLineIntersectionPoint::sDescription()
{
  return i18n("Construct the intersection of a line and a conic...");
}

void LineConicRadical::addActions( NormalModePopupObjects& p )
{
  p.addNormalAction( 58731, i18n( "Switch" ) );
}

void LineConicRadical::doNormalAction( int which, KigDocument* d, KigWidget* v, NormalMode* m, const Coordinate& cp )
{
  if ( which != 58731 ) Line::doNormalAction( which, d, v, m, cp );
  else
  {
    mzeroindex = mzeroindex % 3 + 1;
    calcForWidget( *v );
    v->redrawScreen();
  };
}
