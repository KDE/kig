/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>
    SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "goniometry.h"

#include <cmath>

#include <QDebug>

#include <KLocalizedString>

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

double Goniometry::value() const
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

Goniometry::System Goniometry::system() const
{
  return msys;
}

double Goniometry::getValue( Goniometry::System system )
{
  return convert( mvalue, msys, system );
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
  sl << i18nc( "Translators: Degrees", "Deg" );
  sl << i18nc( "Translators: Radians", "Rad" );
  sl << i18nc( "Translators: Gradians", "Grad" );
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
  qDebug() << "No goniometric system with index " << index;
  return Rad;
}
