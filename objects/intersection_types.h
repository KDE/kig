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
  : public ArgparserObjectType
{
  ConicLineIntersectionType();
  ~ConicLineIntersectionType();
public:
  static const ConicLineIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  int resultId() const;
};

class LineLineIntersectionType
  : public ArgparserObjectType
{
  LineLineIntersectionType();
  ~LineLineIntersectionType();
public:
  static const LineLineIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  int resultId() const;
};

class LineCubicIntersectionType
  : public ArgparserObjectType
{
  LineCubicIntersectionType();
  ~LineCubicIntersectionType();
public:
  static const LineCubicIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  int resultId() const;
};

#endif
