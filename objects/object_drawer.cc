// object_drawer.cc
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#include "object_drawer.h"

#include "object_imp.h"
#include "../misc/kigpainter.h"

#include <cassert>

void ObjectDrawer::draw( const ObjectImp& imp, KigPainter& p, bool sel ) const
{
  if ( mshown )
  {
    p.setBrushStyle( Qt::NoBrush );
    p.setBrushColor( sel ? Qt::red : mcolor );
    p.setPen( QPen ( sel ? Qt::red : mcolor,  1) );
    p.setWidth( mwidth );
    imp.draw( p );
  }
}

bool ObjectDrawer::contains( const ObjectImp& imp, const Coordinate& p, const KigWidget& w ) const
{
  return mshown && imp.contains( p, mwidth, w );
}

bool ObjectDrawer::shown() const
{
  return mshown;
}

QColor ObjectDrawer::color() const
{
  return mcolor;
}

ObjectDrawer* ObjectDrawer::getCopyShown( bool s ) const
{
  ObjectDrawer* ret = new ObjectDrawer;
  ret->mcolor = mcolor;
  ret->mshown = s;
  ret->mwidth = mwidth;
  return ret;
}

ObjectDrawer* ObjectDrawer::getCopyColor( const QColor& c ) const
{
  ObjectDrawer* ret = new ObjectDrawer;
  ret->mcolor = c;
  ret->mshown = mshown;
  ret->mwidth = mwidth;
  return ret;
}

ObjectDrawer* ObjectDrawer::getCopyWidth( int w ) const
{
  ObjectDrawer* ret = new ObjectDrawer;
  ret->mcolor = mcolor;
  ret->mshown = mshown;
  ret->mwidth = w;
  return ret;
}

int ObjectDrawer::width() const
{
  return mwidth;
}

ObjectDrawer::ObjectDrawer( const QColor& color, int width, bool shown )
  : mcolor( color ), mshown( shown ), mwidth( width )
{
}

ObjectDrawer::ObjectDrawer()
  : mcolor( Qt::blue ), mshown( true ), mwidth( -1 )
{
}

bool ObjectDrawer::inRect( const ObjectImp& imp, const Rect& r, const KigWidget& w ) const
{
  return mshown && imp.inRect( r, mwidth, w );
}
