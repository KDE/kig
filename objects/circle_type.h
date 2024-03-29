// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "base_type.h"

/**
 * Circle by center and point
 */
class CircleBCPType : public ObjectABType
{
    CircleBCPType();
    ~CircleBCPType();

public:
    static const CircleBCPType *instance();

    ObjectImp *calcx(const Coordinate &a, const Coordinate &b) const override;
    const ObjectImpType *resultId() const override;
};

/**
 * Circle by point and radius.
 */
class CircleBPRType : public ArgsParserObjectType
{
    CircleBPRType();
    ~CircleBPRType();

public:
    static const CircleBPRType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

/**
 * Circle by three points (with orientation)
 */
class CircleBTPType : public ArgsParserObjectType
{
    CircleBTPType();
    ~CircleBTPType();

public:
    static const CircleBTPType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};
