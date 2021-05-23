// SPDX-FileCopyrightText: 2005 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "equationstring.h"

#include <cmath>

#include <QLocale>

/*
 * Definitions for EquationString
 */

EquationString::EquationString( const QString& s )
  : QString( s )
{
}

double EquationString::trunc( double d )
{
  if ( std::fabs( d ) < 1e-7 ) return 0.0;
  return d;
}

void EquationString::addTerm( double coeff, const QString& monomial, bool& needsign )
{
  if ( trunc( coeff ) == 0.0 ) return;
  if ( needsign )
  {
    if ( coeff < 0 )
    {
      append( " - " );
    } else {
      append( " + " );
    }
  } else {
    needsign = true;
    if ( coeff < 0 )
    {
      append( "- " );
    }
  }
  coeff = std::fabs( coeff );
  if ( monomial.isEmpty() || std::fabs( coeff - 1.0 ) > 1e-6 ) 
    append( QLocale().toString( coeff, 'f', 3 ) );
  if ( !monomial.isEmpty() )
  {
    append( " " );
    append( monomial );
  }
  return;
}

const QString EquationString::x3() const
{
  return QStringLiteral( "x³" );
}

const QString EquationString::y3() const
{
  return QStringLiteral( "y³" );
}

const QString EquationString::x2y() const
{
  return QStringLiteral( "x²y" );
}

const QString EquationString::xy2() const
{
  return QStringLiteral( "xy²" );
}

const QString EquationString::x2() const
{
  return QStringLiteral( "x²" );
}

const QString EquationString::y2() const
{
  return QStringLiteral( "y²" );
}

const QString EquationString::xy() const
{
  return QStringLiteral("xy");
}

const QString EquationString::x() const
{
  return QStringLiteral("x");
}

const QString EquationString::y() const
{
  return QStringLiteral("y");
}

/* used in the circle equation */

void EquationString::prettify( void )
{
  replace( QLatin1String("( x )"), QLatin1String("x") );
  replace( QLatin1String("( y )"), QLatin1String("y") );
}

const QString EquationString::xnym(int n, int m) const
{
 QString ret=QLatin1String("");
 switch (n)
 {
   case 0: 
     break;
   case 1:
     ret += 'x';
     break;
   case 2:
     ret += QStringLiteral( "x²" );
     break;
   case 3:
     ret += QStringLiteral( "x³" );
     break;
   case 4: 
     ret += QStringLiteral( "x⁴" );
     break;
   case 5:
     ret += QStringLiteral( "x⁵" );
     break;
   case 6:
     ret += QStringLiteral( "x⁶" );
     break;
   default:
     ret += QLatin1String( "x^" ) + QString::number( n );
 }

 switch (m)
 {
   case 0:
     break;
   case 1:
     ret += 'y';
     break;
   case 2:
     ret += QStringLiteral( "y²" );
     break;
   case 3:
     ret += QStringLiteral( "y³" );
     break;
   case 4:
     ret += QStringLiteral( "y⁴" );
     break;
   case 5:
     ret += QStringLiteral( "y⁵" );
     break;
   case 6:
     ret += QStringLiteral( "y⁶" );
     break;
   default:
     ret += QLatin1String( "y^" ) + QString::number( n );
 }

 return ret;
}
