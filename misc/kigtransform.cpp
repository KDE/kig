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

#include "../objects/point.h"
#include "../objects/line.h"
#include "../objects/vector.h"
#include "../objects/ray.h"
#include "../objects/angle.h"
#include "../objects/circle.h"
#include "../objects/segment.h"
#include "kigtransform.h"
#include "i18n.h"

#include <cmath>
#include "../objects/object.h"

Transformation getProjectiveTransformation ( int argsnum,
    Object *transforms[], bool& valid )
{
  valid = true;

  assert ( argsnum > 0 );
  int argn = 0;
  Object* transform = transforms[argn++];
  if (transform->toVector())
  {
    // translation
    assert (argn == argsnum);
    Vector* v = transform->toVector();
    Coordinate dir = v->getDir();
    return Transformation::translation( dir );
  }

  if (transform->toPoint())
  {
    // point reflection ( or is point symmetry the correct term ? )
    assert (argn == argsnum);
    Point* p = transform->toPoint();
    return Transformation::pointReflection( p->getCoord() );
  }

  if (transform->toLine())
  {
    // line reflection ( or is it line symmetry ? )
    Line* line = transform->toLine();
    assert (argn == argsnum);
    return Transformation::lineReflection( line->lineData() );
  }

  if (transform->toRay())
  {
    // domi: sorry, but what kind of transformation does this do ?
    //       i'm guessing it's some sort of rotation, but i'm not
    //       really sure..
//     Ray* line = transform->toRay();
//     Coordinate d = line->direction().normalize();
//     double alpha = 0.1*M_PI/2;  // a small angle for the DrawPrelim
//     if (argn < argsnum)
//     {
//       Angle* angle = transforms[argn++]->toAngle();
//       alpha = angle->size();
//     }
//     lt[0][0] =  cos(alpha);
//     lt[1][1] =  cos(alpha)*d.x*d.x + d.y*d.y;
//     lt[0][1] = -sin(alpha)*d.x;
//     lt[1][0] =  sin(alpha)*d.x;
//     lt[0][2] = -sin(alpha)*d.y;
//     lt[2][0] =  sin(alpha)*d.y;
//     lt[1][2] =  cos(alpha)*d.x*d.y - d.x*d.y;
//     lt[2][1] =  cos(alpha)*d.x*d.y - d.x*d.y;
//     lt[2][2] =  cos(alpha)*d.y*d.y + d.x*d.x;
//     assert (argn == argsnum);
//     return true;
  }

  if (transform->toAngle())
  {
    // rotation..
    Coordinate center = Coordinate( 0., 0. );
    if (argn < argsnum)
    {
      Object* arg = transforms[argn++];
      assert (arg->toPoint());
      center = arg->toPoint()->getCoord();
    }
    Angle* angle = transform->toAngle();
    double alpha = angle->size();

    assert (argn == argsnum);

    return Transformation::rotation( alpha, center );
  }

  if (transform->toSegment())  // this is a scaling
  {
    Segment* segment = transform->toSegment();
    Coordinate p = segment->p2() - segment->p1();
    double s = p.length();
    if (argn < argsnum)
    {
      Object* arg = transforms[argn++];
      if (arg->toSegment()) // s is the length of the first segment
                            // divided by the length of the second..
      {
        Segment* segment = arg->toSegment();
        Coordinate p = segment->p2() - segment->p1();
        s /= p.length();
        if (argn < argsnum) arg = transforms[argn++];
      }
      if (arg->toPoint())   // scaling w.r. to a point
      {
        Point* p = arg->toPoint();
        assert (argn == argsnum);
        return Transformation::scaling( s, p->getCoord() );
      }
      if (arg->toLine())  // scaling w.r. to a line
      {
        Line* line = arg->toLine();
        assert( argn == argsnum );
        return Transformation::scaling( s, line->lineData() );
      }
    }

    // domi: is this reached ?  can i assert( false ) here ?
    assert (argn == argsnum);
    return Transformation::identity();
  }

  valid = false;
  return Transformation::identity();
}

tWantArgsResult WantTransformation ( Objects::const_iterator& i,
      const Objects& os )
{
  Object* o = *i++;
  if (o->toVector()) return tComplete;
  if (o->toPoint()) return tComplete;
  if (o->toLine()) return tComplete;
  if (o->toAngle())
  {
    if ( i == os.end() ) return tNotComplete;
    o = *i++;
    if (o->toPoint()) return tComplete;
    if (o->toLine()) return tComplete;
    return tNotGood;
  }
  if (o->toRay())
  {
    if ( i == os.end() ) return tNotComplete;
    o = *i++;
    if (o->toAngle()) return tComplete;
    return tNotGood;
  }
  if (o->toSegment())
  {
    if ( i == os.end() ) return tNotComplete;
    o = *i++;
    if ( o->toSegment() )
    {
      if ( i == os.end() ) return tNotComplete;
      o = *i++;
    }
    if (o->toPoint()) return tComplete;
    if (o->toLine()) return tComplete;
    return tNotGood;
  }
  return tNotGood;
}

QString getTransformMessage ( const Objects& os, const Object *o )
{
  int size = os.size();
  switch (size)
  {
    case 1:
    if (o->toVector()) return i18n("translate by this vector");
    if (o->toPoint()) return i18n("central symmetry by this point. You"
     " can obtain different transformations by clicking on lines (reflection),"
     " vectors (translation), angles (rotation), segments (scaling) and rays"
     " (projective transformation)");
    if (o->toLine()) return i18n("reflect by this line");
    if (o->toAngle()) return i18n("rotate by this angle");
    if (o->toSegment()) return i18n("scale using the length of this vector");
    if (o->toRay()) return i18n("a projective transformation in the direction"
     " indicated by this ray, it is a rotation in the projective plane"
     " about a point at infinity");
    return i18n("Use this transformation");

    case 2:   // we ask for the first parameter of the transformation
    case 3:
    if (os[1]->toAngle())
    {
      if (o->toPoint()) return i18n("about this point");
      assert (false);
    }
    if (os[1]->toSegment())
    {
      if (o->toSegment())
        return i18n("relative to the length of this other vector");
      if (o->toPoint())
        return i18n("about this point");
      if (o->toLine())
        return i18n("about this line");
    }
    if (os[1]->toRay())
    {
      if (o->toAngle()) return i18n("rotate by this angle in the projective"
       " plane");
    }
    return i18n("Using this object");

    default: assert(false);
  }

  return i18n("Use this transformation");
}


/* domi: not necessary anymore, homotheticness is kept as a bool in
 *       the Transformation class..
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
  ret.mIsHomothety = true;
  return ret;
}

const Transformation Transformation::scaling( double factor, const Coordinate& center )
{
  Transformation ret;
  for ( int i = 0; i < 3; ++i )
    for ( int j = 0; j < 3; ++j )
      ret.mdata[i][j] = ( i == j ? factor : 0 );
  ret.mdata[0][0] = 1;
  ret.mdata[1][0] = center.x - factor * center.x;
  ret.mdata[2][0] = center.y - factor * center.y;
  ret.mIsHomothety = true;
  return ret;
}

const Transformation Transformation::translation( const Coordinate& c )
{
  Transformation ret = identity();
  ret.mdata[1][0] = c.x;
  ret.mdata[2][0] = c.y;
  return ret;
}

const Transformation Transformation::pointReflection( const Coordinate& c )
{
  return scaling( -1, c );
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

  ret.mIsHomothety = a.mIsHomothety && b.mIsHomothety;

  return ret;
};

const Transformation Transformation::lineReflection( const LineData& l )
{
  return scaling( -1, l );
}

const Transformation Transformation::scaling( double factor, const LineData& l )
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

  ret.mIsHomothety = true;
  return ret;
}

const Coordinate Transformation::apply( const Coordinate& p, bool& valid ) const
{
  valid = true;
  double phom[3] = {1., p.x, p.y};
  double rhom[3];


  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      rhom[i] += mdata[i][j]*phom[j];
    }
  }

  if (rhom[0] == 0.)
  {
    valid = false;
    return Coordinate (0., 0.);
  }

  return Coordinate (rhom[1]/rhom[0], rhom[2]/rhom[0]);
}

const Transformation Transformation::rotation( double alpha, const Coordinate& center )
{
  Transformation ret = identity();

  double x = center.x;
  double y = center.y;

  ret.mdata[1][1] = ret.mdata[2][2] = cos(alpha);
  ret.mdata[1][2] = -sin(alpha);
  ret.mdata[2][1] = sin(alpha);
  ret.mdata[1][0] = x - ret.mdata[1][1]*x - ret.mdata[1][2]*y;
  ret.mdata[2][0] = y - ret.mdata[2][1]*x - ret.mdata[2][2]*y;

  return ret;
}

bool Transformation::isHomothetic() const
{
  return mIsHomothety;
}

double Transformation::data( int r, int c ) const
{
  return mdata[r][c];
}

const Transformation Transformation::inverse( bool& valid ) const
{
  Transformation ret;

  valid = Invert3by3matrix( mdata, ret.mdata );

  // domi: is the inverse of a homothetic matrix a homothety ?
  ret.mIsHomothety = false;

}
