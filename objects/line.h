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

#ifndef KIG_OBJECTS_LINE_H
#define KIG_OBJECTS_LINE_H

#include "curve.h"
#include "../misc/common.h"

#include <kdebug.h>

class Line
  : public Curve
{
public:
  Line() {};
  ~Line() {};

  // type identification
  virtual const QCString vBaseTypeName() const { return sBaseTypeName();};
  static const QCString sBaseTypeName() { return I18N_NOOP("line"); };

  virtual Line* toLine();
  virtual const Line* toLine() const;

  bool contains (const Coordinate& o, const double fault ) const;
  void draw (KigPainter& p, bool showSelection) const;
  bool inRect (const Rect&) const;

  // arguments
  QString wantArg ( const Object* ) const = 0;
  bool selectArg (Object* which) = 0;
//   void unselectArg (Object* which) = 0;

  // moving
  void startMove(const Coordinate&) = 0;
  void moveTo(const Coordinate&) = 0;
  void stopMove() = 0;
//   void cancelMove() = 0;

  void calc() = 0;

  Coordinate getPoint (double param) const;
  double getParam (const Coordinate&) const;

  const Coordinate& getP1() const { return p1;};
  const Coordinate& getP2() const { return p2;};

protected:
  Coordinate p1, p2;
  Coordinate pwwsm; // point where we started moving
};

// a line Through Two Points
class LineTTP
  : public Line
{
public:
  LineTTP() : pt1(0), pt2(0) { };
  ~LineTTP();
  LineTTP(const LineTTP& l);
  LineTTP* copy() { return new LineTTP(*this); };

  virtual const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "LineTTP"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName() { return i18n("Line by two points"); };
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription() { return i18n( "A line constructed through two points" ); };
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "line"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return CTRL+Key_L; };

  // arguments
  QString wantArg ( const Object* ) const;
  QString wantPoint() const;
  bool selectArg (Object* which);
  void unselectArg (Object* which);
  void drawPrelim ( KigPainter&, const Object* ) const;
  Objects getParents() const;

  // moving
  void startMove(const Coordinate&);
  void moveTo(const Coordinate&);
  void stopMove();
//   void cancelMove();

  void calc();

protected:
  Point* pt1;
  Point* pt2;
  Coordinate pwwsm;
};

// a perpendicular line on a line or segment, through a point
class LinePerpend
  : public Line
{
public:
  LinePerpend() : segment(0), line(0), point(0) {};
  ~LinePerpend();
  LinePerpend(const LinePerpend& l);
  LinePerpend* copy() { return new LinePerpend (*this);};

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return ("LinePerpend"); };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName() { return i18n("Perpendicular"); };
  const QString vDescription() const { return sDescription(); };
  static QString sDescription() { return i18n( "A line constructed through a point, perpendicular on another line or segment." ); };
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "perpendicular"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };

  // arguments
  QString wantArg ( const Object* ) const;
  QString wantPoint() const;
  bool selectArg (Object* which);
    Objects getParents() const;
//   void unselectArg (Object* which);
  void drawPrelim ( KigPainter&, const Object* ) const;

  // moving
  void startMove(const Coordinate&);
  void moveTo(const Coordinate&);
  void stopMove();
  void cancelMove();

  void calc();

protected:
  Segment* segment;
  Line* line;
  Point* point;
};

// a line parallel to a line or segment, through a point...
class LineParallel
  : public Line
{
public:
  LineParallel() : segment(0), line(0), point(0) {};
  ~LineParallel() {};
  LineParallel (const LineParallel& l);
  LineParallel* copy() { return new LineParallel (*this); };

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "LineParallel"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName() { return i18n("Parallel"); };
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription() { return i18n( "A line constructed through a point, and parallel to another line or segment" ); };
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "parallel"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };

  // arguments
  QString wantArg ( const Object* ) const;
  QString wantPoint() const;
  bool selectArg (Object* which);
  Objects getParents() const;
//   void unselectArg (Object* which);
  void drawPrelim ( KigPainter&, const Object* ) const;

  // moving
  void startMove(const Coordinate&) {};
  void moveTo(const Coordinate&) {};
  void stopMove() {};
  void cancelMove() {};

  void calc();

protected:
  Segment* segment;
  Line* line;
  Point* point;

};

/**
 * implements a radical line...
 * Maurizio Paolini wrote this as a patch for KGeo, after which i
 * ported it to Kig..
 * @author Maurizio Paolini
 */
class LineRadical
  : public Line
{
public:
  LineRadical();
  ~LineRadical() {};
  LineRadical (const LineRadical& l);
  LineRadical* copy() { return new LineRadical (*this); };

  virtual const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "LineRadical"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName() { return i18n("Radical Line"); };
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription() { return i18n( "A line constructed through the intersections of two circles.  This is also defined for non-intersecting circles..." ); };
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "radicalline"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };

  // arguments
  QString wantArg ( const Object* ) const;
  QString wantPoint() const;
  bool selectArg (Object* which);
  Objects getParents() const;
//   void unselectArg (Object* which);
  void drawPrelim ( KigPainter&, const Object* arg ) const;

  // moving
  // we don't move unless our parents do...
  void startMove(const Coordinate&) {};
  void moveTo(const Coordinate&) {};
  void stopMove() {};
  //  void cancelMove() {};

  void calc();

protected:
  Circle* c1;
  Circle* c2;
};
#endif
