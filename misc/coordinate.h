/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/


#ifndef COORDINATE_H
#define COORDINATE_H

#include <qpoint.h>
#include <kdebug.h>

// including cmath doesn't work properly here...
extern "C"
{
  double sqrt( double );
  double round( double );
}

struct Coordinate
{
public:
  inline Coordinate( const double x, const double y );
  inline Coordinate( const Coordinate& p );
  inline Coordinate();
  ~Coordinate() {};
  
  inline Coordinate operator+ ( const Coordinate& c ) const;
  inline Coordinate operator- ( const Coordinate& c ) const;
  inline Coordinate operator* ( const double r ) const;
  inline Coordinate operator/ ( const double r ) const;
  inline Coordinate operator- () const;
  inline double distance ( const Coordinate& p ) const;
  inline double length () const;
  inline double squareLength() const;
  // returns a vector which is orthogonal on this vector
  // this relation always holds:
  // <pre>
  // Coordinate a = ...;
  // assert( a*a.orthogonal() ) == -1;
  // </pre>
  inline Coordinate orthogonal() const;
  inline bool operator== ( const Coordinate& p ) const;
  inline bool operator!=( const Coordinate& p ) const;
  inline Coordinate round() const;
  inline Coordinate normalize() const;
  inline QPoint toQPoint() const;

  inline Coordinate& operator= ( const Coordinate& c );
  inline Coordinate& operator+= ( const Coordinate& c );
  inline Coordinate& operator-= ( const Coordinate& c );
  inline Coordinate& operator*= ( const double r );
  inline Coordinate& operator/= ( const double r );
public:
  double x;
  double y;
  friend kdbgstream& operator<<( kdbgstream& s, const Coordinate& t );
};

inline kdbgstream& operator<<( kdbgstream& s, const Coordinate& t )
{
  s << "x: " << t.x << "  y: " << t.y << endl;
  return s;
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

Coordinate Coordinate::operator+( const Coordinate& p ) const
{
  return Coordinate ( x + p.x, y + p.y );
};

Coordinate Coordinate::operator-( const Coordinate& p ) const
{
  return Coordinate ( x - p.x, y - p.y );
};

Coordinate Coordinate::operator*( const double r ) const
{
  return Coordinate ( r*x, r*y );
};

Coordinate Coordinate::operator/( const double r ) const
{
  return Coordinate ( x/r, y/r );
};

Coordinate Coordinate::operator-() const
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

Coordinate Coordinate::orthogonal() const
{
  return Coordinate( -y, x );
};

bool Coordinate::operator==( const Coordinate& p ) const
{
  return p.x == x && p.y == y;
};

bool Coordinate::operator!=( const Coordinate& p ) const
{
  return !operator==(p);
};

Coordinate Coordinate::normalize() const
{
  return operator/(length());
};

Coordinate Coordinate::round() const
{
  return Coordinate( ::round( x ), ::round( y ) );
};

QPoint Coordinate::toQPoint() const
{
  Coordinate t = round();
  return QPoint( t.x, t.y );
};

#endif

