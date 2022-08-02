// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "base_type.h"

class ConicB5PType : public ArgsParserObjectType
{
    ConicB5PType();
    ~ConicB5PType();

public:
    static const ConicB5PType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ConicBAAPType : public ArgsParserObjectType
{
    ConicBAAPType();
    ~ConicBAAPType();

public:
    static const ConicBAAPType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ConicBFFPType : public ArgsParserObjectType
{
protected:
    ConicBFFPType(const char *fullname, const ArgsParser::spec *, int n);
    ~ConicBFFPType();

public:
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;

    /**
     * -1 for hyperbola, 1 for ellipse.
     */
    virtual int type() const = 0;
    const ObjectImpType *resultId() const override;
};

class EllipseBFFPType : public ConicBFFPType
{
    EllipseBFFPType();
    ~EllipseBFFPType();

public:
    static const EllipseBFFPType *instance();
    int type() const override;
};

class HyperbolaBFFPType : public ConicBFFPType
{
    HyperbolaBFFPType();
    ~HyperbolaBFFPType();

public:
    static const HyperbolaBFFPType *instance();
    int type() const override;
};

class ConicBDFPType : public ArgsParserObjectType
{
    ConicBDFPType();
    ~ConicBDFPType();

public:
    static const ConicBDFPType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ParabolaBTPType : public ArgsParserObjectType
{
    ParabolaBTPType();
    ~ParabolaBTPType();

public:
    static const ParabolaBTPType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class EquilateralHyperbolaB4PType : public ArgsParserObjectType
{
    EquilateralHyperbolaB4PType();
    ~EquilateralHyperbolaB4PType();

public:
    static const EquilateralHyperbolaB4PType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ConicPolarPointType : public ArgsParserObjectType
{
    ConicPolarPointType();
    ~ConicPolarPointType();

public:
    static const ConicPolarPointType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ConicPolarLineType : public ArgsParserObjectType
{
    ConicPolarLineType();
    ~ConicPolarLineType();

public:
    static const ConicPolarLineType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ConicDirectrixType : public ArgsParserObjectType
{
    ConicDirectrixType();
    ~ConicDirectrixType();

public:
    static const ConicDirectrixType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ParabolaBDPType : public ObjectLPType
{
    ParabolaBDPType();
    ~ParabolaBDPType();

public:
    static const ParabolaBDPType *instance();
    using ObjectLPType::calc;
    ObjectImp *calc(const LineData &l, const Coordinate &c) const override;
    const ObjectImpType *resultId() const override;
};

class ConicAsymptoteType : public ArgsParserObjectType
{
    ConicAsymptoteType();
    ~ConicAsymptoteType();

public:
    static const ConicAsymptoteType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class ConicRadicalType : public ArgsParserObjectType
{
    ConicRadicalType();
    ~ConicRadicalType();

public:
    static const ConicRadicalType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
    QStringList specialActions() const override;
    void executeAction(int i, ObjectHolder &o, ObjectTypeCalcer &t, KigPart &d, KigWidget &w, NormalMode &m) const override;
};
