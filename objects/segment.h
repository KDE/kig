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

#ifndef segment_h
#define segment_h

#include "abstractline.h"

#include "../misc/coordinate.h"

class Segment
  : public AbstractLine
{
  typedef AbstractLine Parent;
public:
  Segment() {};
  Segment( const Segment& s );
  ~Segment() {};

  bool isa( int type ) const;

  // some type information
  const QCString vBaseTypeName() const;
  static QCString sBaseTypeName();

  bool contains (const Coordinate& o, const ScreenInfo& si ) const;
  void draw ( KigPainter& p, bool showSelection ) const;

  bool inRect (const Rect&) const;

  Coordinate getPoint (double param) const;
  double getParam (const Coordinate&) const;

  const Coordinate p1() const;
  const Coordinate p2() const;

  const uint numberOfProperties() const;
  const Property property( uint which, const KigWidget& w ) const;
  const QCStringList properties() const;

protected:
  Coordinate mpa, mpb;
};

class SegmentAB
  : public Segment
{
public:
  SegmentAB( const Objects& os );
  SegmentAB( const SegmentAB& s );
  ~SegmentAB();

  const QCString vFullTypeName() const;
  static QCString sFullTypeName() { return "Segment"; };
  const QString vDescriptiveName() const;
  static const QString sDescriptiveName();
  const QString vDescription() const;
  static const QString sDescription();
  const QCString vIconFileName() const;
  static const QCString sIconFileName();
  const int vShortCut() const;
  static const int sShortCut();
  static const char* sActionName();

  // arguments
  static void sDrawPrelim ( KigPainter&, const Objects& o );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );

  Objects getParents() const;

  // moving
  void startMove(const Coordinate&, const ScreenInfo&);
  void moveTo(const Coordinate&);
  void stopMove();

  void calc();

protected:
  Point* pt1, *pt2;
  Coordinate pwwsm; // point where we started moving
};
#endif
