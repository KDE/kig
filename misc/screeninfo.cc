// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "screeninfo.h"

#include <cmath>

ScreenInfo::ScreenInfo( const Rect& docRect, const QRect& viewRect )
  : mkrect( docRect.normalized() ), mqrect( viewRect.normalized() )
{
}

Rect ScreenInfo::fromScreen( const QRect& r ) const
{
  return Rect(
    fromScreen( r.topLeft() ),
    fromScreen( r.bottomRight() )
    ).normalized();
}

Coordinate ScreenInfo::fromScreen( const QPoint& p ) const
{
  // invert the y-axis: 0 is at the bottom !
  Coordinate t( p.x(), mqrect.height() - p.y() );
  t *= mkrect.width();
  t /= mqrect.width();
  return t + mkrect.bottomLeft();
}

QPoint ScreenInfo::toScreen( const Coordinate& p ) const
{
  Coordinate t = p - mkrect.bottomLeft();
  t *= mqrect.width();
  t /= mkrect.width();
  // invert the y-axis: 0 is at the bottom !
  return QPoint( (int) t.x, mqrect.height() - (int) t.y );
}

QRect ScreenInfo::toScreen( const Rect& r ) const
{
  return QRect(
    toScreen( r.bottomLeft() ),
    toScreen( r.topRight() )
    ).normalized();
}

QPointF ScreenInfo::toScreenF( const Coordinate& p ) const
{
  Coordinate t = p - mkrect.bottomLeft();
  t *= mqrect.width();
  t /= mkrect.width();
  // invert the y-axis: 0 is at the bottom !
  return QPointF( t.x, mqrect.height() - t.y );
}

QRectF ScreenInfo::toScreenF( const Rect& r ) const
{
  return QRectF(
    toScreenF( r.bottomLeft() ),
    toScreenF( r.topRight() )
    ).normalized();
}

double ScreenInfo::pixelWidth() const
{
  Coordinate a = fromScreen( QPoint( 0, 0 ) );
  Coordinate b = fromScreen( QPoint( 0, 1000 ) );
  return std::fabs( b.y - a.y ) / 1000;
}

const Rect& ScreenInfo::shownRect() const
{
  return mkrect;
}

void ScreenInfo::setShownRect( const Rect& r )
{
  mkrect = r;
}

const QRect ScreenInfo::viewRect() const
{
  return mqrect;
}

void ScreenInfo::setViewRect( const QRect& r )
{
  mqrect = r;
}

double ScreenInfo::normalMiss( int width ) const
{
  int twidth = width == -1 ? 1 : width;
  return (twidth+2)*pixelWidth();
}
