// SPDX-FileCopyrightText: 2005 Maurizio Paolini <paolini@dmf.unicatt.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "base_type.h"

/**
 * Circular inversion (new style)
 */

class CircularInversionType : public ArgsParserObjectType
{
    CircularInversionType();
    ~CircularInversionType();

public:
    static const CircularInversionType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

/**
 * Inversion of a point, line
 */
class InvertPointType : public ArgsParserObjectType
{
    InvertPointType();
    ~InvertPointType();

public:
    static const InvertPointType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class InvertLineType : public ArgsParserObjectType
{
    InvertLineType();
    ~InvertLineType();

public:
    static const InvertLineType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class InvertSegmentType : public ArgsParserObjectType
{
    InvertSegmentType();
    ~InvertSegmentType();

public:
    static const InvertSegmentType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class InvertCircleType : public ArgsParserObjectType
{
    InvertCircleType();
    ~InvertCircleType();

public:
    static const InvertCircleType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

class InvertArcType : public ArgsParserObjectType
{
    InvertArcType();
    ~InvertArcType();

public:
    static const InvertArcType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};
