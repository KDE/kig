// transform_types.h
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

#ifndef KIG_OBJECTS_TRANSFORM_TYPES_H
#define KIG_OBJECTS_TRANSFORM_TYPES_H

#include "object_type.h"

class TranslatedType
  : public BuiltinType
{
  TranslatedType();
  ~TranslatedType();
public:
  static const TranslatedType* instance();
  ObjectImp* calc( const Args& args ) const;
  int resultId() const;
};

class PointReflectionType
  : public BuiltinType
{
  PointReflectionType();
  ~PointReflectionType();
public:
  static const PointReflectionType* instance();
  ObjectImp* calc( const Args& args ) const;
  int resultId() const;
};

class LineReflectionType
  : public BuiltinType
{
  LineReflectionType();
  ~LineReflectionType();
public:
  static const LineReflectionType* instance();
  ObjectImp* calc( const Args& args ) const;
  int resultId() const;
};

class RotationType
  : public BuiltinType
{
  RotationType();
  ~RotationType();
public:
  static const RotationType* instance();
  ObjectImp* calc( const Args& args ) const;
  int resultId() const;
};

class ScalingOverCenterType
  : public BuiltinType
{
  ScalingOverCenterType();
  ~ScalingOverCenterType();
public:
  static const ScalingOverCenterType* instance();
  ObjectImp* calc( const Args& args ) const;
  int resultId() const;
};

class ScalingOverLineType
  : public BuiltinType
{
  ScalingOverLineType();
  ~ScalingOverLineType();
public:
  static const ScalingOverLineType* instance();
  ObjectImp* calc( const Args& args ) const;
  int resultId() const;
};

class ProjectiveRotationType
  : public BuiltinType
{
  ProjectiveRotationType();
  ~ProjectiveRotationType();
public:
  static const ProjectiveRotationType* instance();
  ObjectImp* calc( const Args& args ) const;
  int resultId() const;
};

class CastShadowType
  : public BuiltinType
{
  CastShadowType();
  ~CastShadowType();
public:
  static const CastShadowType* instance();
  ObjectImp* calc( const Args& args ) const;
  int resultId() const;
};

#endif
