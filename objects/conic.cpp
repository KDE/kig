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

  pos = Coordinate(o) - mfocus1;
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

  p.drawConic( mfocus1, pdimen, ecostheta0, esintheta0 );
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

  costheta = cos(p * 2 * M_PI);
  sintheta = sin(p * 2 * M_PI);
  rho = pdimen / (1 - costheta*ecostheta0 - sintheta*esintheta0);
  return mfocus1 + Coordinate (costheta, sintheta) * rho;
};

double Conic::getParam (const Coordinate& p) const
{
  Coordinate tmp = p - mfocus1;

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

void EllipseBFFP::calc( const ScreenInfo& )
{
  Coordinate f2f1;
  double f2f1l, rhomin, rhomax, d1, d2, eccentricity;

  if (poc->valid() && focus1->valid() && focus2->valid() )
  {
    f2f1 = focus2->getCoord() - focus1->getCoord();
    f2f1l = f2f1.length();
    ecostheta0 = f2f1.x / f2f1l;
    esintheta0 = f2f1.y / f2f1l;

    d1 = (poc->getCoord() - focus1->getCoord()).length();
    d2 = (poc->getCoord() - focus2->getCoord()).length();
    rhomin = (d1 + d2 - f2f1l) /2.0;
    rhomax = (d1 + d2 + f2f1l) /2.0;
    eccentricity = (rhomax - rhomin) / (rhomax + rhomin);
    ecostheta0 *= eccentricity;
    esintheta0 *= eccentricity;
    pdimen = (1 - eccentricity)*rhomax;

    mvalid = true;
    mfocus1 = focus1->getCoord();
  }
  else
    mvalid = false;
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
  : Curve( c ), mfocus1( c.mfocus1 ), pdimen( c.pdimen ),
    ecostheta0( c.ecostheta0 ),
    esintheta0( c.esintheta0 )
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
  double f2f1l, rhomin, rhomax, d1, d2, lpdimen, eccentricity;
  double lcostheta0, lsintheta0;

  if ( args.size() != 3 ) return;
  if ( ! ( args[0]->toPoint() && args[1]->toPoint() && args[2]->toPoint() ) ) return;
  Coordinate f1 = args[0]->toPoint()->getCoord();
  Coordinate f2 = args[1]->toPoint()->getCoord();
  Coordinate d = args[2]->toPoint()->getCoord();
  p.setPen(QPen (Qt::red, 1));

  Coordinate f2f1 = f2 - f1;
  f2f1l = f2f1.length();
  lcostheta0 = f2f1.x / f2f1l;
  lsintheta0 = f2f1.y / f2f1l;

  d1 = (d - f1).length();
  d2 = (d - f2).length();
  rhomin = (d1 + d2 - f2f1l) /2.0;
  rhomax = (d1 + d2 + f2f1l) /2.0;
  eccentricity = (rhomax - rhomin) / (rhomax + rhomin);
  lcostheta0 *= eccentricity;
  lsintheta0 *= eccentricity;
  lpdimen = (1 - eccentricity)*rhomax;

  p.drawConic( f1, lpdimen, lcostheta0, lsintheta0 );
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
