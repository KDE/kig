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
  ~CollectionObject() { for (Object* i = cos.first(); i; i = cos.next()) delete i; };
  
  // todo: do something more intelligent with this (if it is
  // necessary, not sure about that yet...
  virtual QString vType() const { return sType(); };
  static QString sType() { return i18n("CollectionObject"); };

  virtual QCString vTypeName() const { return sTypeName(); };
  static QCString sTypeName() { return "CollectionObject"; };

  void draw (KigPainter& p, bool showSelection) const;
  bool contains(const Coordinate& o, bool strict = false) const;
  bool inRect(const Rect& r) const;
  QString wantArg(const Object*) const = 0;
  bool selectArg(Object*) = 0;
  void drawPrelim(KigPainter&, const Coordinate&) const = 0;
  void startMove(const Coordinate& p) = 0;
  void moveTo(const Coordinate& p) = 0;
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
