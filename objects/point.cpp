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

#include "point.h"

#include "../modes/popup.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "../kig/kig_view.h"

#include <qpen.h>
#include <qpopupmenu.h>

Point::Point( const Coordinate& p )
  : mC( p ), msize( 5 )
{
};

bool Point::contains( const Coordinate& o, const ScreenInfo& si ) const
{
  return (o - mC).length() - msize*si.pixelWidth() < 0;
};

void Point::draw (KigPainter& p, bool ss) const
{
  bool s = selected && ss;
  p.setBrushStyle( Qt::SolidPattern );
  p.setBrushColor( s ? Qt::red : mColor );
  p.setPen( QPen ( s ? Qt::red : mColor, 1 ) );
  p.drawFatPoint( mC, msize * p.pixelWidth() );
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
  : mC(), msize( 5 )
{
}

Point::Point( const Point& p )
  : Object( p ), mC( p.mC ), msize( 5 )
{
}

Object::prop_map Point::getParams()
{
  prop_map ret = Object::getParams();
  ret["point-size"] = QString::number( msize );
  return ret;
}

void Point::setParams( const prop_map& m )
{
  Object::setParams( m );
  prop_map::const_iterator p = m.find( "point-size" );
  if ( p == m.end() ) msize = 5;
  else
  {
    bool ok = true;
    msize = p->second.toInt( &ok );
    if ( ! ok ) msize = 5;
  };
}

void Point::addActions( NormalModePopupObjects& p )
{
  QPopupMenu* pop = new QPopupMenu( &p, "point virtual popup" );
  QPixmap pix( 20, 20 );
  for ( int i = 1; i < 8; ++i )
  {
    int size = 1 + 2*i;
    pix.fill( pop->eraseColor() );
    QPainter ptr( &pix );
    ptr.setPen( QPen( mColor, 1 ) );
    ptr.setBrush( QBrush( mColor, Qt::SolidPattern ) );
    QRect r( ( 20 - size ) / 2,  ( 20 - size ) / 2, size, size );
    ptr.drawEllipse( r );
    pop->insertItem( pix, i );
  };
  p.addPopupAction( 10, i18n( "Set size..." ), pop );
}

void Point::doPopupAction( int popupid, int actionid, KigDocument*, KigWidget* w, NormalMode*, const Coordinate& )
{
  if ( popupid == 10 )
  {
    msize = 1+ 2*actionid;
    w->redrawScreen();
  };
}

const uint Point::numberOfProperties() const
{
  return Object::numberOfProperties() + 1;
}

const Property Point::property( uint which ) const
{
  if ( which < Object::numberOfProperties() ) return Object::property( which );
  assert( which <= numberOfProperties() );
  if ( which == Object::numberOfProperties() )
    return Property( mC );
  else assert( false );
}

const QCStringList Point::properties() const
{
  QCStringList l = Object::properties();
  l << I18N_NOOP( "Coordinate" );
  assert( l.size() == Point::numberOfProperties() );
  return l;
}
