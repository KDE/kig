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

#ifndef KIG_OBJECTS_LINE_H
#define KIG_OBJECTS_LINE_H

#include "abstractline.h"
#include "../misc/common.h"

class Line
  : public AbstractLine
{
  typedef AbstractLine Parent;
public:
  Line() {};
  Line( const Line& l );
  ~Line() {};

  bool isa( int type ) const;

  // type identification
  virtual const QCString vBaseTypeName() const;
  static const QCString sBaseTypeName();

  bool contains (const Coordinate& o, const ScreenInfo& si ) const;
  void draw (KigPainter& p, bool showSelection) const;
  bool inRect (const Rect&) const;

  // implement the Curve interface...
  Coordinate getPoint (double param) const;
  double getParam (const Coordinate&) const;

  const Coordinate p1() const;
  const Coordinate p2() const;

protected:
  Coordinate mpa, mpb;
  Coordinate pwwsm; // point where we started moving
};

// a line Through Two Points
class LineTTP
  : public Line
{
public:
  LineTTP( const Objects& os );
  LineTTP(const LineTTP& l);
  ~LineTTP();

  virtual const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "LineTTP"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "line"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return CTRL+Key_L; };
  static const char* sActionName();

  // arguments
  static void sDrawPrelim ( KigPainter&, const Objects& o );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );
  Objects getParents() const;

  // moving
  void startMove(const Coordinate&, const ScreenInfo&);
  void moveTo(const Coordinate&);

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
  LinePerpend( const Objects& os );
  LinePerpend( const LinePerpend& l );
  ~LinePerpend();

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return ("LinePerpend"); };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "perpendicular"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // arguments
  static void sDrawPrelim ( KigPainter& p, const Objects& os );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );

  Objects getParents() const;

  void calc();

protected:
  AbstractLine* mdir;
  Point* point;
};

// a line parallel to a line or segment, through a point...
class LineParallel
  : public Line
{
public:
  LineParallel( const Objects& o );
  LineParallel (const LineParallel& l);
  ~LineParallel() {};

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "LineParallel"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "parallel"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // arguments
  static void sDrawPrelim ( KigPainter& p, const Objects& os );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );
  Objects getParents() const;

  void calc();

protected:
  AbstractLine* mdir;
  Point* point;
};

/**
 * implements a radical line...
 * Maurizio Paolini wrote this as a patch for KGeo.  I ported it to
 * Kig..
 * @author Maurizio Paolini
 */
class LineRadical
  : public Line
{
public:
  LineRadical( const Objects& os );
  LineRadical( const LineRadical& l );
  ~LineRadical() {};

  virtual const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "LineRadical"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "radicalline"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // arguments
  static void sDrawPrelim ( KigPainter& p, const Objects& os );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );
  Objects getParents() const;

  void calc();

protected:
  Circle* c1;
  Circle* c2;
};
#endif
