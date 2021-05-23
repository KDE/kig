// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "rect.h"
#include "common.h"

Rect& Rect::operator=( const Rect& other )
{
  if (this != &other)
  {
    setBottomLeft( other.bottomLeft() );
    setWidth( other.width() );
    setHeight( other.height() );
  }
  normalize();
  return *this;
}

bool operator==( const Rect& r, const Rect& s )
{
  return ( r.bottomLeft() == s.bottomLeft()
           && r.width() == s.width()
           && r.height() == s.height() );
}

QDebug& operator<<( QDebug& s, const Rect& t )
{
  s << "left: " << t.left()
    << "bottom: " << t.bottom()
    << "right: " << t.right()
    << "top: " << t.top();
  return s;
}

Rect::Rect( const Coordinate &bottomLeft, const Coordinate &topRight )
  : mBottomLeft(bottomLeft)
{
  mwidth = topRight.x - bottomLeft.x;
  mheight = topRight.y - bottomLeft.y;
  normalize();
}

Rect::Rect( const Coordinate &p, const double width, const double height )
  : mBottomLeft(p),
    mwidth(width),
    mheight(height)
{
  normalize();
}

Rect::Rect( double xa, double ya, double width, double height )
  : mBottomLeft( xa, ya ),
    mwidth( width ),
    mheight( height )
{
  normalize();
}

Rect::Rect( const Rect& r )
  : mBottomLeft (r.mBottomLeft),
    mwidth(r.mwidth),
    mheight(r.mheight)
{
  normalize();
}

Rect::Rect()
  : mwidth(0),
    mheight(0)
{
}

void Rect::setBottomLeft( const Coordinate &p )
{
  mBottomLeft = p;
}

void Rect::setBottomRight( const Coordinate &p )
{
  mBottomLeft = p - Coordinate(mwidth,0);
}

void Rect::setTopRight( const Coordinate &p )
{
  mBottomLeft = p - Coordinate(mwidth, mheight);
}

void Rect::setCenter( const Coordinate &p )
{
  mBottomLeft = p - Coordinate(mwidth, mheight)/2;
}

void Rect::setLeft( const double p )
{
  double r = right();
  mBottomLeft.x = p;
  setRight( r );
}

void Rect::setRight( const double p )
{
  mwidth = p - left();
}

void Rect::setBottom( const double p )
{
  double t = top();
  mBottomLeft.y = p;
  setTop( t );
}

void Rect::setTop( const double p )
{
  mheight = p - bottom();
}

void Rect::setWidth( const double w )
{
  mwidth = w;
}

void Rect::setHeight( const double h )
{
  mheight = h;
}

void Rect::normalize()
{
  if ( mwidth < 0 )
    {
      mBottomLeft.x += mwidth;
      mwidth = -mwidth;
    };
  if ( mheight < 0 )
    {
      mBottomLeft.y += mheight;
      mheight = -mheight;
    };
}

void Rect::moveBy( const Coordinate &p )
{
  mBottomLeft += p;
}

void Rect::scale( const double r )
{
  mwidth *= r;
  mheight *= r;
}


QRect Rect::toQRect() const
{
  return QRect(mBottomLeft.toQPoint(), topRight().toQPoint());
}

Coordinate Rect::bottomLeft() const
{
  return mBottomLeft;
}

Coordinate Rect::bottomRight() const
{
  return mBottomLeft + Coordinate(mwidth, 0);
}

Coordinate Rect::topLeft() const
{
  return mBottomLeft + Coordinate(0, mheight);
}

Coordinate Rect::topRight() const
{
  return mBottomLeft + Coordinate(mwidth, mheight);
}

Coordinate Rect::center() const
{
  return mBottomLeft + Coordinate(mwidth, mheight)/2;
}

double Rect::left() const
{
  return mBottomLeft.x;
}
double Rect::right() const
{
  return left() + mwidth;
}
double Rect::bottom() const
{
  return mBottomLeft.y;
}

double Rect::top() const
{
  return bottom() + mheight;
}

double Rect::width() const
{
  return mwidth;
}

double Rect::height() const
{
  return mheight;
}

bool Rect::contains( const Coordinate& p, double allowed_miss ) const
{
  return p.x - left() >= - allowed_miss &&
    p.y - bottom() >= - allowed_miss &&
    p.x - left() - width() <= allowed_miss &&
    p.y - bottom() - height() <= allowed_miss;
}

bool Rect::contains( const Coordinate& p ) const
{
  return p.x >= left() &&
    p.y >= bottom() &&
    p.x - left() <= width() &&
    p.y - bottom() <= height();
}

bool Rect::intersects( const Rect& p ) const
{
  // never thought it was this simple :)
  if( p.left() < left() && p.right() < left()) return false;
  if( p.left() > right() && p.right() > right()) return false;
  if( p.bottom() < bottom() && p.top() < bottom()) return false;
  if( p.bottom() > top() && p.top() > top()) return false;
  return true;
}

void Rect::setContains( const Coordinate &p )
{
  normalize();
  if( p.x < left() ) setLeft( p.x );
  if( p.x > right() ) setRight(p.x);
  if( p.y < bottom() ) setBottom( p.y );
  if( p.y > top() ) setTop( p.y );
}

Rect Rect::normalized() const
{
  Rect t = *this;
  (void) t.normalize();
  return t;
}

Rect Rect::fromQRect( const QRect& r )
{
  return Rect( r.left(), r.top(), r.right(), r.bottom() );
}

void Rect::setTopLeft( const Coordinate &p )
{
  Coordinate bl = Coordinate( p.x, p.y - mheight );
  setBottomLeft( bl );
}

Rect operator|( const Rect& lhs, const Rect& rhs )
{
  Rect r( lhs );
  r |= rhs;
  return r;
}

void Rect::eat( const Rect& r )
{
  setLeft( kigMin( left(), r.left() ) );
  setRight( kigMax( right(), r.right() ) );
  setBottom( kigMin( bottom(), r.bottom() ) );
  setTop( kigMax( top(), r.top() ) );
}

Rect Rect::matchShape( const Rect& rhs, bool shrink ) const
{
  Rect ret = *this;
  Coordinate c = center();
  double v = width()/height(); // current ratio
  double w = rhs.width()/rhs.height(); // wanted ratio

  // we don't show less than r, if the dimensions don't match, we
  // extend r into some dimension...
  if( ( v > w ) ^ shrink )
    ret.setHeight( ret.width() / w );
  else
    ret.setWidth( ret.height() * w );

  ret.setCenter(c);
  return ret.normalized();
}

bool Rect::valid()
{
  return mBottomLeft.valid() && mwidth != double_inf && mheight != double_inf;
}

Rect Rect::invalidRect()
{
  return Rect( Coordinate::invalidCoord(), double_inf, double_inf );
}
