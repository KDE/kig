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


#ifndef COORDINATES_H
#define COORDINATES_H

#include "kigpainter.h"
#include "coordinate.h"

#include <qobject.h>

class CoordinateSystem;
class QValidator;

class CoordinateSystemFactory
{
public:
  static CoordinateSystem* build( const char* type );
};

// a CoordinateSystem is what the user sees: it is kept by KigPart to
// show the user a grid, and to show the coordinates of points... it
// allows for weird CoordinateSystem's like homogeneous or
// projective...
// internally, it does nothing, it could almost have been an ordinary
// Object..., mapping coordinates from and to the screen to and from
// the internal coordinates is done elsewhere ( KigPainter and
// KigWidget... )
class CoordinateSystem
  : public Qt
{
public:
  CoordinateSystem();
  virtual ~CoordinateSystem();
  virtual QString fromScreen ( const Coordinate& pt, const KigDocument& w ) const = 0;
  /**
   * This returns a notice to say in which format coordinates should
   * be entered.  This should be something like:
   * i18n( "Enter coordinates in the following form: \"(x,y)\", where
   * x is the x coordinate, and y is the y coordinate." );
   */
  virtual QString coordinateFormatNotice() const = 0;
  virtual Coordinate toScreen (const QString& pt, bool& ok) const = 0;
  virtual void drawGrid ( KigPainter& p, bool showgrid = true, bool showaxes = true ) const = 0;

  virtual const char* type() const = 0;

  virtual QValidator* coordinateValidator() const = 0;
};

class EuclideanCoords
  : public CoordinateSystem
{
public:
  EuclideanCoords();
  ~EuclideanCoords();
  QString fromScreen( const Coordinate& pt, const KigDocument& w ) const;
  QString coordinateFormatNotice() const;
  Coordinate toScreen (const QString& pt, bool& ok) const;
  void drawGrid ( KigPainter& p, bool showgrid = true, bool showaxes = true ) const;

  const char* type() const;

  QValidator* coordinateValidator() const;
};

class PolarCoords
  : public CoordinateSystem
{
public:
  PolarCoords();
  ~PolarCoords();
  QString fromScreen( const Coordinate& pt, const KigDocument& w ) const;
  QString coordinateFormatNotice() const;
  Coordinate toScreen (const QString& pt, bool& ok) const;
  void drawGrid ( KigPainter& p, bool showgrid = true, bool showaxes = true ) const;

  const char* type() const;
  QValidator* coordinateValidator() const;
};

#endif
