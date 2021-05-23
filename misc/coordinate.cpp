// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "coordinate.h"

#include <qglobal.h>
#include <cmath>
#include <QDebug>

#include "common.h"

using namespace std;

Coordinate Coordinate::fromQPoint( const QPoint& p )
{
  return Coordinate( p.x(), p.y() );
}

QDebug& operator<<( QDebug& s, const Coordinate& t )
{
  s << "x:" << t.x << "y:" << t.y;
  return s;
}

const Coordinate operator+ ( const Coordinate& a, const Coordinate& b )
{
  return Coordinate ( a.x + b.x, a.y + b.y );
}

const Coordinate operator- ( const Coordinate& a, const Coordinate& b )
{
  return Coordinate ( a.x - b.x, a.y - b.y );
}

const Coordinate operator* ( const Coordinate& a, double r )
{
  return Coordinate ( r*a.x, r*a.y );
}

const Coordinate operator* ( double r, const Coordinate& a )
{
  return Coordinate ( r*a.x, r*a.y );
}

const Coordinate operator/ ( const Coordinate& a, double r )
{
  return Coordinate ( a.x/r, a.y/r );
}

bool operator==( const Coordinate& a, const Coordinate& b )
{
  return a.x == b.x && a.y == b.y;
}

bool operator!=( const Coordinate& a, const Coordinate& b )
{
  return !operator==( a, b );
}

Coordinate::Coordinate()
  : x(0),
    y(0)
{
}

Coordinate::Coordinate( double nx, double ny )
  : x( nx ),
    y( ny )
{
}

Coordinate::Coordinate( const Coordinate& p )
  : x( p.x ),
    y( p.y )
{
}

const Coordinate Coordinate::operator-() const
{
  return Coordinate ( -x, -y );
}

Coordinate& Coordinate::operator=( const Coordinate& p )
{
  x = p.x;
  y = p.y;
  return *this;
}

Coordinate& Coordinate::operator+=( const Coordinate& p )
{
  x += p.x;
  y += p.y;
  return *this;
}

Coordinate& Coordinate::operator-=( const Coordinate& p )
{
  x -= p.x;
  y -= p.y;
  return *this;
}

Coordinate& Coordinate::operator*=( double r )
{
  x *= r;
  y *= r;
  return *this;
}

Coordinate& Coordinate::operator*=( int r )
{
  x *= r;
  y *= r;
  return *this;
}

Coordinate& Coordinate::operator/=( double r )
{
  x /= r;
  y /= r;
  return *this;
}

double Coordinate::distance( const Coordinate& p ) const
{
  return (p - *this).length();
}

double Coordinate::length() const
{
  return sqrt(x*x+y*y);
}

const Coordinate Coordinate::orthogonal() const
{
  return Coordinate( -y, x );
}

const Coordinate Coordinate::normalize( double l ) const
{
  double oldlength = length();
  return ( *this * l ) / oldlength;
}

const Coordinate Coordinate::round() const
{
  return Coordinate( qRound( x ), qRound( y ) );
}

QPoint Coordinate::toQPoint() const
{
  Coordinate t = round();
  return QPoint( (int) t.x, (int) t.y );
}

Coordinate Coordinate::invalidCoord()
{
  return Coordinate( double_inf, double_inf );
}

bool Coordinate::valid() const
{
  return fabs( x ) != double_inf && fabs( y ) != double_inf;
}

double operator*( const Coordinate& a, const Coordinate& b )
{
  return a.x * b.x + a.y * b.y;
}
