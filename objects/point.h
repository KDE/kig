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

/**
 * NormalPoint is a point that is either Independent ( you move it
 * wherever you want ), or Constrained ( it can move anywhere on a
 * Curve ).  To allow for dynamic changing of its behaviour, we have a
 * Bridge pattern here... ( read the Design Patterns book on
 * object-oriented design patterns ( especially if you're a Qt
 * programmer :P ) !! )
 */
class NormalPointImp;
class FixedPointImp;
class ConstrainedPointImp;

class NormalPoint
  : public Point
{
  NormalPointImp* mimp;
public:
  NormalPoint( NormalPointImp* );
  // this is for allowing us to be constructed by the native file
  // format filter... it sets mimp to 0 and waits for setParams() to
  // set it to something useful...
  NormalPoint();
  NormalPoint( const NormalPoint& );
  ~NormalPoint();
  NormalPoint* copy();

  virtual NormalPoint* toNormalPoint() { return this; };

  FixedPointImp* fixedImp();
  const FixedPointImp* fixedImp() const;
  ConstrainedPointImp* constrainedImp();
  const ConstrainedPointImp* constrainedImp() const;

  // NormalPointImp needs an interface to us..
  void setCoord( const Coordinate& c );

  // this returns a suitable mimp for Coordinate c... for now it only
  // checks if c is on a Curve, so it returns a ConstrainedImp.., and
  // else returns a FixedImp...
  // fault is passed to KigDocument::whatAmIOn()...
  static NormalPointImp* NPImpForCoord( const Coordinate& c, const KigDocument* d, double fault );

  std::map<QCString, QString> getParams();
  void setParams( const std::map<QCString, QString>& m );

  virtual const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "NormalPoint"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName() { return i18n("Normal Point"); };
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription() { return i18n( "A normal point, that is either independent or attached to a line, circle, segment..." ); };
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "point4"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };

  static KAction* sConstructAction( KigDocument*, Type*, int );

  // no drawPrelim...
  virtual void drawPrelim( KigPainter &, const Coordinate& ) const {};

  // passing arguments
  virtual QString wantArg(const Object*) const;
  virtual QString wantPoint() const { return 0; };
  virtual bool selectArg( Object *);

  // no args => no parents
  virtual Objects getParents() const;
  virtual void calc();

  //moving
  virtual void startMove( const Coordinate& c );
  virtual void moveTo( const Coordinate& c );
  virtual void stopMove();
};

class FixedPointImp;
class ConstrainedPointImp;

class NormalPointImp
{
 protected:
  NormalPointImp();
 public:
  virtual ~NormalPointImp();

  virtual NormalPointImp* copy( NormalPoint* newParent ) = 0;

  virtual FixedPointImp* toFixed() { return 0; };
  virtual ConstrainedPointImp* toConstrained() { return 0; };
  virtual const FixedPointImp* toFixed() const { return 0; };
  virtual const ConstrainedPointImp* toConstrained() const { return 0; };

  virtual void startMove( const Coordinate& c, NormalPoint* p ) = 0;
  virtual void moveTo( const Coordinate& c, NormalPoint* p ) = 0;
  virtual void stopMove( NormalPoint* p ) = 0;
  virtual void calc( NormalPoint* p ) = 0;

  virtual QString type() = 0;
  virtual void writeParams( std::map<QCString, QString>& m, NormalPoint* p ) = 0;
  virtual void readParams( const std::map<QCString, QString>& m, NormalPoint* p ) = 0;

  virtual QString wantArg( const Object* ) const = 0;
  virtual bool selectArg( Object *, NormalPoint* ) = 0;

  virtual Objects getParents() = 0;
};

class FixedPointImp
  : public NormalPointImp
{
  // point where we last moved to...
  Coordinate pwwlmt;
  // point where we currently are...
  Coordinate pwwca;
public:
  FixedPointImp( const Coordinate& c = Coordinate( 0, 0 ) );
  FixedPointImp( const FixedPointImp& p, NormalPoint* p );

  virtual NormalPointImp* copy( NormalPoint* parent );

  FixedPointImp* toFixed() { return this; };
  const FixedPointImp* toFixed() const { return this; };

  virtual void calc( NormalPoint* p );
  virtual void startMove( const Coordinate& c, NormalPoint* p );
  virtual void moveTo( const Coordinate& c, NormalPoint* p );
  virtual void stopMove( NormalPoint* p);

  virtual QString type() { return sType(); };
  static QString sType() { return QString::fromUtf8( "Fixed" ); };
  virtual void writeParams( std::map<QCString, QString>& m, NormalPoint* p );
  virtual void readParams( const std::map<QCString, QString>& m, NormalPoint* p );

  virtual QString wantArg(const Object*) const;

  virtual Objects getParents();

  virtual bool selectArg( Object *, NormalPoint* );
};

// this is the imp for a point which is constrained to a Curve, which
// means it's always on the curve, moving it doesn't cause it to move
// off it. ( this is very related to locuses, check locus.h and
// locus.cpp for more info...)
class ConstrainedPointImp
  : public NormalPointImp
{
  double mparam;
  Curve* mcurve;
public:
  ConstrainedPointImp( const Coordinate& d, Curve* c );
  ConstrainedPointImp();
  ConstrainedPointImp( const ConstrainedPointImp& p, NormalPoint* d );

  virtual NormalPointImp* copy( NormalPoint* p );

  virtual ConstrainedPointImp* toConstrained() { return this; };
  virtual const ConstrainedPointImp* toConstrained() const { return this; };

  void setP( const double p ) { mparam = p; };
  double getP() { return mparam; };

  virtual void calc( NormalPoint* p );
  virtual void startMove( const Coordinate& c, NormalPoint* );
  virtual void moveTo( const Coordinate& c, NormalPoint* );
  virtual void stopMove( NormalPoint* );

  virtual QString type() { return sType(); };
  static QString sType();
  virtual void writeParams( std::map<QCString, QString>& m, NormalPoint* p );
  virtual void readParams( const std::map<QCString, QString>& m, NormalPoint* p );

  virtual QString wantArg(const Object*) const;
  virtual bool selectArg( Object *, NormalPoint* );

  virtual Objects getParents();
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

#endif // POINT_H
