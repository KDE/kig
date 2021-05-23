// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_TANGENT_TYPE_H
#define KIG_OBJECTS_TANGENT_TYPE_H

#include "base_type.h"

/**
 * the line tangent to a generic conic
 */
class TangentConicType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  TangentConicType();
  ~TangentConicType();
public:
  static const TangentConicType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

/**
 * the line tangent to an arc
 */
class TangentArcType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  TangentArcType();
  ~TangentArcType();
public:
  static const TangentArcType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

/**
 * the line tangent to a cubic
 */
class TangentCubicType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  TangentCubicType();
  ~TangentCubicType();
public:
  static const TangentCubicType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

/**
 * the line tangent to a curve
 */
class TangentCurveType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  TangentCurveType();
  ~TangentCurveType();
public:
  static const TangentCurveType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

#endif
