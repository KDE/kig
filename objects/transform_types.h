// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_TRANSFORM_TYPES_H
#define KIG_OBJECTS_TRANSFORM_TYPES_H

#include "object_type.h"

double getDoubleFromImp( const ObjectImp*, bool& );

class TranslatedType
  : public ArgsParserObjectType
{
  TranslatedType();
  ~TranslatedType();
public:
  static const TranslatedType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class PointReflectionType
  : public ArgsParserObjectType
{
  PointReflectionType();
  ~PointReflectionType();
public:
  static const PointReflectionType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class LineReflectionType
  : public ArgsParserObjectType
{
  LineReflectionType();
  ~LineReflectionType();
public:
  static const LineReflectionType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class RotationType
  : public ArgsParserObjectType
{
  RotationType();
  ~RotationType();
public:
  static const RotationType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class ScalingOverCenterType
  : public ArgsParserObjectType
{
  ScalingOverCenterType();
  ~ScalingOverCenterType();
public:
  static const ScalingOverCenterType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class ScalingOverCenter2Type
  : public ArgsParserObjectType
{
  ScalingOverCenter2Type();
  ~ScalingOverCenter2Type();
public:
  static const ScalingOverCenter2Type* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class ScalingOverLineType
  : public ArgsParserObjectType
{
  ScalingOverLineType();
  ~ScalingOverLineType();
public:
  static const ScalingOverLineType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class ScalingOverLine2Type
  : public ArgsParserObjectType
{
  ScalingOverLine2Type();
  ~ScalingOverLine2Type();
public:
  static const ScalingOverLine2Type* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class ProjectiveRotationType
  : public ArgsParserObjectType
{
  ProjectiveRotationType();
  ~ProjectiveRotationType();
public:
  static const ProjectiveRotationType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class HarmonicHomologyType
  : public ArgsParserObjectType
{
  HarmonicHomologyType();
  ~HarmonicHomologyType();
public:
  static const HarmonicHomologyType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class AffinityB2TrType
  : public ArgsParserObjectType
{
  AffinityB2TrType();
  ~AffinityB2TrType();
public:
  static const AffinityB2TrType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class AffinityGI3PType
  : public ArgsParserObjectType
{
  AffinityGI3PType();
  ~AffinityGI3PType();
public:
  static const AffinityGI3PType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class ProjectivityB2QuType
  : public ArgsParserObjectType
{
  ProjectivityB2QuType();
  ~ProjectivityB2QuType();
public:
  static const ProjectivityB2QuType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class ProjectivityGI4PType
  : public ArgsParserObjectType
{
  ProjectivityGI4PType();
  ~ProjectivityGI4PType();
public:
  static const ProjectivityGI4PType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class CastShadowType
  : public ArgsParserObjectType
{
  CastShadowType();
  ~CastShadowType();
public:
  static const CastShadowType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class ApplyTransformationObjectType
  : public ArgsParserObjectType
{
  ApplyTransformationObjectType();
  ~ApplyTransformationObjectType();
public:
  static const ApplyTransformationObjectType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
};

class SimilitudeType
  : public ArgsParserObjectType
{
  SimilitudeType();
  ~SimilitudeType();
public:
  static const SimilitudeType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

#endif
