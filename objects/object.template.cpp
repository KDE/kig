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


#include "segment.h"

#include <klocale.h>

bool Segment::contains(const Coordinate& o, bool strict) const
{
  return false;
}

void Segment::draw(KigPainter& p, bool selected, bool showSelection)
{
    if ( !shown ) return;
}

bool Segment::inRect(const Rect& p) const
{
}

Rect Segment::getSpan(void) const
{
}

QString Segment::wantArg( const Object* o) const
{
  return 0;
}

bool Segment::selectArg(Object* which)
{
  return true; // if we're complete (don't need any more args), we return true
}

void Segment::unselectArg(Object* which)
{
}

void Segment::startMove(const Coordinate& p)
{
}

void Segment::moveTo(const Coordinate& p)
{
}

void Segment::stopMove()
{
}

void Segment::cancelMove()
{
}

void Segment::moved(Object*)
{
}
