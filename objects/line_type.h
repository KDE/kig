// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "base_type.h"

class LineData;

class SegmentABType : public ObjectABType
{
    SegmentABType();
    ~SegmentABType();

public:
    static const SegmentABType *instance();

    ObjectImp *calcx(const Coordinate &a, const Coordinate &b) const override;
    const ObjectImpType *resultId() const override;

    QStringList specialActions() const override;
    /**
     * execute the \p i 'th action from the specialActions above.
     */
    void executeAction(int i, ObjectHolder &o, ObjectTypeCalcer &c, KigPart &d, KigWidget &w, NormalMode &m) const override;
};

class LineABType : public ObjectABType
{
    LineABType();
    ~LineABType();

public:
    static const LineABType *instance();
    ObjectImp *calcx(const Coordinate &a, const Coordinate &b) const override;
    const ObjectImpType *resultId() const override;
};

class RayABType : public ObjectABType
{
    RayABType();
    ~RayABType();

public:
    static const RayABType *instance();
    ObjectImp *calcx(const Coordinate &a, const Coordinate &b) const override;
    const ObjectImpType *resultId() const override;
};

class SegmentAxisType : public ArgsParserObjectType
{
    SegmentAxisType();
    ~SegmentAxisType();

public:
    static const SegmentAxisType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &d) const override;
    const ObjectImpType *resultId() const override;
};

class LinePerpendLPType : public ObjectLPType
{
    LinePerpendLPType();
    ~LinePerpendLPType();

public:
    static LinePerpendLPType *instance();
    using ObjectLPType::calc;
    ObjectImp *calc(const LineData &a, const Coordinate &b) const override;
    const ObjectImpType *resultId() const override;
};

class LineParallelLPType : public ObjectLPType
{
    LineParallelLPType();
    ~LineParallelLPType();

public:
    static LineParallelLPType *instance();
    using ObjectLPType::calc;
    ObjectImp *calc(const LineData &a, const Coordinate &b) const override;
    const ObjectImpType *resultId() const override;
};

class LineByVectorType : public ArgsParserObjectType
{
    LineByVectorType();
    ~LineByVectorType();

public:
    static const LineByVectorType *instance();
    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class HalflineByVectorType : public ArgsParserObjectType
{
    HalflineByVectorType();
    ~HalflineByVectorType();

public:
    static const HalflineByVectorType *instance();
    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};
