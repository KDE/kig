// normalpoint.h
// Copyright (C)  2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef NORMALPOINT_H
#define NORMALPOINT_H

#include "point.h"

/**
 * NormalPoint is a point that is either Independent ( you move it
 * wherever you want ), or Constrained ( it can move anywhere on a
 * Curve ).  To allow for dynamic changing of its behaviour, we have a
 * Bridge pattern here... ( read the Design Patterns book on
 * object-oriented design patterns !! )
 */
class NormalPointImp;
class FixedPointImp;
class ConstrainedPointImp;

class NormalPoint
  : public Point
{
  NormalPointImp* mimp;
  void setImp( NormalPointImp* i );

  // tparents is used to hold the parents between the construction of
  // the Object, and setParams() or setImp() where our mimp is
  // defined...
  Objects tparents;
public:
  NormalPoint( NormalPointImp* );
  // this is for allowing us to be constructed by the native file
  // format filter... it sets mimp to 0 and waits for setParams() to
  // set it to something useful...
  NormalPoint( const Objects& os );
  NormalPoint( const NormalPoint& );

  ~NormalPoint();

  NormalPoint* copy();

  // we provide these functions so clients don't have to mess with
  // constructing Imp's...  (which is made impossible btw, by making
  // imp constructors private...)
  static NormalPoint* fixedPoint( const Coordinate& c );
  static NormalPoint* constrainedPoint( Curve* c, const Coordinate& c );
  // sensiblePoint() returns a new NormalPoint with an imp selected
  // with the same method as in redefine()...
  static NormalPoint* sensiblePoint( const Coordinate& c, const KigDocument&, double fault );

  virtual NormalPoint* toNormalPoint();
  virtual const NormalPoint* toNormalPoint() const;

  FixedPointImp* fixedImp();
  const FixedPointImp* fixedImp() const;
  ConstrainedPointImp* constrainedImp();
  const ConstrainedPointImp* constrainedImp() const;

  // NormalPointImp needs an interface to us..
  void setCoord( const Coordinate& c );

  // this finds a suitable mimp for Coordinate c... for now it only
  // checks if c is on a Curve, in which case it returns a
  // ConstrainedImp.., and otherwise returns a FixedImp...
  // fault is the argument we pass to KigDocument::whatAmIOn()...
  // you should still call calc() after this !
  void redefine( const Coordinate& c, const KigDocument&, double fault );

  std::map<QCString, QString> getParams();
  void setParams( const std::map<QCString, QString>& m );

  virtual const QCString vFullTypeName() const;
  static const QCString sFullTypeName();
  const QString vDescriptiveName() const;
  static const QString sDescriptiveName();
  const QString vDescription() const;
  static const QString sDescription();
  const QCString vIconFileName() const;
  static const QCString sIconFileName();
  const int vShortCut() const;
  static const int sShortCut();
  static const char* sActionName();

  static KigMode* sConstructMode( Type*, KigDocument*, NormalMode* );

  const QStringList objectActions() const;
  void doAction( int which, KigDocument* d, KigView* v, NormalMode* m );

  virtual Objects getParents() const;
  virtual void calc( const ScreenInfo& s );

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

  virtual FixedPointImp* toFixed();
  virtual ConstrainedPointImp* toConstrained();
  virtual const FixedPointImp* toFixed() const;
  virtual const ConstrainedPointImp* toConstrained() const;

  virtual void startMove( const Coordinate& c, NormalPoint* p ) = 0;
  virtual void moveTo( const Coordinate& c, NormalPoint* p ) = 0;
  virtual void stopMove( NormalPoint* p ) = 0;
  virtual void calc( NormalPoint* p, const ScreenInfo& s ) = 0;

  virtual QString type() = 0;
  virtual void writeParams( std::map<QCString, QString>& m, NormalPoint* p ) = 0;
  virtual void readParams( const std::map<QCString, QString>& m, NormalPoint* p ) = 0;

  virtual void setParents( const Objects& os, NormalPoint* p ) = 0;
  virtual void unselectArgs( NormalPoint* ) = 0;

  virtual Objects getParents() = 0;
};

class FixedPointImp
  : public NormalPointImp
{
  // point where we last moved to...
  Coordinate pwwlmt;
  // point where we currently are...
  Coordinate pwwca;

  FixedPointImp( const Coordinate& c = Coordinate( 0, 0 ) );
  FixedPointImp( const FixedPointImp& p, NormalPoint* p );

public:
  friend class NormalPoint;

  virtual NormalPointImp* copy( NormalPoint* parent );

  FixedPointImp* toFixed();
  const FixedPointImp* toFixed() const;

  void setCoord( const Coordinate& c );
  const Coordinate getCoord();

  virtual void calc( NormalPoint* p, const ScreenInfo& r );
  virtual void startMove( const Coordinate& c, NormalPoint* p );
  virtual void moveTo( const Coordinate& c, NormalPoint* p );
  virtual void stopMove( NormalPoint* p);

  virtual QString type();
  static QString sType();
  virtual void writeParams( std::map<QCString, QString>& m, NormalPoint* p );
  virtual void readParams( const std::map<QCString, QString>& m, NormalPoint* p );

  virtual Objects getParents();

  virtual void setParents( const Objects& os, NormalPoint* p );
  virtual void unselectArgs( NormalPoint* np );
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

  ConstrainedPointImp( const Coordinate& d, Curve* c );
  ConstrainedPointImp();
  ConstrainedPointImp( const ConstrainedPointImp& p, NormalPoint* np );

public:
  friend class NormalPoint;

  virtual NormalPointImp* copy( NormalPoint* p );

  virtual ConstrainedPointImp* toConstrained();
  virtual const ConstrainedPointImp* toConstrained() const;

  void redefine( Curve*, const Coordinate& c, NormalPoint* );

  void setP( const double p );
  double getP();

  virtual void calc( NormalPoint* p, const ScreenInfo& s );
  virtual void startMove( const Coordinate& c, NormalPoint* );
  virtual void moveTo( const Coordinate& c, NormalPoint* );
  virtual void stopMove( NormalPoint* );

  virtual QString type();
  static QString sType();
  virtual void writeParams( std::map<QCString, QString>& m, NormalPoint* p );
  virtual void readParams( const std::map<QCString, QString>& m, NormalPoint* p );

  virtual void setParents( const Objects& os, NormalPoint* p );
  virtual void unselectArgs( NormalPoint* );

  virtual Objects getParents();
};

#endif
