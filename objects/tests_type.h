// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_TESTS_TYPE_H
#define KIG_OBJECTS_TESTS_TYPE_H

#include "base_type.h"

class AreParallelType
  : public ArgsParserObjectType
{
  AreParallelType();
  ~AreParallelType();
public:
  static const AreParallelType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class AreOrthogonalType
  : public ArgsParserObjectType
{
  AreOrthogonalType();
  ~AreOrthogonalType();
public:
  static const AreOrthogonalType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class AreCollinearType
  : public ArgsParserObjectType
{
  AreCollinearType();
  ~AreCollinearType();
public:
  static const AreCollinearType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ContainsTestType
  : public ArgsParserObjectType
{
  ContainsTestType();
  ~ContainsTestType();
public:
  static const ContainsTestType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class InPolygonTestType
  : public ArgsParserObjectType
{
  InPolygonTestType();
  ~InPolygonTestType();
public:
  static const InPolygonTestType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ConvexPolygonTestType
  : public ArgsParserObjectType
{
  ConvexPolygonTestType();
  ~ConvexPolygonTestType();
public:
  static const ConvexPolygonTestType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class SameDistanceType
  : public ArgsParserObjectType
{
  SameDistanceType();
  ~SameDistanceType();
public:
  static const SameDistanceType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class VectorEqualityTestType
  : public ArgsParserObjectType
{
  VectorEqualityTestType();
  ~VectorEqualityTestType();
public:
  static const VectorEqualityTestType* instance();
  ObjectImp* calc( const Args& parents,  const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ExistenceTestType
  : public ArgsParserObjectType
{
  ExistenceTestType();
  ~ExistenceTestType();
public:
  static const ExistenceTestType* instance();
  ObjectImp* calc( const Args& parents,  const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};
#endif
