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
