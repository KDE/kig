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
  : public ArgsParserObjectType
{
  TranslatedType();
  ~TranslatedType();
public:
  static const TranslatedType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class PointReflectionType
  : public ArgsParserObjectType
{
  PointReflectionType();
  ~PointReflectionType();
public:
  static const PointReflectionType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class LineReflectionType
  : public ArgsParserObjectType
{
  LineReflectionType();
  ~LineReflectionType();
public:
  static const LineReflectionType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class RotationType
  : public ArgsParserObjectType
{
  RotationType();
  ~RotationType();
public:
  static const RotationType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class ScalingOverCenterType
  : public ArgsParserObjectType
{
  ScalingOverCenterType();
  ~ScalingOverCenterType();
public:
  static const ScalingOverCenterType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class ScalingOverCenter2Type
  : public ArgsParserObjectType
{
  ScalingOverCenter2Type();
  ~ScalingOverCenter2Type();
public:
  static const ScalingOverCenter2Type* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class ScalingOverLineType
  : public ArgsParserObjectType
{
  ScalingOverLineType();
  ~ScalingOverLineType();
public:
  static const ScalingOverLineType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class ScalingOverLine2Type
  : public ArgsParserObjectType
{
  ScalingOverLine2Type();
  ~ScalingOverLine2Type();
public:
  static const ScalingOverLine2Type* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class ProjectiveRotationType
  : public ArgsParserObjectType
{
  ProjectiveRotationType();
  ~ProjectiveRotationType();
public:
  static const ProjectiveRotationType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class HarmonicHomologyType
  : public ArgsParserObjectType
{
  HarmonicHomologyType();
  ~HarmonicHomologyType();
public:
  static const HarmonicHomologyType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class AffinityGI3PType
  : public ArgsParserObjectType
{
  AffinityGI3PType();
  ~AffinityGI3PType();
public:
  static const AffinityGI3PType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class ProjectivityGI4PType
  : public ArgsParserObjectType
{
  ProjectivityGI4PType();
  ~ProjectivityGI4PType();
public:
  static const ProjectivityGI4PType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class CastShadowType
  : public ArgsParserObjectType
{
  CastShadowType();
  ~CastShadowType();
public:
  static const CastShadowType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

class ApplyTransformationObjectType
  : public ArgsParserObjectType
{
  ApplyTransformationObjectType();
  ~ApplyTransformationObjectType();
public:
  static const ApplyTransformationObjectType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  bool isTransform() const;
};

class SimilitudeType
  : public ArgsParserObjectType
{
  SimilitudeType();
  ~SimilitudeType();
public:
  static const SimilitudeType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  bool isTransform() const;
};

#endif
