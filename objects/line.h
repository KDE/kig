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


#ifndef LINE_H
#define LINE_H

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
  virtual QCString vBaseTypeName() const { return sBaseTypeName();};
  static QCString sBaseTypeName() { return I18N_NOOP("line"); };

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

  const Coordinate& getP1() { return p1;};
  const Coordinate& getP2() { return p2;};

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

  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "LineTTP"; };

  // arguments
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
  void unselectArg (Object* which);
  void drawPrelim ( KigPainter& , const Coordinate& ) const;
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

  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return ("LinePerpend"); };

  // arguments
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
    Objects getParents() const;
//   void unselectArg (Object* which);
  void drawPrelim ( KigPainter&, const Coordinate& ) const;

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

  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "LineParallel"; };

  // arguments
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
  Objects getParents() const;
//   void unselectArg (Object* which);
  void drawPrelim ( KigPainter&, const Coordinate& ) const;

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
#endif
