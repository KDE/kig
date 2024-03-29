// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "base_type.h"

class VectorType : public ObjectABType
{
    VectorType();
    ~VectorType();

public:
    static const VectorType *instance();
    ObjectImp *calcx(const Coordinate &a, const Coordinate &b) const override;
    const ObjectImpType *resultId() const override;
};

class VectorSumType : public ArgsParserObjectType
{
    VectorSumType();
    ~VectorSumType();

public:
    static const VectorSumType *instance();
    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};
