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
//   void draw (KigPainter& p, bool ss) const = 0;
//   bool contains (const Coordinate& p, bool strict = false) const = 0;
//   bool inRect(const Rect& r) const = 0;
//   Rect getSpan() const = 0;
//   void drawPrelim (KigPainter&, const Coordinate& ) const = 0;
//   void startMove(const Coordinate& p) = 0;
//   void moveTo (const Coordinate& p) = 0;
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
  MacroObjectOne(const MacroObjectOne& m);
  MacroObjectOne* copy() { return new MacroObjectOne(*this); };
  ~MacroObjectOne();
  void draw (KigPainter& p, bool ss) const;
  bool contains (const Coordinate& p, const double fault ) const;
  bool inRect(const Rect& r) const;
  Rect getSpan() const;
  void drawPrelim (KigPainter&, const Coordinate& ) const;

  void startMove(const Coordinate& p);
  void moveTo (const Coordinate& p);
  void stopMove();
  void calc();
  QCString vBaseTypeName() const;
  QCString vFullTypeName() const;
  Objects getParents() const { return arguments; };
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
