#ifndef LOCUS_H
#define LOCUS_H

#include "curve.h"

#include <list>

class ConstrainedPoint;
class ObjectHierarchy;

// this object is inspired on KSeg source, credits for all of the
// ideas go to Ilya Baran <ibaran@mit.edu>
// a locus object is a mathematical locus.  It is defined by a
// constrained point, which moves over a curve, and an object,
// which somehow depends on that point.  The locus contains all
// objects the object moves over as the point moves over all of
// its possible positions...
// this is implemented by having a Locus simply contain some 150
// objects (hmm...)
// drawing is done by simply drawing the points...
class Locus
    : public Curve
{
public:
  // number of points to include, i think this is a nice default...
  static const int numberOfSamples = 300;
public:
  Locus() : cp(0), obj(0), hierarchy(0) { };
  ~Locus() { objs.deleteAll(); };
  Locus(const Locus& loc);
  Locus* copy() { return new Locus(*this); };

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

public:
  Point getPoint (double param) const;
  double getParam (const Point&) const;

  Objects getParents() const
  {
    Objects tmp;
    tmp.append(cp);
    tmp.append(obj);
    return tmp;
  };

  void getOverlay(QPtrList<QRect>& list, const QRect& border) const;

protected:
  ConstrainedPoint* cp;
  Object* obj;

  bool isPointLocus() const { return _pointLocus; }
  bool _pointLocus;

  // don't use this for fillUp or saving, since it has 0 for
  // KigDocument pointer...
  ObjectHierarchy* hierarchy;

  // objs is just a list of pointers to objects
  Objects objs;

  struct CPt
  {
    CPt(Point inPt, double inPm) : pt(inPt), pm (inPm) {};
    Point pt;
    double pm;
  };

  typedef list<CPt> CPts;

  // for calcPointLocus we need some special magic, so it is a special
  // type...
  CPts pts;

  // this is used if the obj is a point; it selects the best points
  // from the possible ones...
  void calcPointLocus();
  // some functions used by calcPointLocus...
  CPts::iterator addPoint(double param);
  void recurse(CPts::iterator, CPts::iterator, int&);

  // this is used when the obj is not a point; it just takes the first
  // numberOfSamples objects it can find...
  void calcObjectLocus();

};
#endif
