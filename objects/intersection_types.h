// intersection_types.h
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

#ifndef KIG_OBJECTS_INTERSECTION_TYPES_H
#define KIG_OBJECTS_INTERSECTION_TYPES_H

#include "object_type.h"

/**
 * conic line intersection.  This also serves as circle-line
 * intersection, in which case it uses the easier way to calc
 * ...  There is no separate CircleLineIntersectionPoint, since the
 * difference between both types is quite small ( same number of
 * intersections with a line, for example.. ), and since with
 * transformations, Circles might dynamically change types to
 * Conics..
 */
class ConicLineIntersectionType
  : public ObjectType
{
public:
  ConicLineIntersectionType();
  ~ConicLineIntersectionType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents, const KigWidget& w ) const;
};

class LineLineIntersectionType
  : public ObjectType
{
public:
  LineLineIntersectionType();
  ~LineLineIntersectionType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents, const KigWidget& ) const;
};

class LineCubicIntersectionType
  : public ObjectType
{
public:
  LineCubicIntersectionType();
  ~LineCubicIntersectionType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents, const KigWidget& ) const;
};

#endif
