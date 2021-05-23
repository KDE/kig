// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_VECTOR_TYPE_H
#define KIG_OBJECTS_VECTOR_TYPE_H

#include "base_type.h"

class VectorType
  : public ObjectABType
{
  VectorType();
  ~VectorType();
public:
  static const VectorType* instance();
  ObjectImp* calcx( const Coordinate& a, const Coordinate& b ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class VectorSumType
  : public ArgsParserObjectType
{
  VectorSumType();
  ~VectorSumType();
public:
  static const VectorSumType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

#endif
