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

#ifndef POINT_H
#define POINT_H

#include "object.h"
#include "../misc/coordinate.h"

class Rect;

class Point
: public Object
{
protected:
  Point();
  Point( const Coordinate& p );
  Point( const Point& p );
public:
  // type info:
  Point* toPoint();
  const Point* toPoint() const;

  // type identification
  virtual const QCString vBaseTypeName() const;
  static const QCString sBaseTypeName();

  // general members
  virtual bool contains (const Coordinate& o, const ScreenInfo& si ) const;
  virtual void draw (KigPainter& p,bool showSelection = true) const;
  virtual bool inRect(const Rect& r) const;

  const Coordinate& getCoord() const;

  virtual std::map<QCString,QString> getParams ();
  virtual void setParams ( const std::map<QCString,QString>& );

  virtual void addActions( NormalModePopupObjects& );
  virtual void doPopupAction( int popupid, int actionid, KigDocument* d, KigWidget* w, NormalMode* m );

  virtual const uint numberOfProperties();
  virtual const Property property( uint which );
  virtual const QStringList properties();

public:
  double getX() const;
  double getY() const;
  void setX (const double inX);
  void setY (const double inY);

protected:
  Coordinate mC;
  int msize;
};

#endif // POINT_H
