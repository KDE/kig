#ifndef OBJECT_H
#define OBJECT_H

#include <list>
#include <algorithm>
#include <map>

#include <qcstring.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qrect.h>
#include <qdom.h>

#include <klocale.h>

#include "../misc/objects.h"

class Point;
class Segment;
class Line;
class Circle;
class Curve;
class ConstrainedPoint;

// base class representing all objects (e.g. points, lines ..., and some day also user defined macro's)
class Object
{
 public:
  Object();
  virtual ~Object() {};
  // returns a copy of the object.  This should prolly simply do
  // "return new <Type>(*this);" (if you have a good copy constructor...)
  virtual Object* copy() = 0;

  // only object types that have "parameters" need this, a parameter
  // is something which you cannot calculate from your parents,
  // e.g. an independent point's params are its coordinates, a
  // ConstrainedPoint's also got a param..
  // The object should do e->addParam() for every one of its
  // parameters
  virtual map<QCString,double> getParams () {return map<QCString, double>();};
  virtual void setParams ( const map<QCString,double>& ) {};

  // getting types from this object: easier to type and supports
  // MacroObjectOne
  static Point* toPoint(Object* o);
  static Segment* toSegment(Object* o);
  static Line* toLine(Object* o);
  static Circle* toCircle(Object* o);
  static Curve* toCurve(Object* o);
  static ConstrainedPoint* toConstrainedPoint(Object* o);

  static const Point* toPoint(const Object* o);
  static  const Segment* toSegment(const Object* o);
  static  const Line* toLine(const Object* o);
  static  const Circle* toCircle(const Object* o);
  static  const Curve* toCurve(const Object* o);
  static  const ConstrainedPoint* toConstrainedPoint(const Object* o);

  // type identification:
  // there are 3x2 functions: you don't need all of them in your
  // object type definition, but you do need a few:
  // 
  // they come in pairs: a static and a virtual, the virtual should
  // always simply call the static.  The static should return
  // interesting stuff. the static func should be there, even though
  // it's not in this prototype, because we use template classes here
  // and there, and they expect the static funcs to be present
  // 
  // the first two return the base type:  this should be something
  // like "point" for MidPoint, notice the minuscule.  Don't return
  // stuff like "midpoint". don't forget to put your name between
  // "I18N_NOOP()", so it will be marked for translation
  virtual QCString vBaseTypeName() const = 0;
  // static QCString sBaseTypeName();

  // this one is also used, it has no static counterpart, and you
  // shouldn't worry about this one
  QString vTBaseTypeName() const { return i18n(vBaseTypeName()); };
  
  // this typename should be unique, e.g. for loading and saving. your
  // class also needs both of these, here's where you return something
  // like MidPoint, you don't need I18N_NOOP
  virtual QCString vFullTypeName() const = 0;
  // static QCString sFullTypeName();

  // drawing etc.
  // showSelect: whether selection should be shown (we don't want to
  // print selected stuff as selected...
  void drawWrap(QPainter& p, bool ss) const { if (!shown) return; draw(p,ss); };
  virtual void draw (QPainter& p, bool showSelection) const = 0;

  // whether the object contains o, if(!strict), you should be less pedantic,
  // and, so for example a point can be 3 pixels from another before this
  // returns false
  virtual bool contains ( const QPoint& o, bool strict = false ) const = 0;

  // are you in this rect ?
  virtual bool inRect (const QRect&) const = 0;

  // for passing args to objects
  // an object is constructed by creating it, putting it in document->obc,
  // and next passing it arguments till it says it's finished.
  // return a string that you want the ui to show, return what you would use
  // the object for,  if it were selected.
  virtual QString wantArg ( const Object* ) const = 0;
  // return true if you're finished
  // if which was already selected,  unselect it
  // here, you should only store the pointer, not do any calculations,
  // those are to be done in calc()
  virtual bool selectArg (Object* which) = 0;
  // draw a preliminary version of yourself, even though you haven't got all
  // your ars yet,  the cursor is currently at pt
  virtual void drawPrelim (QPainter& p, const QPoint& pt ) const = 0;

  // for moving
  // sos contains the objects that are being moved.
  // the point is where the cursor is,  check the point implementation for an
  // example
  virtual void startMove(const QPoint&) = 0;
  virtual void moveTo(const QPoint&) = 0;
  virtual void stopMove() = 0;

  // informs the object that it ( or one of its parents ) has been
  // moved (or other situations), and that it should recalculate any
  // of its variables
  virtual void calc() = 0;
 protected:
  // are we selected?
  bool selected;
 public:
  bool getSelected() const { return selected; };
  virtual void setSelected(bool in);
 protected:
  // are we shown?
  bool shown;
 public:
  bool getShown() const { return shown; };
  void setShown(bool inShown) { shown = inShown; };
 protected:
  bool complete;
 public:
  bool getComplete() const { return complete; };
 protected:
  // objects we know, and that know us: if they move, we move too, and vice versa
  Objects children;
 public:
  const Objects& getChildren() const { return children;};
  // our children + our children's children + ...
  Objects getAllChildren() const;

  void addChild(Object* o) { children.add(o); };
  void delChild(Object* o) { children.remove(o); };
  // returns all objects the object depends upon ( the args it selected )
  virtual Objects getParents() const = 0;
  // this is for drawing speed: the object should append a collection
  // of QRects (constructed using new), which contain the entire
  // object.  Obviously: the less QRects, the faster, you are advised
  // to return rects of about overlayRectWidth() wide.
  // border is the border of the visible area, you should not return rects outside of the border
  void getOverlayWrap(QPtrList<QRect>& list, const QRect& border) const { if (!shown) return; getOverlay(list,border); };
  virtual void getOverlay(QPtrList<QRect>& list, const QRect& border) const = 0;
  virtual void getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& pt) const = 0;
  static inline int overlayRectSize() { return 24; };
};

#endif // OBJECT_H
