// coordinate.cpp
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

#include "coordinate.h"

Coordinate Coordinate::fromQPoint( const QPoint& p )
{
  return Coordinate( p.x(), p.y() );
}

kdbgstream& operator<<( kdbgstream& s, const Coordinate& t )
{
  s << "x: " << t.x << "  y: " << t.y << endl;
  return s;
};

const Coordinate operator+ ( const Coordinate& a, const Coordinate& b )
{
  return Coordinate ( a.x + b.x, a.y + b.y );
};

const Coordinate operator- ( const Coordinate& a, const Coordinate& b )
{
  return Coordinate ( a.x - b.x, a.y - b.y );
};

const Coordinate operator* ( const Coordinate& a, const double r )
{
  return Coordinate ( r*a.x, r*a.y );
};

const Coordinate operator* ( const double r, const Coordinate& a )
{
  return Coordinate ( r*a.x, r*a.y );
};

const Coordinate operator/ ( const Coordinate& a, const double r )
{
  return Coordinate ( a.x/r, a.y/r );
};

bool operator==( const Coordinate& a, const Coordinate& b )
{
  return a.x == b.x && a.y == b.y;
};

bool operator!=( const Coordinate& a, const Coordinate& b )
{
  return !operator==( a, b );
};

Coordinate::Coordinate()
  : x(0),
    y(0)
{
};

Coordinate::Coordinate( const double nx, const double ny )
  : x( nx ),
    y( ny )
{
};

Coordinate::Coordinate( const Coordinate& p )
  : x( p.x ),
    y( p.y )
{
};

const Coordinate Coordinate::operator-() const
{
  return Coordinate ( -x, -y );
};

Coordinate& Coordinate::operator=( const Coordinate& p )
{
  x = p.x;
  y = p.y;
  return *this;
};

Coordinate& Coordinate::operator+=( const Coordinate& p )
{
  x += p.x;
  y += p.y;
  return *this;
};

Coordinate& Coordinate::operator-=( const Coordinate& p )
{
  x -= p.x;
  y -= p.y;
  return *this;
};

Coordinate& Coordinate::operator*=( const double r )
{
  x *= r;
  y *= r;
  return *this;
};

Coordinate& Coordinate::operator/=( const double r )
{
  x /= r;
  y /= r;
  return *this;
};

double Coordinate::distance( const Coordinate& p ) const
{
  return (p - *this).length();
};

double Coordinate::length() const
{
  return sqrt(x*x+y*y);
};

double Coordinate::squareLength() const
{
  return x*x+y*y;
};

const Coordinate Coordinate::orthogonal() const
{
  return Coordinate( -y, x );
};

const Coordinate Coordinate::normalize( double l ) const
{
  return ( *this * l ) / length();
};

const Coordinate Coordinate::round() const
{
  return Coordinate( ::round( x ), ::round( y ) );
};

QPoint Coordinate::toQPoint() const
{
  Coordinate t = round();
  return QPoint( t.x, t.y );
};
