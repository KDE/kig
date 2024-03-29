// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "filter.h"

class ObjectCalcer;

class KigFilterKSeg : public KigFilter
{
    KigFilterKSeg();
    ~KigFilterKSeg();

    ObjectCalcer *transformObject(KigDocument &kigdoc, std::vector<ObjectCalcer *> &parents, int subtype, bool &ok);

public:
    static KigFilterKSeg *instance();

    bool supportMime(const QString &mime) override;
    KigDocument *load(const QString &fromfile) override;
};
