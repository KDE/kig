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


#ifndef POINT_H
#define POINT_H

#include <qpoint.h>
#include <qrect.h>
#include <qcolor.h>

#include <cmath>

#include "object.h"
#include "../misc/coordinate.h"

class Point
: public Object
{
 public:
  Point() {};
protected:
  Point( const Coordinate& p ) : mC( p ) {};
public:

  // type info:
  Point* toPoint() { return this; };
  const Point* toPoint() const { return this; };

  // type identification
  virtual const QCString vBaseTypeName() const { return sBaseTypeName();};
  static const QCString sBaseTypeName() { return I18N_NOOP("point"); };

  // general members
  virtual bool contains (const Coordinate& o, const double fault ) const;
  virtual void draw (KigPainter& p,bool showSelection = true) const;
  virtual bool inRect(const Rect& r) const { return r.contains( mC ); };
  // looks
  QColor getColor() { return Qt::black; };
  void setColor(const QColor&) {};

  const Coordinate& getCoord() const { return mC; };
public:
  double getX() const { return mC.x;};
  double getY() const { return mC.y;};
  void setX (const double inX) { mC.x = inX; };
  void setY (const double inY) { mC.y = inY; };

protected:
  Coordinate mC;
};

class FixedPoint
  : public Point
{
public:
  FixedPoint() {};
  FixedPoint( const FixedPoint& p );
  FixedPoint( const double x, const double y ) : Point( Coordinate( x, y ) ) {};
  FixedPoint( const Coordinate& p ) : Point( p ) {};

  FixedPoint* copy() { return new FixedPoint( *this ); };

  std::map<QCString,QString> getParams();
  void setParams( const std::map<QCString,QString>& m);
  virtual const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "FixedPoint"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName() { return i18n("Fixed Point"); };
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription() { return i18n( "" ); };
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "point4"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };

  // no drawPrelim...
  virtual void drawPrelim( KigPainter &, const Coordinate& ) const {};

  // passing arguments
  virtual QString wantArg(const Object*) const { return 0; };
  virtual QString wantPoint() const { return 0; };
  virtual bool selectArg( Object *) { return true; }; // no args
  virtual void unselectArg( Object *) {}; // no args

  // no args => no parents
  virtual Objects getParents() const { return Objects();};

  //moving
  virtual void startMove(const Coordinate&);
  virtual void moveTo(const Coordinate&);
  virtual void stopMove();

  virtual void calc(){};

  static KAction* sConstructAction( KigDocument*, Type*, int );

protected:
  // point where we last moved to...
  Coordinate pwwlmt;
};

// midpoint of two other points
class MidPoint
  :public Point
{
public:
  MidPoint() :p1(0), p2(0) {};
  ~MidPoint(){};
  MidPoint(const MidPoint& m);

  MidPoint* copy() { return new MidPoint(*this); };

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "MidPoint"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName() { return i18n("Midpoint"); };
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription()  { return i18n( "The midpoint of a segment or of two other points" ); };
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName()  { return "bisection"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };

  QString wantArg(const Object* o) const;
  QString wantPoint() const;
  bool selectArg( Object* );
  void unselectArg (Object*);
  Objects getParents() const { Objects tmp; tmp.push_back(p1); tmp.push_back(p2); return tmp; };

  void startMove(const Coordinate&);
  void moveTo(const Coordinate&);
  void stopMove();
  void cancelMove();
  void calc();
  void drawPrelim( KigPainter&, const Coordinate& ) const {};
protected:
  enum { howmMoving, howmFollowing } howm; // how are we moving
  Point* p1;
  Point* p2;
};

class Curve;

// this is a point which is constrained to a Curve, which means it's
// always on the curve, moving it doesn't cause it to move off it.
// ( this is very related to locuses, check locus.h and locus.cpp for
// more info...)
// it still needs lots of work...
class ConstrainedPoint
  : public Point
{
public:
  ConstrainedPoint(Curve* inC, const Coordinate& inPt);
  ConstrainedPoint(const double inP);
  ConstrainedPoint();
  ~ConstrainedPoint() {};
  ConstrainedPoint( const ConstrainedPoint& c);

  ConstrainedPoint* copy() { return new ConstrainedPoint(*this); };

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "ConstrainedPoint"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName() { return i18n("Constrained Point"); };
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription() { return i18n( "" ); };
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "point4"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };

  QString wantArg(const Object*) const;
  QString wantPoint() const { return 0; };
  bool selectArg( Object* );
  void unselectArg (Object*) {};
  Objects getParents() const;

  void startMove(const Coordinate&) {};
  void moveTo(const Coordinate& pt);
  void stopMove() {};
  void cancelMove() {};
  void calc();

  void drawPrelim( KigPainter&, const Coordinate& ) const {};

  double getP() { return p; };
  void setP(double inP) { p = inP; };

  std::map<QCString,QString> getParams();
  void setParams( const std::map<QCString,QString>& m);

  static KAction* sConstructAction( KigDocument*, Type*, int );
protected:
  double p;
  Curve* c;
};

#endif // POINT_H
