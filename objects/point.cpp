/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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

#include "point.h"

#include "../misc/kigpainter.h"

#include <qpen.h>

#include "../misc/i18n.h"

Point::Point( const Coordinate& p )
 : mC( p )
{
};

bool Point::contains( const Coordinate& o, const double error ) const
{
  return (o - mC).length() <= error;
};

void Point::draw (KigPainter& p, bool ss) const
{
  if (!shown) return;
  bool s = selected && ss;
  p.setBrushStyle( Qt::SolidPattern );
  p.setBrushColor( s ? Qt::red : mColor );
  p.setPen( QPen ( s ? Qt::red : mColor, 1 ) );
  p.drawPoint( mC, false );
  p.setBrush (Qt::NoBrush);
};

Point* Point::toPoint()
{
  return this;
}

const Point* Point::toPoint() const
{
  return this;
}

const QCString Point::vBaseTypeName() const
{
  return sBaseTypeName();
}

const QCString Point::sBaseTypeName()
{
  return I18N_NOOP("point");
}

bool Point::inRect(const Rect& r) const
{
  return r.contains( mC );
}

const Coordinate& Point::getCoord() const
{
  return mC;
}

double Point::getX() const
{
  return mC.x;
}

double Point::getY() const
{
  return mC.y;
}

void Point::setX(const double inX)
{
  mC.x = inX;
}

void Point::setY(const double inY)
{
  mC.y = inY;
}

Point::Point()
{
}

Point::Point( const Point& p )
  : Object( p ), mC( p.mC )
{
}
