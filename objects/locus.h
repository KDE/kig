#ifndef LOCUS_H
#define LOCUS_H

#include "curve.h"

#include <list>

class ConstrainedPoint;
class ObjectHierarchy;

// this object is inspired on KSeg source, credits for the ideas go to
// ibaran@mit.edu
// a locus object is a mathematical locus.  It is defined by a
// constrained point, which moves over a curve, and another point,
// which somehow depends on that point.  The locus contains all
// objects the second point moves over as the first moves over all of
// its possible positions...
// this is implemented by having a Locus simply contain some 100
// points (hmm...)
// drawing is done by simply drawing the points...
class Locus
    : public Curve
{
public:
  // number of points to include...
  static const int numberOfPoints = 100;
public:
  Locus() : cp(0), point(0), hierarchy(0) { };
  ~Locus() {};
  virtual QCString vBaseTypeName() const { return sBaseTypeName(); };
  static QCString sBaseTypeName() { return I18N_NOOP("curve"); };
  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "Curve"; };

  void draw (QPainter& p, bool showSelection) const;
  bool contains (const QPoint& o, bool strict = false ) const;
  bool inRect (const QRect&) const;

  // arguments
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
//   void unselectArg (Object* which);
  void drawPrelim ( QPainter&, const QPoint& ) const {};
  void getPrelimOverlay(QPtrList<QRect>&, const QRect&, const QPoint&) const {};

  // moving
  void startMove(const QPoint&) {};
  void moveTo(const QPoint&) {};
  void stopMove() {};

  void calc();

  Point getPoint (double param) const;
  double getParam (const Point&) const;

  Objects getParents() const { Objects tmp; tmp.append(cp); tmp.append(point); return tmp;};

  void getOverlay(QPtrList<QRect>& list, const QRect& border) const;

protected:
  ConstrainedPoint* cp;
  Point* point;

  // don't use this for fillUp or saving, since it has 0 for
  // KigDocument pointer...
  ObjectHierarchy* hierarchy;
  typedef std::list<Point> PointList;
  PointList points;
  static void draw ( QPainter& p, const PointList& pts );
};
#endif
