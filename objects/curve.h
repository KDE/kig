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


#ifndef CURVE_H
#define CURVE_H

#include "point.h"
#include "object.h"

// abc (abstract base class) representing a curve: something which is composed
// of points, like a line, a circle, a locus...
class Curve
: public Object
{
public:
  Curve* toCurve() { return this; };
  const Curve* toCurve() const { return this; };

  // param is between 0 and 1.  Note that 0 and 1 should be the
  // end-points.  E.g. for a Line, getPoint(0) returns an infinite
  // point.  getPoint(0.5) should return the point in the middle.
  virtual Coordinate getPoint (double param) const = 0;
  // this should be the inverse function of getPoint().
  // Note that it should also do something reasonable when p is not on
  // the curve.
  virtual double getParam (const Coordinate& p) const = 0;
};

#endif // CURVE_H
