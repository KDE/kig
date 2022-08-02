// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "exporter.h"

/**
 * Guess what this one does ;)
 * It exports to the XFig file format, as documented in the file
 * FORMAT3.2 in the XFig source distribution.
 */
class XFigExporter : public KigExporter
{
public:
    ~XFigExporter();
    QString exportToStatement() const override;
    QString menuEntryName() const override;
    QString menuIcon() const override;
    void run(const KigPart &doc, KigWidget &w) override;
};
