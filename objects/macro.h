/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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

#ifndef MACRO_H
#define MACRO_H

#include "curve.h"

class ObjectHierarchy;
class ScreenInfo;
class MType;

class MacroObject
  : public Curve
{
public:
  MacroObject( ObjectHierarchy* inHier, const Objects& args );
  MacroObject( const MacroObject& );
  virtual ~MacroObject() {};
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
  // have we constructed stuff yet?
  bool constructed;
  const MType* mtype;
public:
  const Object* getFinal() const { return final; };
  Object* getFinal() { return final; };
  MacroObjectOne( const MType* type, ObjectHierarchy* inHier, const Objects& args );
  MacroObjectOne(const MacroObjectOne& m);
  MacroObjectOne* copy() { return new MacroObjectOne(*this); };
  ~MacroObjectOne();
  void draw (KigPainter& p, bool ss) const;
  bool contains (const Coordinate& p, const ScreenInfo& si ) const;
  bool inRect(const Rect& r) const;
  Rect getSpan() const;

  void startMove(const Coordinate& p, const ScreenInfo&);
  void moveTo (const Coordinate& p);
  void stopMove();
  void calc( const ScreenInfo& );
  const QCString vBaseTypeName() const;
  const QCString vFullTypeName() const;
  const QString vDescriptiveName() const { return 0; };
  const QString vDescription() const { return 0; };
  const QCString vIconFileName() const { return 0; };
  Objects getParents() const { return arguments; };

  Curve* toCurve();
  const Curve* toCurve() const;
  double getParam( const Coordinate& c ) const;
  Coordinate getPoint( double param ) const;
};

// class MacroObjectMulti
//   : public MacroObject, public CollectionObject
// {
// public:
//   MacroObjectMulti(ObjectHierarchy* inHier) : MacroObject (inHier) {};
//   virtual void handleNewObjects (const Objects& o);
// };

#endif
