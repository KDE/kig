/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/


#ifndef KIG_MISC_COORDINATE_H
#define KIG_MISC_COORDINATE_H

class QPoint;
class QDebug;

/**
 * The Coordinate class is the basic class representing a 2D location
 * by its x and y components.  It has all relevant arithmetic
 * operators properly defined, and should be straightforward to use..
 */
class Coordinate
{
public:
  static Coordinate fromQPoint( const QPoint& p );

  /** Constructor.  Construct a new Coordinate, with a given x and y
   * value.
   */
  Coordinate( double x, double y );
  /** Copy Constructor.  Construct a new Coordinate, and give it the
   *  same value as p.
   */
  Coordinate( const Coordinate& p );
  /**
   * \ifnot creating-python-scripting-doc
   * \brief Default Constructor
   *
   * Constructs a new Coordinate, with x and y initialized to 0.
   * \endif
   */
  Coordinate();
  ~Coordinate() {}

  /** Create an invalid Coordinate.  This is a special value of a
   * Coordinate that signals that something went wrong..
   *
   * \see Coordinate::valid
   *
   * \internal We represent an invalid coordinate by setting x or y to
   * positive or negative infinity.  This is handy, since it doesn't
   * require us to adapt most of the functions, it doesn't need extra
   * space, and most of the times that we should get an invalid coord,
   * we get one automatically..
   */
  static Coordinate invalidCoord();
  /** Return whether this is a valid Coordinate.
   * \see Coordinate::invalidCoord
   */
  bool valid() const;

  /** Distance to another Coordinate.
   */
  double distance ( const Coordinate& p ) const;
  /** Length.  Returns the length or norm of this coordinate.
   * I.e. return the distance from this Coordinate to the origin.
   * \see squareLength
   */
  double length () const;
  /** Square length.  Equivalent to the square of \ref length, but a
   * bit more efficient because no square root has to be calculated.
   * \see length
   */
  inline double squareLength() const;
  /** Inverse.  Returns the inverse of this Coordinate.
   */
  const Coordinate operator- () const;
  /** Orthogonal.  Returns a vector which is orthogonal on this vector.
   * This relation always holds:
   * <pre>
   * Coordinate a = ...;
   * assert( a*a.orthogonal() ) == 0;
   * </pre>
   */
  const Coordinate orthogonal() const;
  /** Round. Returns this coordinate, rounded to the nearest integral
   * values.
   */
  const Coordinate round() const;
  /** Normalize.  This sets the length to length, while keeping the
   * x/y ratio untouched...
   */
  const Coordinate normalize( double length = 1 ) const;
  QPoint toQPoint() const;

  Coordinate& operator= ( const Coordinate& c );
  /** Add.  Add c to this Coordinate
   */
  Coordinate& operator+= ( const Coordinate& c );
  /** Subtract.  Subtract c from this Coordinate
   */
  Coordinate& operator-= ( const Coordinate& c );
  /** Scale.  Scales this Coordinate by a factor r
   */
  Coordinate& operator*= ( double r );
  /** Scale.  Scales this Coordinate by a factor r
   */
  Coordinate& operator*= ( int r );
  /** Scale.  Scales this Coordinate by a factor 1/r
   */
  Coordinate& operator/= ( double r );
public:
  /** X Component.  The X Component of this Coordinate.
   */
  double x;
  /** Y Component.  The Y Component of this Coordinate.
   */
  double y;

  friend QDebug& operator<<( QDebug& s, const Coordinate& t );
  /** Add.  Returns the sum of a and b.
   */
  friend const Coordinate operator+ ( const Coordinate& a, const Coordinate& b );
  /** Subtract.  Returns the difference between a and b.
   */
  friend const Coordinate operator- ( const Coordinate& a, const Coordinate& b );
  /** Scale.  Returns this a, scaled by a factor of r.
   */
  friend const Coordinate operator* ( const Coordinate& a, double r );
  /** Scale.  Returns a, scaled by a factor of 1/r.
   */
  friend const Coordinate operator/ ( const Coordinate& a, double r );
  /** Scalar Product.  Returns the scalar product of a and b.
   */
  friend double operator*( const Coordinate& a, const Coordinate& b );
  /** Equal.  Tests two Coordinates for equality.
   */
  friend bool operator==( const Coordinate&, const Coordinate& );
  /** Not Equal.  Tests two Coordinates for inequality.
   */
  friend bool operator!=( const Coordinate&, const Coordinate& );
};

const Coordinate operator/ ( const Coordinate& a, double r );
QDebug& operator<<( QDebug& s, const Coordinate& t );
const Coordinate operator+ ( const Coordinate& a, const Coordinate& b );
const Coordinate operator- ( const Coordinate& a, const Coordinate& b );
const Coordinate operator* ( const Coordinate& a, double r );
const Coordinate operator* ( double r, const Coordinate& a );
double operator*( const Coordinate& a, const Coordinate& b );

double Coordinate::squareLength() const
{
  return x*x+y*y;
}

#endif

