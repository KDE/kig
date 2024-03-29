// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>
// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "filter.h"

class QDomNode;
class KigDocument;
class QString;

/**
 * This is an import filter for the GNOME geometry program DrGeo.
 */
class KigFilterDrgeo : public KigFilter
{
protected:
    KigFilterDrgeo();
    ~KigFilterDrgeo();

public:
    static KigFilterDrgeo *instance();

    bool supportMime(const QString &mime) override;
    KigDocument *load(const QString &file) override;

private:
    KigDocument *importFigure(const QDomNode &f, const bool grid);
};
