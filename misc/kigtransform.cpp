/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Maurizio Paolini <paolini@dmf.unicatt.it>

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

bool getProjectiveTransformation ( int argsnum,
    Object *transforms[], double lt[3][3] )
{
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      lt[i][j] = 0.;
    }
  }
  for (int i = 0; i < 3; i++) lt[i][i] = 1.;

  assert ( argsnum > 0 );
  int argn = 0;
  Object* transform = transforms[argn++];
  if (transform->toVector())
  {
    Vector* v = transform->toVector();
    Coordinate dir = v->getDir();
    lt[1][0] = dir.x;
    lt[2][0] = dir.y;
    assert (argn == argsnum);
    return true;
  }

  if (transform->toPoint())
  {
    Point* p = transform->toPoint();
    lt[1][1] = lt[2][2] = -1;
    lt[1][0] = 2*p->getX();
    lt[2][0] = 2*p->getY();
    assert (argn == argsnum);
    return true;
  }

  if (transform->toLine())
  {
    Line* line = transform->toLine();
    Coordinate a = line->p1();
    Coordinate d = line->direction();
    double dirnormsq = d.x*d.x + d.y*d.y;
    double adotnu = (a.y*d.x - a.x*d.y)/dirnormsq;
    lt[1][0] = -2*adotnu*d.y;
    lt[2][0] =  2*adotnu*d.x;
    lt[1][1] -= 2*d.y*d.y/dirnormsq;
    lt[2][2] -= 2*d.x*d.x/dirnormsq;
    lt[1][2]  = lt[2][1] = 2*d.x*d.y/dirnormsq;
    assert (argn == argsnum);
    return true;
  }

  if (transform->toRay())
  {
    Ray* line = transform->toRay();
    Coordinate d = line->direction().normalize();
    double alpha = 0.1*M_PI/2;  // a small angle for the DrawPrelim
    if (argn < argsnum)
    {
      Angle* angle = transforms[argn++]->toAngle();
      alpha = angle->size();
    }
    lt[0][0] =  cos(alpha);
    lt[1][1] =  cos(alpha)*d.x*d.x + d.y*d.y;
    lt[0][1] = -sin(alpha)*d.x;
    lt[1][0] =  sin(alpha)*d.x;
    lt[0][2] = -sin(alpha)*d.y;
    lt[2][0] =  sin(alpha)*d.y;
    lt[1][2] =  cos(alpha)*d.x*d.y - d.x*d.y;
    lt[2][1] =  cos(alpha)*d.x*d.y - d.x*d.y;
    lt[2][2] =  cos(alpha)*d.y*d.y + d.x*d.x;
    assert (argn == argsnum);
    return true;
  }

  if (transform->toAngle())
  {
    double x = 0.;
    double y = 0.;
    if (argn < argsnum)
    {
      Object* arg = transforms[argn++];
      assert (arg->toPoint());
      Point* p = arg->toPoint();
      x = p->getX();
      y = p->getY();
    }
    Angle* angle = transform->toAngle();
    double alpha = angle->size();
    lt[1][1] = lt[2][2] = cos(alpha);
    lt[1][2] = -sin(alpha);
    lt[2][1] = sin(alpha);
    lt[1][0] = x - lt[1][1]*x - lt[1][2]*y;
    lt[2][0] = y - lt[2][1]*x - lt[2][2]*y;
    assert (argn == argsnum);
    return true;
  }

  if (transform->toSegment())  // this is a scaling
  {
    Segment* segment = transform->toSegment();
    Coordinate p = segment->p2() - segment->p1();
    double s = p.length();
    double x = 0;
    double y = 0;
    lt[1][1] = lt[2][2] = s;
    if (argn < argsnum)
    {
      Object* arg = transforms[argn++];
      if (arg->toSegment())
      {
        Segment* segment = arg->toSegment();
        Coordinate p = segment->p2() - segment->p1();
        s /= p.length();
        if (argn < argsnum) arg = transforms[argn++];
      }
      if (arg->toPoint())   // scaling w.r. to a point
      {
        Point* p = arg->toPoint();
        x = p->getX();
        y = p->getY();
        lt[1][1] = lt[2][2] = s;
        lt[1][0] = x - s*x;
        lt[2][0] = y - s*y;
        assert (argn == argsnum);
        return true;
      }
      if (arg->toLine())  // scaling w.r. to a line
      {
        Line* line = arg->toLine();
        Coordinate a = line->p1();
        Coordinate d = line->direction();
        double dirnormsq = d.x*d.x + d.y*d.y;
        lt[1][1] = (d.x*d.x + s*d.y*d.y)/dirnormsq;
        lt[2][2] = (d.y*d.y + s*d.x*d.x)/dirnormsq;
        lt[1][2] = lt[2][1] = (d.x*d.y - s*d.x*d.y)/dirnormsq;

        lt[1][0] = a.x - lt[1][1]*a.x - lt[1][2]*a.y;
        lt[2][0] = a.y - lt[2][1]*a.x - lt[2][2]*a.y;
      }
    }
    assert (argn == argsnum);
    return true;
  }

  return false;
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

/*
 * decide if the given transformation is homotetic
 */

bool isHomoteticTransformation ( double transformation[3][3] )
{
  if (transformation[0][1] != 0 || transformation[0][2] != 0) return (false);
  // test the orthogonality of the matrix 2x2 of second and third rows
  // and columns
  if (fabs(fabs(transformation[1][1]) -
            fabs(transformation[2][2])) > 1e-8) return (false);
  if (fabs(fabs(transformation[1][2]) -
            fabs(transformation[2][1])) > 1e-8) return (false);

  return transformation[1][2] * transformation[2][1] *
         transformation[1][1] * transformation[2][2] <= 0.;
}

/*
 * transform a point (given in cartesian coordinates)
 * using a projective transformation
 */

const Coordinate calcTransformedPoint ( Coordinate p,
                  double transformation[3][3], bool& valid )
{
  double phom[3] = {1., p.x, p.y};
  double rhom[3] = {0., 0., 0.};


  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      rhom[i] += transformation[i][j]*phom[j];
    }
  }

  if (rhom[0] == 0.)
  {
    valid = false;
    return Coordinate (0., 0.);
  }

  return Coordinate (rhom[1]/rhom[0], rhom[2]/rhom[0]);
}
