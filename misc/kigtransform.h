/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Maurizio Paolini <paolini@dmf.unicatt.it>
 Copyright (C) 2003  Dominique Devriese <devriese@kde.org>

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#ifndef KIG_MISC_KIGTRANSFORM_H
#define KIG_MISC_KIGTRANSFORM_H

#include "coordinate.h"
#include <vector>

class LineData;

/**
 * Class representing a transformation.  More specifically, this class
 * represents a pretty generic 2-dimensional transformation. Various
 * common transformations can be used.  Construct a Transformation by
 * using one of its static members, and use it either with its
 * Transformation::apply method, or the ObjectImp::transform method.
 */
class Transformation
{
  double mdata[3][3];
  bool mIsHomothety;
  bool mIsAffine;
  Transformation();
public:
  ~Transformation();
  Transformation( double data[3][3], bool ishomothety );

  /**
   * Apply this Tranformation.  Apply this transformation to the
   * Coordinate c.  Can return an invalid Coordinate.
   * apply0 assumes that c indicates a point at infinity, having
   * [0, c.x, c.y] as homogeneous coordinates
   */
  const Coordinate apply( const double x0, const double x1, const double x2 ) const;
  const Coordinate apply( const Coordinate& c ) const;
  const Coordinate apply0( const Coordinate& c ) const;

  /**
   * Returns whether this is a homothetic (affine) transformation.
   */
  bool isHomothetic() const;
  bool isAffine() const;
  double getProjectiveIndicator( const Coordinate& c ) const;
  double getAffineDeterminant() const;
  double getRotationAngle() const;
  const Coordinate apply2by2only( const Coordinate& c ) const;
  /**
   * \ifnot creating-python-scripting-doc
   * a homothetic transformation maintains the ratio's of lengths.
   * This means that every length is multiplied by a fixed number when
   * it is projected...  This function does that calculation for
   * you..
   * \endif
   */
  double apply( double length ) const;
  double data( int r, int c ) const;
  /**
   * The inverse Transformation.  Returns the inverse Transformation
   * of this Transformation.
   */
  const Transformation inverse( bool& valid ) const;

  /**
   * Identity.  Returns the Identity Transformation, i.e. a
   * Transformation that doesn't do anything.
   */
  static const Transformation identity();
  /**
   * Scaling over Point.  Returns a Transformation that scales points
   * by a certain factor with relation to a center point.
   */
  static const Transformation scalingOverPoint( double factor, const Coordinate& center = Coordinate() );
  /**
   * Scaling over Line.  Returns a Transformation that scales points
   * by a certain factor with relation to a line.  Note: This is not a
   * homothetic transformation.
   */
  static const Transformation scalingOverLine( double factor, const LineData& l );
  /**
   * Translation.  Returns a Translation by a vector c.
   */
  static const Transformation translation( const Coordinate& c );
  /**
   * Rotation.  Returns a Rotation by a certain angle, around a
   * certain center.
   */
  static const Transformation rotation( double angle, const Coordinate& center = Coordinate() );
  /**
   * Point Reflection.  Returns a reflection over a point
   * \note This equals scaling( -1, c );
   */
  static const Transformation pointReflection( const Coordinate& c );
  /**
   * Line Reflection.  Returns a reflection over a line
   * \note This equals scaling( -1, l );
   */
  static const Transformation lineReflection( const LineData& l );
  /**
   * Harmonic Homology. Returns a Transformation that transforms points in
   * such a way that it appears to cast a shadow, given a certain
   * light source (center), and a line (axis) indicating a plane.
   */
  static const Transformation harmonicHomology( const Coordinate& center,
                                          const LineData& axis );
  /**
   * Affinity given the image of 3 points. Returns the unique
   * affinity that transforms 3 given points into 3 given points.
   */
  static const Transformation affinityGI3P(
                const std::vector<Coordinate>& FromPoints,
                const std::vector<Coordinate>& ToPoints,
                bool& valid );
  /**
   * Projectivity given the image of 4 points. Returns the unique
   * projectivity that transforms 4 given points into 4 given points.
   */
  static const Transformation projectivityGI4P(
                const std::vector<Coordinate>& FromPoints,
                const std::vector<Coordinate>& ToPoints,
                bool& valid );
  /**
   * Cast Shadow.  Returns a Transformation that transforms points in
   * such a way that it appears to cast a shadow, given a certain
   * light source, and a line indicating a plane.
   */
  static const Transformation castShadow( const Coordinate& ls,
                                          const LineData& d );
  /**
   * Projective Rotation.  This is really only a test example of a
   * projective non-affine transformation...
   */
  static const Transformation projectiveRotation( double alpha,
                                                  const Coordinate& d,
                                                  const Coordinate& t );

  /**
   * Similitude.  Sequence of a rotation and a scaling with relation
   * to a certain center.
   */
  static const Transformation similitude(
    const Coordinate& center, double theta, double factor );

  /**
   * Sequence.  This creates a Transformation that executes first
   * transformation b, and then a.
   */
  friend const Transformation operator*( const Transformation& a, const Transformation& b );

  /**
   * Equality.  Tests two Transformation's for equality.
   */
  friend bool operator==( const Transformation& lhs, const Transformation& rhs );
};

const Transformation operator*( const Transformation&, const Transformation& );
bool operator==( const Transformation& lhs, const Transformation& rhs );

// enum tWantArgsResult { tComplete, tNotComplete, tNotGood };

// Transformation getProjectiveTransformation(
//   int transformationsnum, Object *mtransformations[],
//   bool& valid );

// tWantArgsResult WantTransformation ( Objects::const_iterator& i,
//          const Objects& os );

// QString getTransformMessage ( const Objects& os, const Object *o );

// bool isHomoteticTransformation ( double transformation[3][3] );

#endif // KIG_MISC_KIGTRANSFORM_H
