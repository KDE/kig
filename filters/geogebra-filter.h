/*
    GeoGebra Filter for Kig
    SPDX-FileCopyrightText: 2013 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

*/

#ifndef GEOGEBRA_FILTER_H
#define GEOGEBRA_FILTER_H

#include "filter.h"

#include <QAbstractXmlReceiver>
#include <QMap>
#include <QXmlNamePool>

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

#endif // GEOGEBRA_FILTER_H
