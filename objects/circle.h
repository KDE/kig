#ifndef CIRCLE_H
#define CIRCLE_H

#include "curve.h"

class Circle
: public Curve
{
 public:
  Circle();
  ~Circle();

  // type identification
  virtual QCString vBaseTypeName() const { return sBaseTypeName();};
  static QCString sBaseTypeName() { return I18N_NOOP("circle"); };

  bool contains (const QPoint& o, bool strict) const;
  void draw (QPainter& p, bool showSelection) const;
   bool inRect (const QRect&) const;
  Point getPoint (double param) const;
  double getParam (const Point&) const;
  
  static inline void drawCircle (QPainter& p, const Point& pt, const double radius)
  {
    QRect rect (qRound(pt.getX()-radius), qRound(pt.getY()-radius), qRound(2*radius), qRound(2*radius));
    p.drawEllipse(rect);
  };

  void getOverlay (QPtrList<QRect>& list, const QRect& border) const { circleGetOverlay(qpc, radius, list, border); };
  static void circleGetOverlay(const Point& centre, double radius, QPtrList<QRect>& list, const QRect& border)
  {
    QRect rect(centre.getX()-radius-1, centre.getY()-radius-1, 2*radius+3, 2*radius+3);
    circleGetOverlay (centre , radius , list, border, rect);
  };
  static void circleGetOverlay(const Point& centre, double radius, QPtrList<QRect>& list, const QRect& border, const QRect& currentRect);
  static inline double calcRadius(const Point& centre, const Point& point)
  { return qRound((centre-point).length()); };

protected:
  Point qpc;
  double radius;
};

// a circle composed by a centre and a point on the circle
class CircleBCP
  : public Circle
{
public:
  CircleBCP();
  ~CircleBCP(){};
  CircleBCP(const CircleBCP& c);
  CircleBCP* copy() { return new CircleBCP(*this); };

  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "CircleBCP"; };

  void calc ();
  void drawPrelim ( QPainter&, const QPoint& ) const;
  void getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& pt) const;

  // passing arguments
  QString wantArg (const Object*) const;
  bool selectArg (Object*);
  void unselectArg (Object*);
    Objects getParents() const;

  //moving
  void startMove(const QPoint&);
  void moveTo(const QPoint&);
  void stopMove();
  void cancelMove();
protected:
  Point* poc; // point on circle
  Point* centre;
  QPoint pwpsm; // point where poc started moving
  enum { notMoving, movingPoc, lettingPocMove } wawm; // what are we moving ?
};

class CircleBTP
  : public Circle
{
public:
  CircleBTP() :p1(0), p2(0), p3(0) {};
  ~CircleBTP() {};
  CircleBTP(const CircleBTP& c);
  CircleBTP* copy() { return new CircleBTP(*this); };

  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "CircleBTP"; };

  void drawPrelim ( QPainter&, const QPoint& ) const;
  void getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& pt) const;
  // passing arguments
  QString wantArg (const Object*) const;
  bool selectArg (Object*);
  void unselectArg (Object*);
    Objects getParents() const;

  //moving
  void startMove(const QPoint&) {};
  void moveTo(const QPoint&) {};
  void stopMove() {};
  void cancelMove() {};

protected:
  Point* p1;
  Point* p2;
  Point* p3;

  void calc();
  Point calcCenter(double, double, double, double, double, double) const;
};

#endif
