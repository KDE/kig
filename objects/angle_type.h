// SPDX-FileCopyrightText: 2003-2004 Dominique Devriese <devriese@kde.org>
// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MISC_ANGLE_TYPE_H
#define KIG_MISC_ANGLE_TYPE_H

#include "base_type.h"

class AngleType : public ArgsParserObjectType
{
    AngleType();
    ~AngleType();

public:
    static const AngleType *instance();
    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;

    QStringList specialActions() const override;
    void executeAction(int i, ObjectHolder &o, ObjectTypeCalcer &c, KigPart &d, KigWidget &w, NormalMode &m) const override;
};

class HalfAngleType : public ArgsParserObjectType
{
    HalfAngleType();
    ~HalfAngleType();

public:
    static const HalfAngleType *instance();
    ObjectImp *calc(const Args &args, const KigDocument &) const override;
    const ObjectImpType *resultId() const override;
};

#endif
