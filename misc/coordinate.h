/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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

class Coordinate
{
public:
  static Coordinate fromQPoint( const QPoint& p );

  Coordinate( const double x, const double y );
  Coordinate( const Coordinate& p );
  Coordinate();
  ~Coordinate() {};

  double distance ( const Coordinate& p ) const;
  double length () const;
  inline double squareLength() const;
  const Coordinate operator- () const;
  // returns a vector which is orthogonal on this vector
  // this relation always holds:
  // <pre>
  // Coordinate a = ...;
  // assert( a*a.orthogonal() ) == -1;
  // </pre>
  const Coordinate orthogonal() const;
  const Coordinate round() const;
  // this sets our length to length, while keeping the x/y ratio
  // untouched...
  const Coordinate normalize( double length = 1 ) const;
  QPoint toQPoint() const;

  Coordinate& operator= ( const Coordinate& c );
  Coordinate& operator+= ( const Coordinate& c );
  Coordinate& operator-= ( const Coordinate& c );
  Coordinate& operator*= ( const double r );
  Coordinate& operator*= ( const int r );
  Coordinate& operator/= ( const double r );
public:
  double x;
  double y;
  friend kdbgstream& operator<<( kdbgstream& s, const Coordinate& t );
  friend const Coordinate operator+ ( const Coordinate& a, const Coordinate& b );
  friend const Coordinate operator- ( const Coordinate& a, const Coordinate& b );
  friend const Coordinate operator* ( const Coordinate& a, const double r );
  friend const Coordinate operator/ ( const Coordinate& a, const double r );
  friend bool operator==( const Coordinate&, const Coordinate& );
  friend bool operator!=( const Coordinate&, const Coordinate& );
};

const Coordinate operator/ ( const Coordinate& a, const double r );
kdbgstream& operator<<( kdbgstream& s, const Coordinate& t );
const Coordinate operator+ ( const Coordinate& a, const Coordinate& b );
const Coordinate operator- ( const Coordinate& a, const Coordinate& b );
const Coordinate operator* ( const Coordinate& a, const double r );
const Coordinate operator* ( const double r, const Coordinate& a );

double Coordinate::squareLength() const
{
  return x*x+y*y;
};

#endif

