/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2006 Pino Toscano <toscano.pino@tiscali.it>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "unit.h"

#include <QDebug>

#include <KLocalizedString>

Unit::Unit( double value, Unit::MetricalUnit unit, int dpi )
{
  mvalue = value;
  munit = unit;
  mdpi = dpi;
}

Unit::~Unit()
{
}

void Unit::setValue( double value )
{
  mvalue = value;
}

double Unit::value() const
{
  return mvalue;
}

void Unit::setUnit( Unit::MetricalUnit unit )
{
  munit = unit;
}

void Unit::convertTo( Unit::MetricalUnit unit )
{
  mvalue = convert( mvalue, munit, unit, mdpi );
  munit = unit;
}

Unit::MetricalUnit Unit::unit() const
{
  return munit;
}

double Unit::getValue( Unit::MetricalUnit unit ) const
{
  return convert( mvalue, munit, unit, mdpi );
}

void Unit::setDpi( int dpi )
{
  mdpi = dpi;
}

int Unit::dpi() const
{
  return mdpi;
}

Unit& Unit::operator=( const Unit& u )
{
  mvalue = u.mvalue;
  munit = u.munit;
  mdpi = u.mdpi;
  return *this;
}

double Unit::convert( double value, Unit::MetricalUnit from, Unit::MetricalUnit to, int dpi )
{
  switch ( from )
  {
    case pixel:
    {
      if ( to == cm )
        return value / dpi * 2.53995;
      if ( to == in )
        return value / dpi;
      break;
    }
    case cm:
    {
      if ( to == pixel )
        return value * 0.3937 * dpi;
      if ( to == in )
        return value * 0.3937;
      break;
    }
    case in:
    {
      if ( to == pixel )
        return value * dpi;
      if ( to == cm )
        return value * 2.53995;
      break;
    }
  }
  return value;
}

QStringList Unit::unitList()
{
  QStringList ul;
  ul << i18nc( "Translators: Pixel", "pixel" );
  ul << i18nc( "Translators: Centimeter", "cm" );
  ul << i18nc( "Translators: Inch", "in" );
  return ul;
}

Unit::MetricalUnit Unit::intToUnit( int index )
{
  if ( index == 0 )
    return pixel;
  else if ( index == 1 )
    return cm;
  else if ( index == 2 )
    return in;
  qDebug() << "No measure unit with index " << index;
  return pixel;
}

int Unit::precision( Unit::MetricalUnit unit )
{
  if ( unit == Unit::pixel )
    return 0;
  else
    return 2;
}
