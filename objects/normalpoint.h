// normalpoint.h
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

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
public:
  NormalPoint( NormalPointImp* );
  // this is for allowing us to be constructed by the native file
  // format filter... it sets mimp to 0 and waits for setParams() to
  // set it to something useful...
  NormalPoint();
  NormalPoint( const NormalPoint& );
  ~NormalPoint();
  NormalPoint* copy();

  NormalPointImp* imp() { return mimp; };
  const NormalPointImp* imp() const { return mimp; };
  void setImp( NormalPointImp* imp );
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
  static NormalPointImp* NPImpForCoord( const Coordinate& c,
                                        const KigDocument*, double fault,
                                        NormalPointImp* previous = 0 );

  std::map<QCString, QString> getParams();
  void setParams( const std::map<QCString, QString>& m );

  virtual const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "NormalPoint"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "point4"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };

  static KAction* sConstructAction( KigDocument*, Type*, int );

  // no drawPrelim...
  virtual void drawPrelim( KigPainter &, const Object* ) const {};

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

  void setCoord( const Coordinate& c );
  const Coordinate getCoord();

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
  ConstrainedPointImp( const ConstrainedPointImp& p );

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

#endif
