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

#include "point.h"

#include "../misc/common.h"
#include "../misc/kigpainter.h"

#include <klocale.h>
#include <kdebug.h>
#include <qpen.h>
#include <math.h>

Conic::Conic()
{
};

Conic::~Conic()
{
};

bool Conic::contains (const Coordinate& o, const ScreenInfo& si ) const
{
  Coordinate pos;
  double costheta, sintheta, len, ecosthetamtheta0, rho;

  Coordinate focus1 = mequation.focus1;
  double ecostheta0 = mequation.ecostheta0;
  double esintheta0 = mequation.esintheta0;
  double pdimen = mequation.pdimen;

  pos = Coordinate(o) - focus1;
  len = pos.length();
  costheta = pos.x / len;
  sintheta = pos.y / len;

  ecosthetamtheta0 = costheta*ecostheta0 + sintheta*esintheta0;
  rho = pdimen / (1.0 - ecosthetamtheta0);

  return fabs(len - rho) <= si.normalMiss();
};

void Conic::draw (KigPainter& p, bool ss) const
{
  p.setPen(QPen ( selected && ss ? Qt::red : mColor, 1));
  p.setBrush( Qt::NoBrush );

  p.drawConic( mequation );
};

bool Conic::inRect (const Rect& /*r*/) const
{
  // not implemented yet, i'm thinking: take the diagonals of the
  // rect, their intersections with the circle, and check their
  // positions...
  return false;
};

Coordinate Conic::getPoint (double p) const
{
  double rho, costheta, sintheta;

  Coordinate focus1 = mequation.focus1;
  double ecostheta0 = mequation.ecostheta0;
  double esintheta0 = mequation.esintheta0;
  double pdimen = mequation.pdimen;

  costheta = cos(p * 2 * M_PI);
  sintheta = sin(p * 2 * M_PI);
  rho = pdimen / (1 - costheta*ecostheta0 - sintheta*esintheta0);
  return focus1 + Coordinate (costheta, sintheta) * rho;
};

double Conic::getParam (const Coordinate& p) const
{
  Coordinate focus1 = mequation.focus1;
  Coordinate tmp = p - focus1;
  return fmod(atan2(tmp.y, tmp.x) / ( 2 * M_PI ) + 1, 1);
};

EllipseBFFP::EllipseBFFP( const Objects& os )
  : poc (0), focus1(0), focus2(0)
{
  assert( os.size() == 3 );
  focus1 = os[0]->toPoint();
  focus2 = os[1]->toPoint();
  poc = os[2]->toPoint();
  assert( poc );
  assert( focus1 );
  assert( focus2 );
  poc->addChild( this );
  focus1->addChild( this );
  focus2->addChild( this );
};

void EllipseBFFP::calc()
{
  mvalid = poc->valid() && focus1->valid() && focus2->valid();

  if ( mvalid )
  {
    Coordinate f2f1 = focus2->getCoord() - focus1->getCoord();
    double f2f1l = f2f1.length();
    mequation.ecostheta0 = f2f1.x / f2f1l;
    mequation.esintheta0 = f2f1.y / f2f1l;

    double d1 = (poc->getCoord() - focus1->getCoord()).length();
    double d2 = (poc->getCoord() - focus2->getCoord()).length();
    double rhomin = (d1 + d2 - f2f1l) /2.0;
    double rhomax = (d1 + d2 + f2f1l) /2.0;
    double eccentricity = (rhomax - rhomin) / (rhomax + rhomin);
    mequation.ecostheta0 *= eccentricity;
    mequation.esintheta0 *= eccentricity;
    mequation.pdimen = (1 - eccentricity)*rhomax;

    mequation.focus1 = focus1->getCoord();
  }
};

Objects EllipseBFFP::getParents() const
{
  Objects objs;
  objs.push_back( focus1 );
  objs.push_back( focus2 );
  objs.push_back( poc );
  return objs;
}

EllipseBFFP::EllipseBFFP(const EllipseBFFP& c)
  : Conic( c ), poc( c.poc ), focus1( c.focus1 ), focus2 ( c.focus2 )
{
  poc->addChild(this);
  focus1->addChild(this);
  focus2->addChild(this);
}

const QCString Conic::sBaseTypeName()
{
  return I18N_NOOP("conic");
}

const QString EllipseBFFP::sDescriptiveName()
{
  return i18n("Ellipse by focuses and point");
}

const QString EllipseBFFP::sDescription()
{
  return i18n(
    "An ellipse constructed by its focuses and a point on its border"
    );
}

const char* EllipseBFFP::sActionName()
{
  return "objects_new_ellipsebffp";
}

Conic::Conic( const Conic& c )
  : Curve( c ), mequation( c.mequation )
{
}

Object::WantArgsResult EllipseBFFP::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 3 ) return NotGood;
  if ( size < 1 ) return NotGood;
  if ( !os[0]->toPoint() ) return NotGood;
  if ( size < 2 ) return NotComplete;
  if ( !os[1]->toPoint() ) return NotGood;
  if ( size == 3 ) return os[2]->toPoint() ? Complete : NotGood;
  else return NotComplete;
}

QString EllipseBFFP::sUseText( const Objects& os, const Object* )
{
  switch ( os.size() )
  {
  case 0:
    return i18n( "Ellipse with this focus" );
  case 1:
    return i18n( "Ellipse with this focus" );
  case 2:
    return i18n( "Ellipse through this point" );
  default:
    return 0;
  };
}

void EllipseBFFP::sDrawPrelim( KigPainter& p, const Objects& args )
{
  ConicPolarEquationData data;
  p.setPen(QPen (Qt::red, 1));

  if ( args.size() != 3 ) return;
  if ( ! ( args[0]->toPoint() && args[1]->toPoint() && args[2]->toPoint() ) ) return;

  Coordinate f1 = args[0]->toPoint()->getCoord();
  Coordinate f2 = args[1]->toPoint()->getCoord();
  Coordinate d = args[2]->toPoint()->getCoord();

  Coordinate f2f1 = f2 - f1;
  double f2f1l = f2f1.length();
  data.ecostheta0 = f2f1.x / f2f1l;
  data.esintheta0 = f2f1.y / f2f1l;

  double d1 = (d - f1).length();
  double d2 = (d - f2).length();
  double rhomin = (d1 + d2 - f2f1l) /2.0;
  double rhomax = (d1 + d2 + f2f1l) /2.0;
  double eccentricity = (rhomax - rhomin) / (rhomax + rhomin);
  data.ecostheta0 *= eccentricity;
  data.esintheta0 *= eccentricity;
  data.pdimen = (1 - eccentricity)*rhomax;
  data.focus1 = f1;

  p.drawConic( data );
}

Conic* Conic::toConic()
{
  return this;
}

const Conic* Conic::toConic() const
{
  return this;
}

void EllipseBFFP::startMove(const Coordinate&, const ScreenInfo&)
{
  // moving is disabled..
}

void EllipseBFFP::moveTo(const Coordinate&)
{
  // moving is disabled..
}

void EllipseBFFP::stopMove()
{
  // moving is disabled..
};

const ConicCartesianEquationData calcCartesian ( const Coordinate points[5] )
{
  // points is a vector of exactly 5 points through which the conic is
  // constrained.
  // this routine should compute the coefficients in the cartesian equation
  //    a x^2 + b y^2 + c xy + d x + e y + f = 0
  // they are defined up to a multiplicative factor.
  // since we don't know (a priori) which one of them is nonzero, we
  // simply keep all 6 parameters, obtaining a 5x6 linear system which
  // we solve using gaussian elimination with complete pivoting
  double solution[6];

  // 5 rows, 6 columns..
  double matrix[5][6];
  int scambio[5];

  // fill in the matrix elements
  for ( int i = 0; i < 5; ++i )
  {
    double xi = points[i].x;
    double yi = points[i].y;
    matrix[i][0] = xi*xi;
    matrix[i][1] = yi*yi;
    matrix[i][2] = xi*yi;
    matrix[i][3] = xi;
    matrix[i][4] = yi;
    matrix[i][5] = 1.0;
  }

  // start gaussian elimination
  for ( int k = 0; k < 5; ++k )
  {
    // ricerca elemento di modulo massimo
    double maxval = -1.0;
    int imax = -1;
    int jmax = -1;
    for( int i = k; i < 5; ++i )
    {
      for( int j = k; j < 6; ++j )
      {
        if (fabs(matrix[i][j]) > maxval)
        {
          maxval = fabs(matrix[i][j]);
          imax = i;
          jmax = j;
        }
      }
    }
    // scambio di riga
    for( int j = k; j < 5 + 1; ++j )
    {
      double t = matrix[k][j];
      matrix[k][j] = matrix[imax][j];
      matrix[imax][j] = t;
    }
    // scambio di colonna
    for( int i = 0; i < 5; ++i )
    {
      double t = matrix[i][k];
      matrix[i][k] = matrix[i][jmax];
      matrix[i][jmax] = t;
    }
    // ricorda lo scambio effettuato al passo k
    scambio[k] = jmax;

    // ciclo sulle righe
    for( int i = k+1; i < 5; ++i)
    {
      double mik = matrix[i][k]/matrix[k][k];
      matrix[i][k] = mik;    //ricorda il moltiplicatore... (not necessary)
      // ciclo sulle colonne
      for( int j = k+1; j < 6; ++j )
      {
        matrix[i][j] -= mik*matrix[k][j];
      }
    }
  }

  // fine della fase di eliminazione
  // il sistema e' sottodeterminato, fisso l'ultima incognita = 1
  solution[5] = 1.0;
  for( int k = 4; k >= 0; --k )
  {
    // sostituzioni all'indietro
    solution[k] = 0.0;
    for ( int j = k+1; j < 6; ++j)
    {
      solution[k] -= matrix[k][j]*solution[j];
    }
    solution[k] /= matrix[k][k];
  }

  // ultima fase: riordinamento incognite

  for( int k = 4; k >= 0; --k )
  {
    int jmax = scambio[k];
    double t = solution[k];
    solution[k] = solution[jmax];
    solution[jmax] = t;
  }

  // now solution should contain the correct coefficients..
  return ConicCartesianEquationData( solution );
}

const ConicPolarEquationData calcPolar ( const ConicCartesianEquationData& cartdata )
{
  ConicPolarEquationData ret;

  double theta, sintheta, costheta;
  double aa, bb, cc, dd, ee, ff;
  double xf, yf;
  double eccentricity, sqrtdiscrim;

  double a = cartdata.coeffs[0];
  double b = cartdata.coeffs[1];
  double c = cartdata.coeffs[2];
  double d = cartdata.coeffs[3];
  double e = cartdata.coeffs[4];
  double f = cartdata.coeffs[5];

  // 1. Compute theta (tilt of conic)
  theta = atan2(c, b - a)/2;

  // now I should possibly add pi/2...
  costheta = cos(theta);
  sintheta = sin(theta);
  // compute new coefficients (c should now be zero)
  aa = a*costheta*costheta + b*sintheta*sintheta - c*sintheta*costheta;
  bb = a*sintheta*sintheta + b*costheta*costheta + c*sintheta*costheta;
  if (aa*bb < 0)
  {   // we have a hyperbola we need to check the correct orientation
    ee = d*sintheta + e*costheta;
    yf = - ee/(2*bb);
    ff = f + yf*yf + e*yf;
    if (ff*aa > 0)    // wrong orientation
    {
      if (theta > 0)
      {theta -= M_PI/2;} else {theta += M_PI/2;}
      costheta = cos(theta);
      sintheta = sin(theta);
      aa = a*costheta*costheta + b*sintheta*sintheta - c*sintheta*costheta;
      bb = a*sintheta*sintheta + b*costheta*costheta + c*sintheta*costheta;
    }
  } else
  {
    if (fabs (bb) < fabs (aa) )
    {
      if (theta > 0)
      {theta -= M_PI/2;} else {theta += M_PI/2;}
      costheta = cos(theta);
      sintheta = sin(theta);
      aa = a*costheta*costheta + b*sintheta*sintheta - c*sintheta*costheta;
      bb = a*sintheta*sintheta + b*costheta*costheta + c*sintheta*costheta;
    }
  }
  cc = 2*(a - b)*sintheta*costheta + c*(costheta*costheta - sintheta*sintheta);

  //  cc should be zero!!!   cout << "cc = " << cc << "\n";

  a = aa;
  b = bb;
  c = cc;

  dd = d*costheta - e*sintheta;
  ee = d*sintheta + e*costheta;

  d = dd;
  e = ee;

  // now b cannot be zero (otherwise conic is degenerate)
  a /= b;
  c /= b;
  d /= b;
  e /= b;
  f /= b;
  b = 1.0;

  // 2. compute y coordinate of focuses

  yf = - e/2;

  // new values:
  f += yf*yf + e*yf;
  e += 2*yf;   // this should be zero!

  // now: a > 0 -> ellipse
  //      a = 0 -> parabula
  //      a < 0 -> hyperbola

  eccentricity = sqrt(1.0 - a);

  sqrtdiscrim = sqrt(d*d - 4*a*f);
  if (d < 0.0) sqrtdiscrim = -sqrtdiscrim;
  xf = (4*a*f - 4*f - d*d)/(d + eccentricity*sqrtdiscrim);
  xf /= 2;

  // 3. the focus needs to be rotated back into position
  ret.focus1.x = xf*costheta + yf*sintheta;
  ret.focus1.y = -xf*sintheta + yf*costheta;

  // 4. final touch: the pdimen
  ret.pdimen = -sqrtdiscrim/2;

  ret.ecostheta0 = eccentricity*costheta;
  ret.esintheta0 = -eccentricity*sintheta;
  if ( ret.pdimen < 0)
  {
    ret.pdimen = -ret.pdimen;
    ret.ecostheta0 = -ret.ecostheta0;
    ret.esintheta0 = -ret.esintheta0;
  }

  return ret;
}

void ConicB5P::calc()
{
  Coordinate points[5];
  int i;

  mvalid = true;
  for ( Point** ipt = pts; ipt < pts + 5; ++ipt )
    mvalid &= (*ipt)->valid();
  if ( mvalid )
  {

    for (i = 0; i < 5; i++)
    {
      points[i] = pts[i]->getCoord();
    }
    mequation = calcPolar( calcCartesian ( points ) );
  }
}

const char* ConicB5P::sActionName()
{
  return "objects_new_conicb5p";
}

Objects ConicB5P::getParents() const
{
  Objects objs ( pts, pts+5 );
  return objs;
}

ConicB5P::ConicB5P(const ConicB5P& c)
  : Conic( c )
{
  for ( int i = 0; i != 5; ++i )
  {
    pts[i]=c.pts[i];
    pts[i]->addChild(this);
  }
}

const QString ConicB5P::sDescriptiveName()
{
  return i18n("Conic by five points");
}

const QString ConicB5P::sDescription()
{
  return i18n( "A conic constructed through five points" );
}

Object::WantArgsResult ConicB5P::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size > 5 || size < 1 ) return NotGood;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( ! (*i)->toPoint() ) return NotGood;
  return size == 5 ? Complete : NotComplete;
}

QString ConicB5P::sUseText( const Objects&, const Object* )
{
  return i18n("Through point");
}

void ConicB5P::sDrawPrelim( KigPainter& p, const Objects& os )
{
  Coordinate points[5];
  int i;

  // by now do now bother to draw a preliminar conic!
  // lets think about that later...
  uint size = os.size();
  if ( size != 5 ) return;
  assert( os[0]->toPoint() );
  assert( os[1]->toPoint() );
  assert( os[2]->toPoint() );
  assert( os[3]->toPoint() );
  assert( os[4]->toPoint() );
  os[4]->toPoint()->getCoord();

  p.setPen(QPen (Qt::red, 1));
  for (i = 0; i < 5; i++)
  {
    points[i] = os[i]->toPoint()->getCoord();
  }
//  calcCartesian( points );
//  calcPolar();
//  p.drawConic( mfocus1, pdimen, ecostheta0, esintheta0 );
// ... right now I have problems...
  return;
}

ConicB5P::ConicB5P( const Objects& os )
{
  assert( os.size() == 5 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( (*i)->toPoint() );
    (*i)->addChild( this );
    pts[i-os.begin()] = static_cast<Point*>( *i );
  };
}
