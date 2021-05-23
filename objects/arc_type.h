// SPDX-FileCopyrightText: 2003-2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

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

  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;

  bool inherits( int type ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
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

  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;

  bool inherits( int type ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
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

  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;

  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
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

  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;

  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

#endif
