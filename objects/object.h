/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/


#ifndef OBJECT_H
#define OBJECT_H

//#include <list>
#include <algorithm>
#include <map>
#include <vector>

#include <qcstring.h>
#include <qdom.h>

#include <klocale.h>

#include "../misc/objects.h"
#include "../misc/common.h"
#include "../misc/kigpainter.h"


class Point;
class Segment;
class Line;
class Circle;
class Curve;
class ConstrainedPoint;

// base class representing all objects (e.g. points, lines etc.)
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
  // Objects that reimplement this should call Object::getParams() and
  // setParams() after you've handled your params.. --> to set params
  // common to all objects like the color...
  virtual std::map<QCString,QString> getParams ();
  virtual void setParams ( const std::map<QCString,QString>& );

  // getting types from this object: easier to type and supports
  // MacroObjectOne
  static Point* toPoint(Object* o);
  static Segment* toSegment(Object* o);
  static Line* toLine(Object* o);
  static Circle* toCircle(Object* o);
  static Curve* toCurve(Object* o);
  static ConstrainedPoint* toConstrainedPoint(Object* o);

  static const Point* toPoint(const Object* o);
  static const Segment* toSegment(const Object* o);
  static const Line* toLine(const Object* o);
  static const Circle* toCircle(const Object* o);
  static const Curve* toCurve(const Object* o);
  static const ConstrainedPoint* toConstrainedPoint(const Object* o);

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
  // @p showSelect: whether selection should be shown (we don't want to
  // print selected stuff as selected...
  void drawWrap(KigPainter& p, bool ss) const { if (!shown) return; draw(p,ss); };
  virtual void draw (KigPainter& p, bool showSelection) const = 0;

  // whether the object contains o
  // allowed_miss contains the maximum distance there may be between
  // o and your object...
  virtual bool contains ( const Coordinate& o, const double allowed_miss ) const = 0;

  // are you in this rect ?
  virtual bool inRect (const Rect&) const = 0;

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
  virtual void drawPrelim (KigPainter& p, const Coordinate& pt ) const = 0;

  // for moving
  // sos contains the objects that are being moved.
  // the point is where the cursor is,  check the point implementation for an
  // example
  virtual void startMove(const Coordinate&) = 0;
  virtual void moveTo(const Coordinate&) = 0;
  virtual void stopMove() = 0;

  // informs the object that it ( or one of its parents ) has been
  // moved (or other situations), and that it should recalculate any
  // of its variables
  virtual void calc() = 0;
protected:
  // the color --> note that the selected color can't be changed ( i'm
  // thinking about whether this is necessary...
  QColor mColor;
public:
  QColor color() const { return mColor; };
  void setColor( const QColor c ) { mColor = c; };

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
  // an object is valid if it's in a state where its position and such
  // is defined: example: the intersection of a line and a circle:
  // this can be defined and undefined ( unless you start playing with
  // complex numbers, which is useless to us... )
  // objects are unvalid if
  // 1 its parents are unvalid..
  // 2 the object itself is unvalid due to its definition...
  // you should check for these cases in your calc() routine...
  bool valid;
 public:
  bool getValid() const { return valid; };
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
};

#endif // OBJECT_H
