// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "base_type.h"

class AreParallelType : public ArgsParserObjectType
{
    AreParallelType();
    ~AreParallelType();

public:
    static const AreParallelType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class AreOrthogonalType : public ArgsParserObjectType
{
    AreOrthogonalType();
    ~AreOrthogonalType();

public:
    static const AreOrthogonalType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class AreCollinearType : public ArgsParserObjectType
{
    AreCollinearType();
    ~AreCollinearType();

public:
    static const AreCollinearType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ContainsTestType : public ArgsParserObjectType
{
    ContainsTestType();
    ~ContainsTestType();

public:
    static const ContainsTestType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class InPolygonTestType : public ArgsParserObjectType
{
    InPolygonTestType();
    ~InPolygonTestType();

public:
    static const InPolygonTestType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ConvexPolygonTestType : public ArgsParserObjectType
{
    ConvexPolygonTestType();
    ~ConvexPolygonTestType();

public:
    static const ConvexPolygonTestType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class SameDistanceType : public ArgsParserObjectType
{
    SameDistanceType();
    ~SameDistanceType();

public:
    static const SameDistanceType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class VectorEqualityTestType : public ArgsParserObjectType
{
    VectorEqualityTestType();
    ~VectorEqualityTestType();

public:
    static const VectorEqualityTestType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ExistenceTestType : public ArgsParserObjectType
{
    ExistenceTestType();
    ~ExistenceTestType();

public:
    static const ExistenceTestType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};
