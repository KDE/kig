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
#include "../misc/i18n.h"

AbstractLine::AbstractLine()
{
}

AbstractLine::AbstractLine( const AbstractLine& l )
  : Curve( l )
{
}

const Coordinate AbstractLine::direction()
{
  return p2() - p1();
}

const uint AbstractLine::numberOfProperties()
{
  return Curve::numberOfProperties() + 1;
}

const Property AbstractLine::property( uint which )
{
  assert( which < AbstractLine::numberOfProperties() );
  if ( which < Curve::numberOfProperties() ) return Curve::property( which );
  if ( which == Curve::numberOfProperties() )
    return Property( slope() );
  else assert( false );
}

const QStringList AbstractLine::properties()
{
  QStringList l = Curve::properties();
  l << i18n( "Slope" );
  assert( l.size() == AbstractLine::numberOfProperties() );
  return l;
}

double AbstractLine::slope() const
{
  Coordinate diff = p2() - p1();
  return diff.y / diff.x;
}
