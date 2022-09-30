/*
    GeoGebra Filter for Kig
    SPDX-FileCopyrightText: 2013 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

*/

#pragma once

#include "filter.h"

class ObjectCalcer;
class ObjectType;

class KigFilterGeogebra : public KigFilter
{
public:
    static KigFilterGeogebra *instance();
    KigDocument *load(const QString &fromfile) override;
    bool supportMime(const QString &mime) override;

protected:
    KigFilterGeogebra()
    {
    }
    ~KigFilterGeogebra()
    {
    }
};
