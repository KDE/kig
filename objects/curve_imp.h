// curve_imp.h
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef CURVE_IMP_H
#define CURVE_IMP_H

#include "object_imp.h"

//  abc (abstract base class) representing a curve: something which is
//  composed of points, like a line, a circle, a locus...
class CurveImp
  : public ObjectImp
{
  typedef ObjectImp Parent;
public:
  // param is between 0 and 1.  Note that 0 and 1 should be the
  // end-points.  E.g. for a Line, getPoint(0) returns a more or less
  // infinite point.  getPoint(0.5) should return the point in the
  // middle.
  virtual double getParam( const Coordinate& point, const KigDocument& ) const = 0;
  // this should be the inverse function of getPoint().
  // Note that it should also do something reasonable when p is not on
  // the curve.
  virtual const Coordinate getPoint( double param, bool& valid, const KigDocument& ) const = 0;
  bool inherits( int type ) const;
  virtual CurveImp* copy() const = 0;
};

#endif
