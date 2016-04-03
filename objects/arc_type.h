// Copyright (C) 2003-2004  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_OBJECTS_ARC_TYPE_H
#define KIG_OBJECTS_ARC_TYPE_H

#include "base_type.h"
#include "../misc/object_hierarchy.h"

/**
 * an arc by a start point, an intermediate point and an end point
 * (with orientation)
 */
class ArcBTPType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  ArcBTPType();
  ~ArcBTPType();
public:
  static const ArcBTPType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;

  bool inherits( int type ) const;
  const ObjectImpType* resultId() const;
};

/**
 * an arc by a point (center), a starting point and an angle
 */
class ArcBCPAType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  ArcBCPAType();
  ~ArcBCPAType();
public:
  static const ArcBCPAType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;

  bool inherits( int type ) const;
  const ObjectImpType* resultId() const;
};

/**
 * a conic arc by a start point, an intermediate point, an end point and
 * the conic center
 */
class ConicArcBCTPType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  ConicArcBCTPType();
  ~ConicArcBCTPType();
public:
  static const ConicArcBCTPType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;

  const ObjectImpType* resultId() const;
};

/**
 * a conic arc by a five points, a starting point, intermediate,
 * intermediate (used to compute angles), intermediate and
 * end point
 */
class ConicArcB5PType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  ConicArcB5PType();
  ~ConicArcB5PType();
public:
  static const ConicArcB5PType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const;

  const ObjectImpType* resultId() const;
};

#endif
