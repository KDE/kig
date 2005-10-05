/**
   This file is part of Kig, a KDE program for Interactive Geometry...
   Copyright (C) 2004  Dominique Devriese <devriese@kde.org>
   Copyright (C) 2004  Pino Toscano <toscano.pino@tiscali.it>

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
   USA
**/


#include "goniometry.h"

#include <qstringlist.h>

#include <kdebug.h>
#include <klocale.h>

#include <cmath>

Goniometry::Goniometry()
{
  mvalue = 0.0;
  msys = Rad;
}

Goniometry::Goniometry( double value, Goniometry::System system )
{
  mvalue = value;
  msys = system;
}

Goniometry::~Goniometry()
{
}

void Goniometry::setValue( double value )
{
  mvalue = value;
}

const double Goniometry::value() const
{
  return mvalue;
}

void Goniometry::setSystem( Goniometry::System system )
{
  msys = system;
}

void Goniometry::convertTo( Goniometry::System system )
{
  mvalue = convert( mvalue, msys, system );
  msys = system;
}

const Goniometry::System Goniometry::system() const
{
  return msys;
}

double Goniometry::getValue( Goniometry::System system )
{
  return convert( mvalue, msys, system );
}

Goniometry& Goniometry::operator=( const Goniometry& g )
{
  mvalue = g.value();
  msys = g.system();
  return *this;
}

double Goniometry::convert( const double angle, const Goniometry::System from, const Goniometry::System to )
{
  switch( from )
  {
    case Deg:
    {
      if ( to == Rad )
        return angle * M_PI / 180;
      if ( to == Grad )
        return angle * 10 / 9;
      break;
    }
    case Rad:
    {
      if ( to == Deg )
        return angle * 180 / M_PI;
      if ( to == Grad )
        return angle * 200 / M_PI;
      break;
    }
    case Grad:
    {
      if ( to == Deg )
        return angle * 9 / 10;
      if ( to == Rad )
        return angle * M_PI / 200;
      break;
    }
  }
  return angle;
}

QStringList Goniometry::systemList()
{
  QStringList sl;
  sl << i18n( "Translators: Degrees", "Deg" );
  sl << i18n( "Translators: Radians", "Rad" );
  sl << i18n( "Translators: Gradians", "Grad" );
  return sl;
}

Goniometry::System Goniometry::intToSystem( const int index )
{
  if( index == 0 )
    return Deg;
  else if( index == 1 )
    return Rad;
  else if( index == 2 )
    return Grad;
  kdDebug() << "No goniometric system with index " << index << endl;
  return Rad;
}
