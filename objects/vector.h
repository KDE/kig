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

#ifndef KIG_OBJECTS_VECTOR_H
#define KIG_OBJECTS_VECTOR_H

#include "object.h"

#include "../misc/coordinate.h"

class Vector
  : public Object
{
public:
  Vector( const Objects& os );
  Vector( const Vector& s );
  Vector* copy();
  ~Vector();

  // type identification
  Vector* toVector();
  const Vector* toVector() const;
  // some type information
  const QCString vBaseTypeName() const;
  static QCString sBaseTypeName();
  const QCString vFullTypeName() const;
  static QCString sFullTypeName();
  const QString vDescriptiveName() const;
  static const QString sDescriptiveName();
  const QString vDescription() const;
  static const QString sDescription();
  const QCString vIconFileName() const;
  static const QCString sIconFileName();
  const int vShortCut() const;
  static const int sShortCut();
  static const char* sActionName();

  bool contains (const Coordinate& o, const ScreenInfo& si ) const;
  void draw ( KigPainter& p, bool showSelection ) const;

  static void drawVector( KigPainter& p, const Coordinate& a,
                          const Coordinate& b );

  bool inRect ( const Rect& ) const;

  // arguments
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );
  static void sDrawPrelim( KigPainter&, const Objects& o );

  Objects getParents() const;

  // moving
  void startMove(const Coordinate&, const ScreenInfo&);
  void moveTo(const Coordinate&);
  void stopMove();

  void calc();

  const Coordinate getP1() const;
  const Coordinate getP2() const;
  const Coordinate getDir() const;

protected:
  Point* p1, *p2;
};

#endif
