// SPDX-FileCopyrightText: 2003-2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "../misc/object_hierarchy.h"
#include "base_type.h"

class LocusType : public ArgsParserObjectType
{
    typedef ArgsParserObjectType Parent;
    LocusType();
    ~LocusType();

public:
    static const LocusType *instance();

    ObjectImp *calc(const Args &args, const KigDocument &) const override;

    const ObjectImpType *impRequirement(const ObjectImp *o, const Args &parents) const override;

    bool inherits(int type) const override;
    const ObjectImpType *resultId() const override;

    std::vector<ObjectCalcer *> sortArgs(const std::vector<ObjectCalcer *> &args) const override;
    Args sortArgs(const Args &args) const override;
};

class CopyObjectType : public ObjectType
{
protected:
    CopyObjectType();
    ~CopyObjectType();

public:
    static CopyObjectType *instance();
    bool inherits(int type) const override;
    ObjectImp *calc(const Args &parents, const KigDocument &d) const override;
    const ObjectImpType *impRequirement(const ObjectImp *o, const Args &parents) const override;
    bool isDefinedOnOrThrough(const ObjectImp *o, const Args &parents) const override;
    const ObjectImpType *resultId() const override;
    std::vector<ObjectCalcer *> sortArgs(const std::vector<ObjectCalcer *> &os) const override;
    Args sortArgs(const Args &args) const override;
};
