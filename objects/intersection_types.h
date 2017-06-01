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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

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
 * Conics.
 */
class ConicLineIntersectionType
  : public ArgsParserObjectType
{
  ConicLineIntersectionType();
  ~ConicLineIntersectionType();
public:
  static const ConicLineIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

/**
 * conic line 'other' intersection.  In case we already know one of the
 * two intersections
 */
class ConicLineOtherIntersectionType
  : public ArgsParserObjectType
{
  ConicLineOtherIntersectionType();
  ~ConicLineOtherIntersectionType();
public:
  static const ConicLineOtherIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};
/**
 * Francesca Gatti (frency.gatti@gmail.com), january 2008:
 *
 * cubic line 'other' intersection.  In case we already know two of the
 * three intersections
 */

class CubicLineOtherIntersectionType
  : public ArgsParserObjectType
{
  CubicLineOtherIntersectionType();
  ~CubicLineOtherIntersectionType();
public:
  static const CubicLineOtherIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};
/**
 *
 * one of the two cubic line intersection points,  in case we already know one of the
 * three intersections
 */
class CubicLineTwoIntersectionType
  : public ArgsParserObjectType
{
  CubicLineTwoIntersectionType();
  ~CubicLineTwoIntersectionType();
public:
  static const CubicLineTwoIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};
/**
 * circle circle 'other' intersection.  In case we already know one of the
 * two intersections
 */
class CircleCircleOtherIntersectionType
  : public ArgsParserObjectType
{
  CircleCircleOtherIntersectionType();
  ~CircleCircleOtherIntersectionType();
public:
  static const CircleCircleOtherIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class LineLineIntersectionType
  : public ArgsParserObjectType
{
  LineLineIntersectionType();
  ~LineLineIntersectionType();
public:
  static const LineLineIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class CubicLineIntersectionType
  : public ArgsParserObjectType
{
  CubicLineIntersectionType();
  ~CubicLineIntersectionType();
public:
  static const CubicLineIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class CircleCircleIntersectionType
  : public ArgsParserObjectType
{
  CircleCircleIntersectionType();
  ~CircleCircleIntersectionType();
public:
  static const CircleCircleIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

/**
 * arc line intersection.
 */
class ArcLineIntersectionType
  : public ArgsParserObjectType
{
  ArcLineIntersectionType();
  ~ArcLineIntersectionType();
public:
  static const ArcLineIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

#endif
