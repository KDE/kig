#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "point.h"

class Segment;
class Line;
class Circle;

class IntersectionPoint
  : public Point
{
  Segment* segment1;
  Segment* segment2;
  Line* line1;
  Line* line2;
  Circle* circle1;
  Circle* circle2;
public:
  IntersectionPoint() : segment1(0), segment2(0), line1(0), line2(0), circle1(0), circle2(0) {};
  ~IntersectionPoint() {};
  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "IntersectionPoint"; };

  void moved( ) { calc(); };
  QString wantArg(const Object* o) const;
  bool selectArg(Object* o);
  void unselectArg(Object* o);
  Objects getParents() const;
  
  void startMove(const QPoint& ) {};
  void moveTo(const QPoint& ) {};
  void stopMove() {};
  void cancelMove() {};
  
protected:
  void calc();
  static QPoint calc(const Point& p1, const Point& p2, const Point& p3, const Point& p4);
};

#endif
