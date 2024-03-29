// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "exporter.h"

class QString;
class KigPart;
class KigWidget;

/**
 * Export to LaTex.
 */
class LatexExporter : public KigExporter
{
public:
    ~LatexExporter();
    QString exportToStatement() const override;
    QString menuEntryName() const override;
    QString menuIcon() const override;
    void run(const KigPart &doc, KigWidget &w) override;
};
