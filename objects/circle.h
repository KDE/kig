#ifndef CIRCLE_H
#define CIRCLE_H

#include "curve.h"
#include "../misc/common.h"

class Circle
: public Curve
{
 public:
  Circle();
  ~Circle();

  // type identification
  virtual QCString vBaseTypeName() const { return sBaseTypeName();};
  static QCString sBaseTypeName() { return I18N_NOOP("circle"); };

  bool contains (const Coordinate& o, const double miss ) const;
  void draw (KigPainter& p, bool showSelection) const;
  bool inRect (const Rect&) const;
  Coordinate getPoint (double param) const;
  double getParam (const Coordinate&) const;

protected:
  Coordinate qpc;
  double radius;
  
  inline double calcRadius( const Point* c, const Point* p ) const
  {
    return (c->getCoord() - p->getCoord()).length();
  };
  inline double calcRadius( const Coordinate& c, const Coordinate& p ) const
  {
    return ( c-p).length();
  };
  inline double calcRadius( const Point* c, const Coordinate& p ) const
  {
    return (c->getCoord() - p).length();
  };
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
  void drawPrelim ( KigPainter&, const Coordinate& ) const;

  // passing arguments
  QString wantArg (const Object*) const;
  bool selectArg (Object*);
  void unselectArg (Object*);
    Objects getParents() const;

  //moving
  void startMove(const Coordinate&);
  void moveTo(const Coordinate&);
  void stopMove();
  void cancelMove();
protected:
  Point* poc; // point on circle
  Point* centre;
  Coordinate pwpsm; // point where poc started moving
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

  void drawPrelim ( KigPainter&, const Coordinate& ) const;

  // passing arguments
  QString wantArg (const Object*) const;
  bool selectArg (Object*);
  void unselectArg (Object*);
    Objects getParents() const;

  //moving
  void startMove(const Coordinate&) {};
  void moveTo(const Coordinate&) {};
  void stopMove() {};
  void cancelMove() {};

protected:
  Point* p1;
  Point* p2;
  Point* p3;

  void calc();
  Coordinate calcCenter(double, double, double, double, double, double) const;
};

#endif
