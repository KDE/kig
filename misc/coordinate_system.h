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


#ifndef COORDINATES_H
#define COORDINATES_H

#include "kigpainter.h"
#include "coordinate.h"

#include <qobject.h>

// a CoordinateSystem is what the user sees: it is kept by KigPart to
// show the user a grid, and to show the coordinates of points... it
// allows for weird CoordinateSystem's like homogeneous or
// projective...
// internally, it does nothing, it could almost have been an ordinary
// Object..., mapping coordinates from and to the screen to and from
// the internal coordinates is done elsewhere ( KigPainter and
// KigView... )
class CoordinateSystem
  : public Qt
{
public:
  CoordinateSystem() {};
  virtual ~CoordinateSystem() {};
  virtual QString fromScreen (const Coordinate& pt) const = 0;
  virtual Coordinate toScreen (const QString& pt, bool& ok) const = 0;
  virtual void drawGrid ( KigPainter& p ) const = 0;
};

class EuclideanCoords
  : public CoordinateSystem
{
public:
  EuclideanCoords();
  ~EuclideanCoords() {};
  virtual QString fromScreen (const Coordinate& pt) const;
  virtual Coordinate toScreen (const QString& pt, bool& ok) const;
  virtual void drawGrid ( KigPainter& p ) const;
};

#endif
