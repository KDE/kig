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

#ifndef KIG_OBJECTS_ROTATED_POINT_H
#define KIG_OBJECTS_ROTATED_POINT_H

#include "point.h"

class Arc;

class RotatedPoint
  : public Point
{
  Point* mp;
  Point* mc;
  Arc* ma;
public:
  RotatedPoint( const Objects& os );
  RotatedPoint( const RotatedPoint& p );
  ~RotatedPoint();

  virtual RotatedPoint* copy();

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

  void startMove(const Coordinate& );
  void moveTo(const Coordinate& );
  void stopMove();
  void cancelMove();

  void drawPrelim( KigPainter&, const Object* ) const;

  void calc( const ScreenInfo& );
};

#endif
