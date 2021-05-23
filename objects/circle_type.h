// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_CIRCLE_TYPE_H
#define KIG_OBJECTS_CIRCLE_TYPE_H

#include "base_type.h"

/**
 * Circle by center and point
 */
class CircleBCPType
  : public ObjectABType
{
  CircleBCPType();
  ~CircleBCPType();
public:
  static const CircleBCPType* instance();

  ObjectImp* calcx( const Coordinate& a, const Coordinate& b ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

/**
 * Circle by point and radius.
 */
class CircleBPRType
  : public ArgsParserObjectType
{
  CircleBPRType();
  ~CircleBPRType();
public:
  static const CircleBPRType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

/**
 * Circle by three points (with orientation)
 */
class CircleBTPType
  : public ArgsParserObjectType
{
  CircleBTPType();
  ~CircleBTPType();

public:
  static const CircleBTPType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};


#endif
