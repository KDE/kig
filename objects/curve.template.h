#ifndef segment_h
#define segment_h

#include "curve.h"

class Segment
    : public Curve
{
public:
  () {};
    ~() {};
    QCString type() const { return ""; };
    bool contains (const QPoint& o, bool strict) const;
    void draw (QPainter& p, bool selected, bool showSelection);
  bool inRect (const QRect&) const;
    QRect getSpan(void) const;

    // arguments
    QString wantArg ( const Object* ) const;
    bool selectArg (Object* which);
    void unselectArg (Object* which);

    // moving
  void startMove(const QPoint&);
  void moveTo(const QPoint&);
    void stopMove();
    void cancelMove();

    void moved(Object* o);

    Point getPoint (double param) const;
    double getParam (const Point&) const;

protected:
}

#endif
