// midpoint.h
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#ifndef MIDPOINT_H
#define MIDPOINT_H

#include "point.h"

/**
 * midpoint of two other points
 */
class MidPoint
  : public Point
{
public:
  MidPoint( const Objects& os );
  MidPoint( const MidPoint& m );
  ~MidPoint();
  MidPoint* copy() { return new MidPoint(*this); };

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "MidPoint"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName()  { return "bisection"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects&, const Object* o );
  static void sDrawPrelim( KigPainter& p, const Objects& args );
  Objects getParents() const;

  void startMove(const Coordinate&, const ScreenInfo&);
  void moveTo(const Coordinate&);
  void stopMove();
  void cancelMove();
  void calc( const ScreenInfo& r );
  void drawPrelim( KigPainter&, const Object* ) const {};
protected:
  enum { howmMoving, howmFollowing } howm; // how are we moving
  Point* p1;
  Point* p2;
  Segment* s;
};

#endif
