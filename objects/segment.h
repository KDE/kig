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
  bool contains (const Coordinate& o, const double fault ) const;
  void draw ( KigPainter& p, bool showSelection ) const;
  void drawPrelim ( KigPainter&, const Coordinate& ) const;

  bool inRect (const Rect&) const;

  // arguments
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
  void unselectArg (Object* which);
    Objects getParents() const;

  // moving
  void startMove(const Coordinate&);
  void moveTo(const Coordinate&);
  void stopMove();
  //     void cancelMove();

  void calc();
  void deleted( Object* o );

  Coordinate getPoint (double param) const;
  double getParam (const Coordinate&) const;

  const Coordinate& getP1() { return p1->getCoord();};
  const Coordinate& getP2() { return p2->getCoord();};
  Point* getPoint1() { return p1; };
  Point* getPoint2() { return p2; };
protected:
  Point* p1, *p2;
  Coordinate pwwsm; // point where we started moving
};

#endif
