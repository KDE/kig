#ifndef LINE_H
#define LINE_H

#include "curve.h"

#include <kdebug.h>

class Line
  : public Curve
{
public:
  Line() {};
  ~Line() {};

  // type identification
  virtual QCString vBaseTypeName() const { return sBaseTypeName();};
  static QCString sBaseTypeName() { return I18N_NOOP("line"); };

  bool contains (const QPoint& o, bool strict) const;
  void draw (QPainter& p, bool showSelection) const;
  bool inRect (const QRect&) const;

  // arguments
  QString wantArg ( const Object* ) const = 0;
  bool selectArg (Object* which) = 0;
//   void unselectArg (Object* which) = 0;

  // moving
  void startMove(const QPoint&) = 0;
  void moveTo(const QPoint&) = 0;
  void stopMove() = 0;
//   void cancelMove() = 0;

  void calc() = 0;

  Point getPoint (double param) const;
  double getParam (const Point&) const;

  const Point& getP1() { return qp1;};
  const Point& getP2() { return qp2;};
  
  void getOverlay(QPtrList<QRect>& list, const QRect& border) const { lineOverlay(qp1, qp2, list, border); };
  static void lineOverlay(const Point& p1, const Point& p2, QPtrList<QRect>& list, const QRect& border);

  static void calcEndPoints(double& xa, double& xb, double& ya, double& yb, const QRect& r);
  static void drawLineTTP (QPainter& p, const Point& p, const Point& q);
protected:
  Point qp1, qp2;
  QPoint pwwsm; // point where we started moving
  double x1y2_y1x2; // =x1*y2-y1*x2,  so we wouldn't have to calc that every time
  void calcVars(); // calculate the previous vars from the points
};

// a line Through Two Points
class LineTTP
  : public Line
{
public:
  LineTTP() : p1(0), p2(0) { };
  ~LineTTP();
  LineTTP(const LineTTP& l);
  LineTTP* copy() { return new LineTTP(*this); };

  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "LineTTP"; };

  // arguments
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
  void unselectArg (Object* which);
  void drawPrelim ( QPainter& , const QPoint& ) const;
  void getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& pt) const;
  Objects getParents() const;

  // moving
  void startMove(const QPoint&);
  void moveTo(const QPoint&);
  void stopMove();
//   void cancelMove();

  void calc();

protected:
  Point* p1, *p2;
  QPoint pwwsm;
};

// a perpendicular line on a line or segment, through a point
class LinePerpend
  : public Line
{
public:
  LinePerpend() : segment(0), line(0), point(0) {};
  ~LinePerpend();
  LinePerpend(const LinePerpend& l);
  LinePerpend* copy() { return new LinePerpend (*this);};

  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return ("LinePerpend"); };

  // arguments
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
    Objects getParents() const;
//   void unselectArg (Object* which);
  void drawPrelim ( QPainter&, const QPoint& ) const;
  void getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& pt) const;

  // moving
  void startMove(const QPoint&);
  void moveTo(const QPoint&);
  void stopMove();
  void cancelMove();

  void calc();

  // p1 and p2 define a line, find a point on the perpend through q..;
  static Point calcPointOnPerpend (const Point& p1, const Point& p2, const Point& q);
protected:
  Segment* segment;
  Line* line;
  Point* point;
};

// a line parallel to a line or segment, through a point...
class LineParallel
  : public Line
{
public:
  LineParallel() : segment(0), line(0), point(0) {};
  ~LineParallel() {};
  LineParallel (const LineParallel& l);
  LineParallel* copy() { return new LineParallel (*this); };

  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "LineParallel"; };

  // arguments
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
  Objects getParents() const;
//   void unselectArg (Object* which);
  void drawPrelim ( QPainter&, const QPoint& ) const;
  void getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& pt) const;

  // moving
  void startMove(const QPoint&) {};
  void moveTo(const QPoint&) {};
  void stopMove() {};
  void cancelMove() {};

  void calc();

  // p1 and p2 define a line, find a point on the perpend through q..;
  static Point calcPointOnParallel (const Point& p1, const Point& p2, const Point& q);
protected:
  Segment* segment;
  Line* line;
  Point* point;

};
#endif
