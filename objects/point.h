#ifndef POINT_H
#define POINT_H

#include <qpoint.h>
#include <qrect.h>
#include <qcolor.h>

#include <cmath>

#include "object.h"

class Point
: public Object
{
 public:
  Point() {};
  Point(double inX, double inY) : x(inX), y(inY) { complete = true;};
  Point(const QPoint& p) : x(p.x()), y(p.y()) { complete = true; };
  Point(const Point& p) : Object(p), x(p.getX()), y(p.getY()) {};

  void saveXML(QDomDocument& doc, QDomElement& parentElem);

  QPoint toQPoint() const { return QPoint(qRound(x),qRound(y));};

  // type identification
  virtual QCString vBaseTypeName() const { return sBaseTypeName();};
  static QCString sBaseTypeName() { return I18N_NOOP("point"); };
  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "Point"; };

  // general members
  bool contains (const QPoint& o, bool strict) const;
  void draw (QPainter& p,bool showSelection = true) const;
  void drawPrelim( QPainter &, const QPoint& ) const {};
  void moved(Object*) {};
  bool inRect(const QRect& r) const { return r.contains(toQPoint()); };
  // passing arguments
  QString wantArg(const Object*) const { return 0; };
  bool selectArg( Object *) { return true; }; // no args
  void unselectArg( Object *) {}; // no args
    // no args => no parents
    Objects getParents() const { return Objects();};
  // looks
  QColor getColor() { return Qt::black; };
  void setColor(const QColor&) {};
  //moving
  void startMove(const QPoint&);
  void moveTo(const QPoint&);
  void stopMove();
//   void cancelMove();
  void calc(){};

protected:
  QPoint pwwlmt; // point where we last moved to
  QPoint pwwsm;  // point where we started moving

public:
  double length () { return hypot(x,y); };
protected:
  double x, y;
public:
  double getX() const { return x;};
  double getY() const { return y;};
  void setX (const double inX) { x = inX; };
  void setY (const double inY) { y = inY; };

public:
  Point operator* (double factor) const { return Point(factor*x, factor*y); };
  Point& operator*= (double factor) { x*=factor; y*=factor; return *this; };
  Point& operator/= (double factor) { x/=factor; y/=factor; return *this; };
  Point operator+ (const Point& b) const { return Point(x+b.getX(), y+b.getY()); };
  Point operator- (const Point& b) const { return Point(x-b.getX(), y-b.getY()); };
  bool operator!=(const Point& p) const { return !operator==(p); };
  bool operator==(const Point& p) const { return x==p.getX() && y==p.getY(); };

  // sets length to one, while keeping x/y constant
  Point& normalize() { x = x/length(); y = y/length(); return *this;};

  void getOverlay(QPtrList<QRect>& list, const QRect& border) const
  { 
    QRect* tmp = new QRect(x-5, y-5,10,10); if (tmp->intersects(border)) list.append(tmp); else delete tmp;
  };
  void getPrelimOverlay(QPtrList<QRect>& , const QRect& , const QPoint& ) const {};
};

// midpoint of two other points
class MidPoint
  :public Point
{
public:
  MidPoint() :p1(0), p2(0) {};
  ~MidPoint(){};

  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "MidPoint"; };

  QString wantArg(const Object* o) const;
  bool selectArg( Object* );
  void unselectArg (Object*);
  Objects getParents() const { Objects tmp; tmp.push(p1); tmp.push(p2); return tmp; };

  void startMove(const QPoint&);
  void moveTo(const QPoint&);
  void stopMove();
  void cancelMove();
  void calc();
protected:
  enum { howmMoving, howmFollowing } howm; // how are we moving
  Point* p1;
  Point* p2;
};

class Curve;

// this is a point which is constrained to a Curve, which means it's
// always on the curve, moving it doesn't cause it to move off it.
// ( this is very related to locuses, check locus.h and locus.cpp for
// more info...)
// it still needs lots of work...
/*
class ConstrainedPoint
  : public Point
{
public:
  ConstrainedPoint() : p(0.5), c(0) {};
  ~ConstrainedPoint();

  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "ConstrainedPoint"; };

  QString wantArg(const Object* o) const;
  bool selectArg( Object* );
  void unselectArg (Object*);
  Objects getParents() const { Objects tmp; tmp.push(p1); tmp.push(p2); return tmp; };

  void startMove(const QPoint&);
  void moveTo(const QPoint&);
  void stopMove();
  void cancelMove();
  void calc();
protected:
  double p;
  Curve* c;
};

*/
#endif // POINT_H
