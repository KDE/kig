#ifndef MACRO_H
#define MACRO_H

#include "object.h"

class ObjectHierarchy;

class MacroObject
  : public Object
{
public:
  QString wantArg( const Object* o) const;
  MacroObject(ObjectHierarchy* inHier);
  virtual ~MacroObject() {};
  bool selectArg(Object* o);
//   void draw (QPainter& p, bool ss) const = 0;
//   bool contains (const QPoint& p, bool strict = false) const = 0;
//   bool inRect(const QRect& r) const = 0;
//   QRect getSpan() const = 0;
//   void drawPrelim (QPainter&, const QPoint& ) const = 0;
//   void startMove(const QPoint& p) = 0;
//   void moveTo (const QPoint& p) = 0;
//   void stopMove() = 0;
//   void moved() = 0;
//   QCString vBaseTypeName() const = 0;
//   QCString vFullTypeName() const = 0;
//   Objects getParents() const = 0;
protected:
  virtual void handleNewObjects (const Objects& ) = 0;
protected:
  ObjectHierarchy* hier;
  Objects arguments;
};

class MacroObjectOne
  : public MacroObject
{
  // the objects we contain, these are not shown...
  Objects cos;
  Object* final;
public:
  const Object* getFinal() const { return final; };
  Object* getFinal() { return final; };
  MacroObjectOne( ObjectHierarchy* inHier);
  ~MacroObjectOne();
  void draw (QPainter& p, bool ss) const;
  bool contains (const QPoint& p, bool strict = false) const;
  bool inRect(const QRect& r) const;
  QRect getSpan() const;
  void drawPrelim (QPainter&, const QPoint& ) const;
  void getPrelimOverlay(QPtrList<QRect>&, const QRect&, const QPoint&) const {};

  void startMove(const QPoint& p);
  void moveTo (const QPoint& p);
  void stopMove();
  void calc();
  QCString vBaseTypeName() const;
  QCString vFullTypeName() const;
  Objects getParents() const { return arguments; };
  void getOverlay(QPtrList<QRect>& list, const QRect& border) const { final->getOverlay(list,border); };
protected:
  void handleNewObjects (const Objects& o);
  // have we constructed stuff yet?
  bool constructed;
};

// class MacroObjectMulti
//   : public MacroObject, public CollectionObject
// {
// public:
//   MacroObjectMulti(ObjectHierarchy* inHier) : MacroObject (inHier) {};
//   QString wantArg( const Object* o) const;
//   bool selectArg(Object* o);
//   virtual void handleNewObjects (const Objects& o);
// };

#endif
