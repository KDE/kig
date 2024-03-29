// SPDX-FileCopyrightText: 2003-2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "../misc/object_hierarchy.h"
#include "base_type.h"

/**
 * an arc by a start point, an intermediate point and an end point
 * (with orientation)
 */
class ArcBTPType : public ArgsParserObjectType
{
    typedef ArgsParserObjectType Parent;
    ArcBTPType();
    ~ArcBTPType();

public:
    static const ArcBTPType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;

    const ObjectImpType *impRequirement(const ObjectImp *o, const Args &parents) const override;

    bool inherits(int type) const override;
    const ObjectImpType *resultId() const override;
};

/**
 * an arc by a point (center), a starting point and an angle
 */
class ArcBCPAType : public ArgsParserObjectType
{
    typedef ArgsParserObjectType Parent;
    ArcBCPAType();
    ~ArcBCPAType();

public:
    static const ArcBCPAType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;

    const ObjectImpType *impRequirement(const ObjectImp *o, const Args &parents) const override;

    bool inherits(int type) const override;
    const ObjectImpType *resultId() const override;
};

/**
 * a conic arc by a start point, an intermediate point, an end point and
 * the conic center
 */
class ConicArcBCTPType : public ArgsParserObjectType
{
    typedef ArgsParserObjectType Parent;
    ConicArcBCTPType();
    ~ConicArcBCTPType();

public:
    static const ConicArcBCTPType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;

    const ObjectImpType *resultId() const override;
};

/**
 * a conic arc by a five points, a starting point, intermediate,
 * intermediate (used to compute angles), intermediate and
 * end point
 */
class ConicArcB5PType : public ArgsParserObjectType
{
    typedef ArgsParserObjectType Parent;
    ConicArcB5PType();
    ~ConicArcB5PType();

public:
    static const ConicArcB5PType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;

    const ObjectImpType *resultId() const override;
};
