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
  virtual Point getPoint (double param) const = 0;
  virtual double getParam (const Point&) const = 0;
};

#endif // CURVE_H
