// screeninfo.h
// Copyright (C)  2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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

#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <qrect.h>

#include "rect.h"

/**
 * ScreenInfo is a simple utility class that maps a region of the
 * document onto a region of the screen.  It is used by both
 * KigPainter and KigView, and the objects use it in their calc()
 * method...
 */
class ScreenInfo
{
  Rect mkrect;
  QRect mqrect;
public:
  ScreenInfo( const Rect& docRect, const QRect& viewRect );

  Coordinate fromScreen( const QPoint& p ) const;
  Rect fromScreen( const QRect& r ) const;

  QPoint toScreen( const Coordinate& p ) const;
  QRect toScreen( const Rect r ) const;

  double pixelWidth() const;

  const Rect& shownRect() const;

  void setShownRect( const Rect& r );

  const QRect& viewRect() const;

  void setViewRect( const QRect& r );
};

#endif
