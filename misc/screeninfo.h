// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <QRect>

#include "rect.h"

/**
 * ScreenInfo is a simple utility class that maps a region of the
 * document onto a region of the screen.  It is used by both
 * KigPainter and KigWidget, and the objects use it in their calc()
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
  QRect toScreen( const Rect& r ) const;
  QPointF toScreenF( const Coordinate& p ) const;
  QRectF toScreenF( const Rect& r ) const;

  double pixelWidth() const;

  double normalMiss( int width ) const;

  const Rect& shownRect() const;

  void setShownRect( const Rect& r );

  const QRect viewRect() const;

  void setViewRect( const QRect& r );
};

#endif
