/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Maurizio Paolini <paolini@dmf.unicatt.it>
    SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kigtransform.h"

#include "kignumerics.h"
#include "common.h"

#include <cmath>

#include <QDebug>

using std::fabs;
using std::cos;
using std::sin;
using std::atan2;

// Transformation getProjectiveTransformation ( int argsnum,
//     Object *transforms[], bool& valid )
// {
//   valid = true;

//   assert ( argsnum > 0 );
//   int argn = 0;
//   Object* transform = transforms[argn++];
//   if (transform->toVector())
//   {
//     // translation
//     assert (argn == argsnum);
//     Vector* v = transform->toVector();
//     Coordinate dir = v->getDir();
//     return Transformation::translation( dir );
//   }

//   if (transform->toPoint())
//   {
//     // point reflection ( or is point symmetry the correct term ? )
//     assert (argn == argsnum);
//     Point* p = transform->toPoint();
//     return Transformation::pointReflection( p->getCoord() );
//   }

//   if (transform->toLine())
//   {
//     // line reflection ( or is it line symmetry ? )
//     Line* line = transform->toLine();
//     assert (argn == argsnum);
//     return Transformation::lineReflection( line->lineData() );
//   }

//   if (transform->toRay())
//   {
//     // domi: sorry, but what kind of transformation does this do ?
//     //       I'm guessing it's some sort of rotation, but I'm not
//     //       really sure..
//      Ray* line = transform->toRay();
//      Coordinate d = line->direction().normalize();
//      Coordinate t = line->p1();
//      double alpha = 0.1*M_PI/2;  // a small angle for the DrawPrelim
//      if (argn < argsnum)
//      {
//        Angle* angle = transforms[argn++]->toAngle();
//        alpha = angle->size();
//      }
//      assert (argn == argsnum);
//      return Transformation::projectiveRotation( alpha, d, t );
//   }

//   if (transform->toAngle())
//   {
//     // rotation..
//     Coordinate center = Coordinate( 0., 0. );
//     if (argn < argsnum)
//     {
//       Object* arg = transforms[argn++];
//       assert (arg->toPoint());
//       center = arg->toPoint()->getCoord();
//     }
//     Angle* angle = transform->toAngle();
//     double alpha = angle->size();

//     assert (argn == argsnum);

//     return Transformation::rotation( alpha, center );
//   }

//   if (transform->toSegment())  // this is a scaling
//   {
//     Segment* segment = transform->toSegment();
//     Coordinate p = segment->p2() - segment->p1();
//     double s = p.length();
//     if (argn < argsnum)
//     {
//       Object* arg = transforms[argn++];
//       if (arg->toSegment()) // s is the length of the first segment
//                             // divided by the length of the second..
//       {
//         Segment* segment = arg->toSegment();
//         Coordinate p = segment->p2() - segment->p1();
//         s /= p.length();
//         if (argn < argsnum) arg = transforms[argn++];
//       }
//       if (arg->toPoint())   // scaling w.r. to a point
//       {
//         Point* p = arg->toPoint();
//         assert (argn == argsnum);
//         return Transformation::scaling( s, p->getCoord() );
//       }
//       if (arg->toLine())  // scaling w.r. to a line
//       {
//         Line* line = arg->toLine();
//         assert( argn == argsnum );
//         return Transformation::scaling( s, line->lineData() );
//       }
//     }

//     return Transformation::scaling( s, Coordinate( 0., 0. ) );
//   }

//   valid = false;
//   return Transformation::identity();
// }

// tWantArgsResult WantTransformation ( Objects::const_iterator& i,
//       const Objects& os )
// {
//   Object* o = *i++;
//   if (o->toVector()) return tComplete;
//   if (o->toPoint()) return tComplete;
//   if (o->toLine()) return tComplete;
//   if (o->toAngle())
//   {
//     if ( i == os.end() ) return tNotComplete;
//     o = *i++;
//     if (o->toPoint()) return tComplete;
//     if (o->toLine()) return tComplete;
//     return tNotGood;
//   }
//   if (o->toRay())
//   {
//     if ( i == os.end() ) return tNotComplete;
//     o = *i++;
//     if (o->toAngle()) return tComplete;
//     return tNotGood;
//   }
//   if (o->toSegment())
//   {
//     if ( i == os.end() ) return tNotComplete;
//     o = *i++;
//     if ( o->toSegment() )
//     {
//       if ( i == os.end() ) return tNotComplete;
//       o = *i++;
//     }
//     if (o->toPoint()) return tComplete;
//     if (o->toLine()) return tComplete;
//     return tNotGood;
//   }
//   return tNotGood;
// }

// QString getTransformMessage ( const Objects& os, const Object *o )
// {
//   int size = os.size();
//   switch (size)
//   {
//     case 1:
//     if (o->toVector()) return i18n("translate by this vector");
//     if (o->toPoint()) return i18n("central symmetry by this point. You"
//      " can obtain different transformations by clicking on lines (mirror),"
//      " vectors (translation), angles (rotation), segments (scaling) and rays"
//      " (projective transformation)");
//     if (o->toLine()) return i18n("reflect in this line");
//     if (o->toAngle()) return i18n("rotate by this angle");
//     if (o->toSegment()) return i18n("scale using the length of this vector");
//     if (o->toRay()) return i18n("a projective transformation in the direction"
//      " indicated by this ray, it is a rotation in the projective plane"
//      " about a point at infinity");
//     return i18n("Use this transformation");

//     case 2:   // we ask for the first parameter of the transformation
//     case 3:
//     if (os[1]->toAngle())
//     {
//       if (o->toPoint()) return i18n("about this point");
//       assert (false);
//     }
//     if (os[1]->toSegment())
//     {
//       if (o->toSegment())
//         return i18n("relative to the length of this other vector");
//       if (o->toPoint())
//         return i18n("about this point");
//       if (o->toLine())
//         return i18n("about this line");
//     }
//     if (os[1]->toRay())
//     {
//       if (o->toAngle()) return i18n("rotate by this angle in the projective"
//        " plane");
//     }
//     return i18n("Using this object");

//     default: assert(false);
//   }

//   return i18n("Use this transformation");
// }


/* domi: not necessary anymore, homotheticness is kept as a bool in
 *       the Transformation class..
 * keeping it here, in case a need for it arises some time in the
 * future...
 * decide if the given transformation is homotetic
 */
// bool isHomoteticTransformation ( double transformation[3][3] )
// {
//   if (transformation[0][1] != 0 || transformation[0][2] != 0) return (false);
//   // test the orthogonality of the matrix 2x2 of second and third rows
//   // and columns
//   if (fabs(fabs(transformation[1][1]) -
//             fabs(transformation[2][2])) > 1e-8) return (false);
//   if (fabs(fabs(transformation[1][2]) -
//             fabs(transformation[2][1])) > 1e-8) return (false);

//   return transformation[1][2] * transformation[2][1] *
//          transformation[1][1] * transformation[2][2] <= 0.;
// }

const Transformation Transformation::identity()
{
  Transformation ret;
  for ( int i = 0; i < 3; ++i )
    for ( int j = 0; j < 3; ++j )
      ret.mdata[i][j] = ( i == j ? 1 : 0 );
  ret.mIsHomothety = ret.mIsAffine = true;
  return ret;
}

const Transformation Transformation::scalingOverPoint( double factor, const Coordinate& center )
{
  Transformation ret;
  for ( int i = 0; i < 3; ++i )
    for ( int j = 0; j < 3; ++j )
      ret.mdata[i][j] = ( i == j ? factor : 0 );
  ret.mdata[0][0] = 1;
  ret.mdata[1][0] = center.x - factor * center.x;
  ret.mdata[2][0] = center.y - factor * center.y;
  ret.mIsHomothety = ret.mIsAffine = true;
  return ret;
}

const Transformation Transformation::translation( const Coordinate& c )
{
  Transformation ret = identity();
  ret.mdata[1][0] = c.x;
  ret.mdata[2][0] = c.y;

  // this is already set in the identity() constructor, but just for
  // clarity..
  ret.mIsHomothety = ret.mIsAffine = true;
  return ret;
}

const Transformation Transformation::pointReflection( const Coordinate& c )
{
  Transformation ret = scalingOverPoint( -1, c );
  ret.mIsHomothety = ret.mIsAffine = true;
  return ret;
}

const Transformation operator*( const Transformation& a, const Transformation& b )
{
  // just multiply the two matrices..
  Transformation ret;

  for ( int i = 0; i < 3; ++i )
    for ( int j = 0; j < 3; ++j )
    {
      ret.mdata[i][j] = 0;
      for ( int k = 0; k < 3; ++k )
        ret.mdata[i][j] += a.mdata[i][k] * b.mdata[k][j];
    };

  // combination of two homotheties is a homothety..

  ret.mIsHomothety = a.mIsHomothety && b.mIsHomothety;

  // combination of two affinities is affine..

  ret.mIsAffine = a.mIsAffine && b.mIsAffine;

  return ret;
}

const Transformation Transformation::lineReflection( const LineData& l )
{
  Transformation ret = scalingOverLine( -1, l );
  // a reflection is a homothety...
  ret.mIsHomothety = ret.mIsAffine = true;
  return ret;
}

const Transformation Transformation::scalingOverLine( double factor, const LineData& l )
{
  Transformation ret = identity();

  Coordinate a = l.a;
  Coordinate d = l.dir();
  double dirnormsq = d.squareLength();
  ret.mdata[1][1] = (d.x*d.x + factor*d.y*d.y)/dirnormsq;
  ret.mdata[2][2] = (d.y*d.y + factor*d.x*d.x)/dirnormsq;
  ret.mdata[1][2] = ret.mdata[2][1] = (d.x*d.y - factor*d.x*d.y)/dirnormsq;

  ret.mdata[1][0] = a.x - ret.mdata[1][1]*a.x - ret.mdata[1][2]*a.y;
  ret.mdata[2][0] = a.y - ret.mdata[2][1]*a.x - ret.mdata[2][2]*a.y;

  // domi: is 1e-8 a good value ?
  ret.mIsHomothety = ( fabs( factor - 1 ) < 1e-8 || fabs ( factor + 1 ) < 1e-8 );
  ret.mIsAffine = true;
  return ret;
}

const Transformation Transformation::harmonicHomology(
  const Coordinate& center, const LineData& axis )
{
  // this is a well known projective transformation.  We find it by first
  // computing the homogeneous equation of the axis ax + by + cz = 0
  // then a straightforward computation shows that the 3x3 matrix describing
  // the transformation is of the form:
  //
  // (r . C) Id - 2 (C tensor r)
  //
  // where r = [c, a, b], C = [1, Cx, Cy], Cx and Cy are the coordinates of
  // the center, '.' denotes the scalar product, Id is the identity matrix,
  // 'tensor' is the tensor product producing a 3x3 matrix.
  //
  // note: here we decide to use coordinate '0' in place of the third coordinate
  // in homogeneous notation; e.g. C = [1, cx, cy]

  Coordinate pointa = axis.a;
  Coordinate pointb = axis.b;

  double a = pointa.y - pointb.y;
  double b = pointb.x - pointa.x;
  double c = pointa.x*pointb.y - pointa.y*pointb.x;

  double cx = center.x;
  double cy = center.y;

  double scalprod = a*cx + b*cy + c;
  scalprod *= 0.5;
  Transformation ret;

  ret.mdata[0][0]  = c - scalprod;
  ret.mdata[0][1]  = a;
  ret.mdata[0][2]  = b;

  ret.mdata[1][0]  = c*cx;
  ret.mdata[1][1]  = a*cx - scalprod;
  ret.mdata[1][2]  = b*cx;

  ret.mdata[2][0]  = c*cy;
  ret.mdata[2][1]  = a*cy;
  ret.mdata[2][2]  = b*cy - scalprod;

  ret.mIsHomothety = ret.mIsAffine = false;
  return ret;
}

const Transformation Transformation::affinityGI3P(
  const std::vector<Coordinate>& FromPoints,
  const std::vector<Coordinate>& ToPoints,
  bool& valid )
{
  // construct the (generically) unique affinity that transforms 3 given
  // point into 3 other given points; i.e. it depends on the coordinates of
  // a total of 6 points.  This actually amounts in solving a 6x6 linear
  // system to find the entries of a 2x2 linear transformation matrix T
  // and of a translation vector t.
  // If Pi denotes one of the starting points and Qi the corresponding
  // final position we actually have to solve: Qi = t + T Pi, for i=1,2,3
  // (each one is a vector equation, so that it really gives 2 equations).
  // In our context T and t are used to build a 3x3 projective transformation
  // as follows:
  //
  //    [  1  0   0  ]
  //    [ t1 T11 T12 ]
  //    [ t2 T21 T22 ]
  //
  // In order to take advantage of the two functions "GaussianElimination"
  // and "BackwardSubstitution", which are specifically aimed at solving
  // homogeneous underdetermined linear systems, we just add a further
  // unknown m and solve for t + T Pi - m Qi = 0.  Since our functions
  // returns a nonzero solution we shall have a nonzero 'm' in the end and
  // can build the 3x3 matrix as follows:
  //
  //    [  m  0   0  ]
  //    [ t1 T11 T12 ]
  //    [ t2 T21 T22 ]
  //
  // we order the unknowns as follows: m, t1, t2, T11, T12, T21, T22

  double row0[7], row1[7], row2[7], row3[7], row4[7], row5[7];

  double *matrix[6] = {row0, row1, row2, row3, row4, row5};

  double solution[7];
  int scambio[7];

  assert (FromPoints.size() == 3);
  assert (ToPoints.size() == 3);

  // fill in the matrix elements
  for ( int i = 0; i < 6; i++ )
  {
    for ( int j = 0; j < 7; j++ )
    {
      matrix[i][j] = 0.0;
    }
  }

  for ( int i = 0; i < 3; i++ )
  {
    Coordinate p = FromPoints[i];
    Coordinate q = ToPoints[i];
    matrix[i][0] = -q.x;
    matrix[i][1] = 1.0;
    matrix[i][3] = p.x;
    matrix[i][4] = p.y;
    matrix[i+3][0] = -q.y;
    matrix[i+3][2] = 1.0;
    matrix[i+3][5] = p.x;
    matrix[i+3][6] = p.y;
  }

  Transformation ret;
  valid = true;
  if ( ! GaussianElimination( matrix, 6, 7, scambio ) )
    { valid = false; return ret; }

  // fine della fase di eliminazione
  BackwardSubstitution( matrix, 6, 7, scambio, solution );

  // now we can build the 3x3 transformation matrix; remember that
  // unknown 0 is the multiplicator 'm'

  ret.mdata[0][0] = solution[0];
  ret.mdata[0][1] = ret.mdata[0][2] = 0.0;
  ret.mdata[1][0] = solution[1];
  ret.mdata[2][0] = solution[2];
  ret.mdata[1][1] = solution[3];
  ret.mdata[1][2] = solution[4];
  ret.mdata[2][1] = solution[5];
  ret.mdata[2][2] = solution[6];

  ret.mIsHomothety = false;
  ret.mIsAffine = true;
  return ret;
}

const Transformation Transformation::projectivityGI4P(
  const std::vector<Coordinate>& FromPoints,
  const std::vector<Coordinate>& ToPoints,
  bool& valid )
{
  // construct the (generically) unique projectivity that transforms 4 given
  // point into 4 other given points; i.e. it depends on the coordinates of
  // a total of 8 points.  This actually amounts in solving an underdetermined
  // homogeneous linear system.

  double
    row0[13], row1[13], row2[13], row3[13], row4[13], row5[13], row6[13], row7[13],
    row8[13], row9[13], row10[13], row11[13];

  double *matrix[12] = {row0, row1, row2, row3, row4, row5, row6, row7,
                        row8, row9, row10, row11};

  double solution[13];
  int scambio[13];

  assert (FromPoints.size() == 4);
  assert (ToPoints.size() == 4);

  // fill in the matrix elements
  for ( int i = 0; i < 12; i++ )
  {
    for ( int j = 0; j < 13; j++ )
    {
      matrix[i][j] = 0.0;
    }
  }

  for ( int i = 0; i < 4; i++ )
  {
    Coordinate p = FromPoints[i];
    Coordinate q = ToPoints[i];
    matrix[i][0] = matrix[4+i][3] = matrix[8+i][6] = 1.0;
    matrix[i][1] = matrix[4+i][4] = matrix[8+i][7] = p.x;
    matrix[i][2] = matrix[4+i][5] = matrix[8+i][8] = p.y;
    matrix[i][9+i] = -1.0;
    matrix[4+i][9+i] = -q.x;
    matrix[8+i][9+i] = -q.y;
  }

  Transformation ret;
  valid = true;
  if ( ! GaussianElimination( matrix, 12, 13, scambio ) )
    { valid = false; return ret; }

  // fine della fase di eliminazione
  BackwardSubstitution( matrix, 12, 13, scambio, solution );

  // now we can build the 3x3 transformation matrix; remember that
  // unknowns from 9 to 13 are just multiplicators that we don't need here

  int k = 0;
  for ( int i = 0; i < 3; i++ )
  {
    for ( int j = 0; j < 3; j++ )
    {
      ret.mdata[i][j]  = solution[k++];
    }
  }

  ret.mIsHomothety = ret.mIsAffine = false;
  return ret;
}

const Transformation Transformation::castShadow(
  const Coordinate& lightsrc, const LineData& l )
{
  // first deal with the line l, I need to find an appropriate reflection
  // that transforms l onto the x-axis

  Coordinate d = l.dir();
  Coordinate a = l.a;
  double k = d.length();
  if ( d.x < 0 ) k *= -1;         // for numerical stability
  Coordinate w = d + Coordinate( k, 0 );
  // w /= w.length();
  // w defines a Householder transformation, but we don't need to normalize
  // it here.
  // warning: this w is the orthogonal of the w of the textbooks!
  // this is fine for us since in this way it indicates the line direction
  Coordinate ra = Coordinate ( a.x + w.y*a.y/(2*w.x), a.y/2 );
  Transformation sym = lineReflection ( LineData( ra, ra + w ) );

  // in the new coordinates the line is the x-axis
  // I must transform the point

  Coordinate modlightsrc = sym.apply ( lightsrc );
  Transformation ret = identity();
  // parameter t indicates the distance of the light source from
  // the plane of the drawing. A negative value means that the light
  // source is behind the plane.
  double t = -1.0;
  //  double t = -modlightsrc.y;    <-- this gives the old transformation!
  double e = modlightsrc.y - t;
  ret.mdata[0][0] =  e;
  ret.mdata[0][2] = -1;
  ret.mdata[1][1] =  e;
  ret.mdata[1][2] = -modlightsrc.x;
  ret.mdata[2][2] =  -t;

  ret.mIsHomothety = ret.mIsAffine = false;
  return sym*ret*sym;
//  return translation( t )*ret*translation( -t );
}

const Transformation Transformation::projectiveRotation(
  double alpha, const Coordinate& d, const Coordinate& t )
{
  Transformation ret;
  double cosalpha = cos( alpha );
  double sinalpha = sin( alpha );
  ret.mdata[0][0] =  cosalpha;
  ret.mdata[1][1] =  cosalpha*d.x*d.x + d.y*d.y;
  ret.mdata[0][1] = -sinalpha*d.x;
  ret.mdata[1][0] =  sinalpha*d.x;
  ret.mdata[0][2] = -sinalpha*d.y;
  ret.mdata[2][0] =  sinalpha*d.y;
  ret.mdata[1][2] =  cosalpha*d.x*d.y - d.x*d.y;
  ret.mdata[2][1] =  cosalpha*d.x*d.y - d.x*d.y;
  ret.mdata[2][2] =  cosalpha*d.y*d.y + d.x*d.x;

  ret.mIsHomothety = ret.mIsAffine = false;
  return translation( t )*ret*translation( -t );
}

const Coordinate Transformation::apply( const double x0,
					const double x1,
					const double x2) const
{
  double phom[3] = {x0, x1, x2};
  double rhom[3] = {0., 0., 0.};


  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      rhom[i] += mdata[i][j]*phom[j];
    }
  }

  if (rhom[0] == 0.)
    return Coordinate::invalidCoord();

  return Coordinate (rhom[1]/rhom[0], rhom[2]/rhom[0]);
}

const Coordinate Transformation::apply( const Coordinate& p ) const
{
  return apply( 1., p.x, p.y );
//  double phom[3] = {1., p.x, p.y};
//  double rhom[3] = {0., 0., 0.};
//
//  for (int i = 0; i < 3; i++)
//  {
//    for (int j = 0; j < 3; j++)
//    {
//      rhom[i] += mdata[i][j]*phom[j];
//    }
//  }
//
//  if (rhom[0] == 0.)
//    return Coordinate::invalidCoord();
//
//  return Coordinate (rhom[1]/rhom[0], rhom[2]/rhom[0]);
}

const Coordinate Transformation::apply0( const Coordinate& p ) const
{
  return apply( 0., p.x, p.y );
}

const Transformation Transformation::rotation( double alpha, const Coordinate& center )
{
  Transformation ret = identity();

  double x = center.x;
  double y = center.y;

  double cosalpha = cos( alpha );
  double sinalpha = sin( alpha );

  ret.mdata[1][1] = ret.mdata[2][2] = cosalpha;
  ret.mdata[1][2] = -sinalpha;
  ret.mdata[2][1] = sinalpha;
  ret.mdata[1][0] = x - ret.mdata[1][1]*x - ret.mdata[1][2]*y;
  ret.mdata[2][0] = y - ret.mdata[2][1]*x - ret.mdata[2][2]*y;

  // this is already set in the identity() constructor, but just for
  // clarity..
  ret.mIsHomothety = ret.mIsAffine = true;

  return ret;
}

bool Transformation::isHomothetic() const
{
  return mIsHomothety;
}

bool Transformation::isAffine() const
{
  return mIsAffine;
}

/*
 *mp:
 * this function has the property that it changes sign if computed
 * on two points that lie on either sides with respect to the critical
 * line (this is the line that goes to the line at infinity).
 * For affine transformations the result has always the same sign.
 * NOTE: the result is *not* invariant under rescaling of all elements
 * of the transformation matrix.
 * The typical use is to determine whether a segment is transformed
 * into a segment or a couple of half-lines.
 */

double Transformation::getProjectiveIndicator( const Coordinate& c ) const
{
  return mdata[0][0] + mdata[0][1]*c.x + mdata[0][2]*c.y;
}

// assuming that this is an affine transformation, return its
// determinant.  What is really important here is just the sign
// of the determinant.
double Transformation::getAffineDeterminant() const
{
  return mdata[1][1]*mdata[2][2] - mdata[1][2]*mdata[2][1];
}

// this assumes that the 2x2 affine part of the matrix is of the
// form [ cos a, sin a; -sin a, cos a] or a multiple
double Transformation::getRotationAngle() const
{
  return atan2( mdata[1][2], mdata[1][1] );
}

const Coordinate Transformation::apply2by2only( const Coordinate& p ) const
{
  double x = p.x;
  double y = p.y;
  double nx = mdata[1][1]*x + mdata[1][2]*y;
  double ny = mdata[2][1]*x + mdata[2][2]*y;
  return Coordinate( nx, ny );
}

double Transformation::data( int r, int c ) const
{
  return mdata[r][c];
}

const Transformation Transformation::inverse( bool& valid ) const
{
  Transformation ret;

  valid = Invert3by3matrix( mdata, ret.mdata );

  // the inverse of a homothety is a homothety, same for affinities..
  ret.mIsHomothety = mIsHomothety;
  ret.mIsAffine = mIsAffine;

  return ret;
}

Transformation::Transformation()
{
  // this is the constructor used by the static Transformation
  // creation functions, so mIsHomothety is in general false
  mIsHomothety = mIsAffine = false;
  for ( int i = 0; i < 3; ++i )
    for ( int j = 0; j < 3; ++j )
      mdata[i][j] = ( i == j ) ? 1 : 0;
}

Transformation::~Transformation()
{
}

double Transformation::apply( double length ) const
{
  assert( isHomothetic() );
  double det = mdata[1][1]*mdata[2][2] -
               mdata[1][2]*mdata[2][1];
  return std::sqrt( fabs( det ) ) * length;
}

Transformation::Transformation( double data[3][3], bool ishomothety )
  : mIsHomothety( ishomothety )
{
  for ( int i = 0; i < 3; ++i )
    for ( int j = 0; j < 3; ++j )
      mdata[i][j] = data[i][j];

  //mp: a test for affinity is used to initialize mIsAffine...
  mIsAffine = false;
  if ( fabs(mdata[0][1]) + fabs(mdata[0][2]) < 1e-8 * fabs(mdata[0][0]) )
    mIsAffine = true;
}

bool operator==( const Transformation& lhs, const Transformation& rhs )
{
  for ( int i = 0; i < 3; ++i )
    for ( int j = 0; j < 3; ++j )
      if ( lhs.data( i, j ) != rhs.data( i, j ) )
        return false;
  return true;
}

const Transformation Transformation::similitude(
  const Coordinate& center, double theta, double factor )
{
  Transformation ret;
  ret.mIsHomothety = true;
  double costheta = cos( theta );
  double sintheta = sin( theta );
  ret.mdata[0][0] = 1;
  ret.mdata[0][1] = 0;
  ret.mdata[0][2] = 0;
  ret.mdata[1][0] = ( 1 - factor*costheta )*center.x + factor*sintheta*center.y;
  ret.mdata[1][1] = factor*costheta;
  ret.mdata[1][2] = -factor*sintheta;
  ret.mdata[2][0] = -factor*sintheta*center.x + ( 1 - factor*costheta )*center.y;
  ret.mdata[2][1] = factor*sintheta;
  ret.mdata[2][2] = factor*costheta;
  // fails for factor == infinity
  //assert( ( ret.apply( center ) - center ).length() < 1e-5 );
  ret.mIsHomothety = ret.mIsAffine = true;
  return ret;
}
