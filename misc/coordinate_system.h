#ifndef COORDINATES_H
#define COORDINATES_H

#include "kigpainter.h"
#include "coordinate.h"

#include <qobject.h>

// a CoordinateSystem is what the user sees: it is kept by KigPart to
// show the user a grid, and to show the coordinates of points... it
// allows for weird CoordinateSystem's like homogeneous or
// projective...
// internally, it does nothing, it could almost have been an ordinary
// Object..., mapping coordinates from and to the screen to and from
// the internal coordinates is done by another class
// (InternalCoords)... 
class CoordinateSystem
  : public Qt
{
public:
  CoordinateSystem() {};
  virtual ~CoordinateSystem() {};
  virtual QString fromScreen (const Coordinate& pt) const = 0;
  virtual Coordinate toScreen (const QString& pt, bool& ok) const = 0;
  virtual void drawGrid ( KigPainter& p ) const = 0;
};

class EuclideanCoords
  : public CoordinateSystem
{
public:
  EuclideanCoords();
  ~EuclideanCoords() {};
  virtual QString fromScreen (const Coordinate& pt) const;
  virtual Coordinate toScreen (const QString& pt, bool& ok) const;
  virtual void drawGrid ( KigPainter& p ) const;
};

#endif
