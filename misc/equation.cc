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

#include "equation.h"

#include <qregexp.h>
#include <klocale.h>
#include <kglobal.h>
#include <cmath>
//#include <vector>

/*
 * Definitions for EquationString
 */

EquationString::EquationString( const QString s )
  : QString( s )
{
}

double EquationString::trunc( double d )
{
  if ( fabs( d ) < 1e-7 ) return 0.0;
  return d;
}

void EquationString::addTerm( double coeff, const QString monomial, bool& needsign )
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
  coeff = fabs( coeff );
  if ( monomial.isEmpty() || fabs( coeff - 1.0 ) > 1e-6 ) 
    append( KGlobal::locale()->formatNumber( coeff, 3 ) );
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
     ret += "x";
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
     ret += "x^"+n;
 }

 switch (m)
 {
   case 0:
     break;
   case 1:
     ret += "y";
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
     ret += "y^"+m;
 }

 return ret;
}

//struct EqElem
//{
//  double coeff;
//  int xpow;
//  int ypow;
//};
//
//static const QString mon_3 = QString/*::fromUtf8*/( "³" );
//static const QString mon_2 = QString/*::fromUtf8*/( "²" );
//
//static QString varToString( const char* var, int power, bool& outputted )
//{
//  outputted = true;
//  switch ( power )
//  {
//    case 0:
//      outputted = false;
//      return "";
//      break;
//    case 1:
//      return QString( var );
//      break;
//    case 2:
//      return QString( var ) + mon_2;
//      break;
//    case 3:
//      return QString( var ) + mon_3;
//      break;
//  }
//  return QString( var ) + "^" + QString::number( power );
//}
//
//static QString sign( double x )
//{
//  if ( x < 0 )
//    return "-";
//  else if ( x > 0 )
//    return "+";
// return "";
//}
//
//static QString coeffToString( double coeff, bool with_spaces, bool& iszero )
//{
//  iszero = false;
//  QString res;
//  if ( fabs( coeff ) < 1e-8 )
//  {
//    iszero = true;
//    return "";
//  }
//  else if ( coeff == 1 )
//  {
//    return "+";
//  }
//  else
//  {
//    return sign( coeff ) + ( with_spaces ? " " : "" ) + QString::number( fabs( coeff ) );
//  }
//}
//
//static QString EqElemToString( const EqElem& el, bool with_spaces )
//{
//  bool iszero = false;
//  QString res = coeffToString( el.coeff, with_spaces, iszero )
//                + ( with_spaces ? " " : "" );
//  if ( !iszero )
//  {
//    bool xout = true;
//    bool yout = true;
//    res += varToString( "x", el.xpow, xout )
//           + varToString( "y", el.ypow, yout );
//    if ( xout || yout )
//      res += ( with_spaces ? " " : "" );
//  }
//
//  return res;
//}
//
//
//class Equation::Private
//{
//public:
//  Private()
//    : m_show2ndMember( false ), m_2ndMemberValue( 0.0 )
//  {
//  }
//
//  std::vector<EqElem> m_elems;
//  bool m_show2ndMember;
//  double m_2ndMemberValue;
//};
//
//static int hash( const EqElem& el )
//{
//  // order by grade, then by x power and then by y power
//  return ( ( el.xpow + el.ypow ) << 8 ) | ( el.xpow << 4 ) | el.ypow;
//}
//
//Equation::Equation()
//  : d( new Private )
//{
//}
//
//void Equation::addTerm( double coeff, int xpower, int ypower )
//{
//  if ( fabs( coeff ) < 1e-8 )
//  {
//    removeTerm( xpower, ypower );
//    return;
//  }
//  EqElem el;
//  el.coeff = coeff;
//  el.xpow = xpower;
//  el.ypow = ypower;
//  int newhash = hash( el );
//  bool found = false;
//  for ( std::vector<EqElem>::iterator it = d->m_elems.begin(); it != d->m_elems.end() && !found; ++it )
//  {
//    int currhash = hash( *it );
//    if ( newhash > currhash )
//    {
//      d->m_elems.insert( it, el );
//      found = true;
//    }
//    else if ( newhash == currhash )
//    {
//      (*it).coeff = el.coeff;
//      found = true;
//    }
//  }
//  if ( !found )
//  {
//    d->m_elems.push_back( el );
//  }
//}
//
//void Equation::removeTerm( int xpower, int ypower )
//{
//  bool found = false;
//  for ( std::vector<EqElem>::iterator it = d->m_elems.begin(); it != d->m_elems.end() && !found; ++it )
//  {
//    if ( ( (*it).xpow == xpower ) && ( (*it).ypow == ypower ) )
//    {
//      d->m_elems.erase( it );
//      found = true;
//    }
//  }
//}
//
//bool Equation::term( double& value, int xpower, int ypower )
//{
//  bool found = false;
//  value = 0;
//  for ( std::vector<EqElem>::iterator it = d->m_elems.begin(); it != d->m_elems.end() && !found; ++it )
//  {
//    if ( ( (*it).xpow == xpower ) && ( (*it).ypow == ypower ) )
//    {
//      value = (*it).coeff;
//      found = true;
//    }
//  }
//  return found;
//}
//
//void Equation::clear()
//{
//  d->m_elems.clear();
//}
//
//void Equation::setSecondMemberShown( bool show )
//{
//  d->m_show2ndMember = show;
//}
//
//bool Equation::isSecondMemberShown() const
//{
//  return d->m_show2ndMember;
//}
//
//void Equation::setSecondMemberValue( double value )
//{
//  d->m_2ndMemberValue = value;
//}
//
//double Equation::secondMemberValue() const
//{
//  return d->m_2ndMemberValue;
//}
//
//QString Equation::prettyString( bool with_spaces ) const
//{
//  QString res;
//  if ( d->m_elems.size() == 0 ) return res;
//
//  for ( std::vector<EqElem>::iterator it = d->m_elems.begin(); it != d->m_elems.end(); ++it )
//    res += EqElemToString( *it, with_spaces );
//  if ( d->m_show2ndMember )
//    res += QString( "=" ) + ( with_spaces ? " " : "" ) + QString::number( d->m_2ndMemberValue );
//
//  res.replace( QRegExp("^\\+\\s?"), "" );
//  res.replace( QRegExp("\\s?$"), "" );
//
//  return res;
//}

