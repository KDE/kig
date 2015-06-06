// Copyright (C)  2005  Pino Toscano <toscano.pino@tiscali.it>

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

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
  return QString::fromUtf8( "x³" );
}

const QString EquationString::y3() const
{
  return QString::fromUtf8( "y³" );
}

const QString EquationString::x2y() const
{
  return QString::fromUtf8( "x²y" );
}

const QString EquationString::xy2() const
{
  return QString::fromUtf8( "xy²" );
}

const QString EquationString::x2() const
{
  return QString::fromUtf8( "x²" );
}

const QString EquationString::y2() const
{
  return QString::fromUtf8( "y²" );
}

const QString EquationString::xy() const
{
  return "xy";
}

const QString EquationString::x() const
{
  return "x";
}

const QString EquationString::y() const
{
  return "y";
}

/* used in the circle equation */

void EquationString::prettify( void )
{
  replace( "( x )", "x" );
  replace( "( y )", "y" );
}

const QString EquationString::xnym(int n, int m) const
{
 QString ret="";
 switch (n)
 {
   case 0: 
     break;
   case 1:
     ret += 'x';
     break;
   case 2:
     ret += QString::fromUtf8( "x²" );
     break;
   case 3:
     ret += QString::fromUtf8( "x³" );
     break;
   case 4: 
     ret += QString::fromUtf8( "x⁴" );
     break;
   case 5:
     ret += QString::fromUtf8( "x⁵" );
     break;
   case 6:
     ret += QString::fromUtf8( "x⁶" );
     break;
   default:
     ret += QString::fromLatin1( "x^" ) + QString::number( n );
 }

 switch (m)
 {
   case 0:
     break;
   case 1:
     ret += 'y';
     break;
   case 2:
     ret += QString::fromUtf8( "y²" );
     break;
   case 3:
     ret += QString::fromUtf8( "y³" );
     break;
   case 4:
     ret += QString::fromUtf8( "y⁴" );
     break;
   case 5:
     ret += QString::fromUtf8( "y⁵" );
     break;
   case 6:
     ret += QString::fromUtf8( "y⁶" );
     break;
   default:
     ret += QString::fromLatin1( "y^" ) + QString::number( n );
 }

 return ret;
}
