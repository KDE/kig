/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#ifndef KIG_OBJECT_RADICALLINES_H
#define KIG_OBJECT_RADICALLINES_H

#include "line.h"
#include "point.h"
#include "../misc/common.h"

class MultiConstructibleType;

/**
 * implements a radical line for conics
 */
class LineConicRadical
  : public Line
{
public:
  LineConicRadical( const Objects& os );
  LineConicRadical( const LineConicRadical& l );
  ~LineConicRadical() {};

  virtual const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "LineConicRadical"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "conicsradicalline"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // arguments
  static void sDrawPrelim ( KigPainter& p, const Objects& os );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );
  Objects getParents() const;

  void calc();

  virtual prop_map getParams ();
  virtual void setParams ( const Object::prop_map& );

  static Objects sMultiBuild( const Objects& args );

  static KigMode* sConstructMode( MultiConstructibleType* ourtype,
                                  KigDocument& theDoc );

  virtual void addActions( NormalModePopupObjects& );
  virtual void doNormalAction( int which, KigDocument* d, KigWidget* v, NormalMode* m, const Coordinate& cp );

protected:
  Conic* c1;
  Conic* c2;
  int mwhich;     // which of the two lines of the degenerate conic:
                  // -1 or 1
  int mzeroindex; // which of the three degenerate conic (not yet
                  // used): 1, 2 or 3
};

/*
 * intersection of a line and a conic
 */

class ConicLineIntersectionPoint
  : public Point
{
  Conic* mconic;
  AbstractLine* mline;
  int mwhich;            // which of the two points
public:
  ConicLineIntersectionPoint( const Objects& os );
  ConicLineIntersectionPoint( const ConicLineIntersectionPoint& p );
  ~ConicLineIntersectionPoint();

  const QCString vFullTypeName() const;
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

  static void sDrawPrelim ( KigPainter& p, const Objects& os );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );
  Objects getParents() const;

  void calc();

  virtual prop_map getParams ();
  virtual void setParams ( const Object::prop_map& );

  static Objects sMultiBuild( const Objects& args );

  static KigMode* sConstructMode( MultiConstructibleType* ourtype,
                                  KigDocument& theDoc );
};

#endif
