// abstractline.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "abstractline.h"

#include "../misc/coordinate.h"
#include "../misc/common.h"
#include "../misc/i18n.h"

AbstractLine::AbstractLine()
{
}

AbstractLine::AbstractLine( const AbstractLine& l )
  : Curve( l )
{
}

const Coordinate AbstractLine::direction() const
{
  return p2() - p1();
}

const uint AbstractLine::numberOfProperties() const
{
  return Curve::numberOfProperties() + 2;
}

const Property AbstractLine::property( uint which, const KigWidget& w ) const
{
  assert( which < AbstractLine::numberOfProperties() );
  if ( which < Curve::numberOfProperties() ) return Curve::property( which, w );
  if ( which == Curve::numberOfProperties() )
    return Property( slope() );
  else if ( which == Curve::numberOfProperties() + 1 )
    return Property( equationString( w ) );
  else assert( false );
}

const QCStringList AbstractLine::properties() const
{
  QCStringList l = Curve::properties();
  l << I18N_NOOP( "Slope" );
  l << I18N_NOOP( "Equation" );
  assert( l.size() == AbstractLine::numberOfProperties() );
  return l;
}

double AbstractLine::slope() const
{
  Coordinate diff = p2() - p1();
  return diff.y / diff.x;
}

const QString AbstractLine::equationString( const KigWidget& ) const
{
  QString ret = QString::fromUtf8( "y = %1x + %2" );
  Coordinate p = p1();
  Coordinate q = p2();

  double m = ( q.y - p.y ) / ( q.x - p.x );
  double r = ( q.y - p.y ) * p.x / ( q.x - p.x ) + p.y;

  ret = ret.arg( m, 0, 'g', 3 );
  ret = ret.arg( r, 0, 'g', 3 );

  return ret;
}

const LineData AbstractLine::lineData() const
{
  return LineData( p1(), p2() );
}

bool AbstractLine::isa( int type ) const
{
  return type == AbstractLineT ? true : Parent::isa( type );
}
