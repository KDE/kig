#ifndef COLLECTION_H
#define COLLECTION_H

#include "object.h"

class CollectionObject
  : public Object
{
protected:
  Objects cos;
public:
  CollectionObject () {};
  CollectionObject(const Objects& inCos) : cos(inCos) {};
  ~CollectionObject() { for (Objects::iterator i = cos.begin(); i != cos.end(); ++i) delete *i; };
  
  // todo: do something more intelligent with this (if it is
  // necessary, not sure about that yet...
  virtual QString vType() const { return sType(); };
  static QString sType() { return i18n("CollectionObject"); };

  virtual QCString vTypeName() const { return sTypeName(); };
  static QCString sTypeName() { return "CollectionObject"; };

  void draw (QPainter& p, bool showSelection) const;
  bool contains(const QPoint& o, bool strict = false) const;
  bool inRect(const QRect& r) const;
  QString wantArg(const Object*) const = 0;
  bool selectArg(Object*) = 0;
  void drawPrelim(QPainter&, const QPoint&) const = 0;
  void startMove(const QPoint& p) = 0;
  void moveTo(const QPoint& p) = 0;
  void stopMove() = 0;
  void calc() = 0;
  Objects getParents() const = 0;
};

// currently not being used, maybe later...
// Object With Hidden Parents: this objects owns its parents, it's
// mostly here for convenience use in MacroObject, the parents are hidden.
class ObjectWHP
  : public CollectionObject
{
protected:
  // the real object
  Object* real;
public:
  const Object* getReal() const { return real; };
  Object* getReal() { return real; };
  
  
};

#endif
