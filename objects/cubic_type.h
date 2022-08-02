// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "object_type.h"

class CubicB9PType : public ArgsParserObjectType
{
    CubicB9PType();
    ~CubicB9PType();

public:
    static const CubicB9PType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class CubicNodeB6PType : public ArgsParserObjectType
{
    CubicNodeB6PType();
    ~CubicNodeB6PType();

public:
    static const CubicNodeB6PType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class CubicCuspB4PType : public ArgsParserObjectType
{
    CubicCuspB4PType();
    ~CubicCuspB4PType();

public:
    static const CubicCuspB4PType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class VerticalCubicB4PType : public ArgsParserObjectType
{
    VerticalCubicB4PType();
    ~VerticalCubicB4PType();

public:
    static const VerticalCubicB4PType *instance();
    ObjectImp *calc(const Args &parents, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};
