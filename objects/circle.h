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


#ifndef CIRCLE_H
#define CIRCLE_H

#include "curve.h"
#include "../misc/common.h"

class Circle
: public Curve
{
 public:
  Circle();
  Circle( const Circle& c );
  ~Circle();

  // type identification
  virtual const QCString vBaseTypeName() const { return sBaseTypeName();};
  static const QCString sBaseTypeName();

  bool contains (const Coordinate& o, const double miss ) const;
  void draw (KigPainter& p, bool showSelection) const;
  bool inRect (const Rect&) const;
  Coordinate getPoint (double param) const;
  double getParam (const Coordinate&) const;

  Coordinate getCenter() const { return qpc; };
  double getRadius() const { return radius; };

protected:
  Coordinate qpc;
  double radius;

  static double calcRadius( const Point* c, const Point* p );
  static double calcRadius( const Coordinate& c, const Coordinate& p );
};

// a circle composed by a centre and a point on the circle
class CircleBCP
  : public Circle
{
public:
  CircleBCP( const Objects& os );
  ~CircleBCP(){};
  CircleBCP(const CircleBCP& c);
  CircleBCP* copy() { return new CircleBCP(*this); };

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "CircleBCP"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "circle"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return CTRL+Key_C; };
  static const char* sActionName();

  void calc( const ScreenInfo& showingRect );

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects&, const Object* o );
  static void sDrawPrelim( KigPainter& p, const Objects& args );

  Objects getParents() const;

  //moving
  void startMove(const Coordinate&);
  void moveTo(const Coordinate&);
  void stopMove();
  void cancelMove();
protected:
  Point* poc; // point on circle
  Point* centre;
  Coordinate pwpsm; // point where poc started moving
  enum { notMoving, movingPoc, lettingPocMove } wawm; // what are we moving ?
};

class CircleBTP
  : public Circle
{
public:
  CircleBTP( const Objects& os );
  ~CircleBTP() {};
  CircleBTP(const CircleBTP& c);
  CircleBTP* copy() { return new CircleBTP(*this); };

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "CircleBTP"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "circle"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

  //moving
  void startMove(const Coordinate&) {};
  void moveTo(const Coordinate&) {};
  void stopMove() {};
  void cancelMove() {};

protected:
  Point* pts[3];

  void calc( const ScreenInfo& showingRect );
  static Coordinate calcCenter( Coordinate, Coordinate, Coordinate );
};

#endif
