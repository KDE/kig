#ifndef segment_h
#define segment_h

#include "curve.h"

class Segment
  : public Curve
{
public:
  Segment();
  Segment(const Segment& s);
  Segment* copy() { return new Segment(*this); };
  ~Segment();

  // type identification
  virtual QCString vBaseTypeName() const { return sBaseTypeName();};
  static QCString sBaseTypeName() { return I18N_NOOP("segment"); };
  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return I18N_NOOP("Segment"); };

  QCString iType() const { return I18N_NOOP("segment"); };
  bool contains (const QPoint& o, bool strict) const;
  void draw (QPainter& p, bool showSelection) const;
  void drawPrelim ( QPainter&, const QPoint& ) const;
  void getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& pt) const;

  bool inRect (const QRect&) const;

  // arguments
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
  void unselectArg (Object* which);
    Objects getParents() const;

  // moving
  void startMove(const QPoint&);
  void moveTo(const QPoint&);
  void stopMove();
  //     void cancelMove();

  void calc();
  void deleted( Object* o );

  Point getPoint (double param) const;
  double getParam (const Point&) const;

  const Point& getP1() { return *p1;};
  const Point& getP2() { return *p2;};
  Point* getPoint1() { return p1; };
  Point* getPoint2() { return p2; };
protected:
  Point* p1, *p2;
  QPoint pwwsm; // point where we started moving
  // TODO: remove
  double x1y2_y1x2; // =x1y2-y1x2,  so we wouldn't have to calc that every time
  double y1_y2, x2_x1; // ... , dito
  void calcVars(); // calculate the vars from the points
public:
  void getOverlay(QPtrList<QRect>& list, const QRect& border) const { if (p1 && p2) segmentOverlay(*p1, *p2, list, border); };
  static void segmentOverlay(const Point& p1, const Point& p2, QPtrList<QRect>& list, const QRect& border);
};

#endif
