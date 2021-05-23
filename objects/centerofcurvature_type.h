// SPDX-FileCopyrightText: 2004 Maurizio Paolini <paolini@dmf.unicatt.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_CENTEROFCURVATURE_TYPE_H
#define KIG_OBJECTS_CENTEROFCURVATURE_TYPE_H

#include "base_type.h"

/**
 * the center of curvature of a conic at a point
 */
class CocConicType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  CocConicType();
  ~CocConicType();
public:
  static const CocConicType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

/**
 * the center of curvature of a cubic at a point
 */
class CocCubicType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  CocCubicType();
  ~CocCubicType();
public:
  static const CocCubicType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

/**
 * the center of curvature of a curve at a point
 */
class CocCurveType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  CocCurveType();
  ~CocCurveType();
public:
  static const CocCurveType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

#endif
