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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef KIG_MISC_KIGTRANSFORM_H
#define KIG_MISC_KIGTRANSFORM_H

#include "coordinate.h"

class LineData;

class Transformation
{
  double mdata[3][3];
  bool mIsHomothety;
  Transformation();
public:
  ~Transformation();

  const Coordinate apply( const Coordinate& c, bool& valid ) const;

  bool isHomothetic() const;
  double data( int r, int c ) const;
  const Transformation inverse( bool& valid ) const;

  // don't do anything..
  static const Transformation identity();
  // scale over a point..
  static const Transformation scaling( double factor, const Coordinate& center = Coordinate() );
  // scale over a line..
  static const Transformation scaling( double factor, const LineData& l );
  // translate..
  static const Transformation translation( const Coordinate& c );
  // rotate..
  static const Transformation rotation( double angle, const Coordinate& center = Coordinate() );
  // reflect over a point..  this equals scaling( -1, c )
  static const Transformation pointReflection( const Coordinate& c );
  // reflect over a line.. this equals scaling( -1, l );
  static const Transformation lineReflection( const LineData& l );
  // this is a test example of a projective non-affine transformation
  static const Transformation projectiveRotation( double alpha,
                                              const Coordinate& d,
                                              const Coordinate& t );

  friend const Transformation operator*( const Transformation&, const Transformation& );
};

const Transformation operator*( const Transformation&, const Transformation& );


class Object;
class Objects;

enum tWantArgsResult { tComplete, tNotComplete, tNotGood };

Transformation getProjectiveTransformation(
  int transformationsnum, Object *mtransformations[],
  bool& valid );

tWantArgsResult WantTransformation ( Objects::const_iterator& i,
         const Objects& os );

QString getTransformMessage ( const Objects& os, const Object *o );

// bool isHomoteticTransformation ( double transformation[3][3] );

#endif // KIG_MISC_KIGTRANSFORM_H
