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


#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "point.h"

class Segment;
class Line;
class Circle;

class IntersectionPoint
  : public Point
{
  Segment* segment1;
  Segment* segment2;
  Line* line1;
  Line* line2;
  Circle* circle1;
  Circle* circle2;
public:
  IntersectionPoint()
    : segment1(0), segment2(0),
      line1(0), line2(0),
      circle1(0), circle2(0)
  {
  };

  IntersectionPoint( const IntersectionPoint& p )
    : Point( p ),
      segment1( p.segment1 ), segment2( p.segment2 ),
      line1( p.line1 ), line2( p.line2 ),
      circle1( p.circle1 ), circle2( p.circle2 )
  {
  };

  ~IntersectionPoint() {};

  virtual IntersectionPoint* copy() { return new IntersectionPoint( *this ); };

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "IntersectionPoint"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "intersection"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return CTRL+Key_I; };

  void moved( ) { calc(); };
  QString wantArg(const Object* o) const;
  bool selectArg(Object* o);
  void unselectArg(Object* o);
  Objects getParents() const;

  void startMove(const Coordinate& ) {};
  void moveTo(const Coordinate& ) {};
  void stopMove() {};
  void cancelMove() {};

  void drawPrelim( KigPainter&, const Object* ) const {};

  void calc();
};

#endif
