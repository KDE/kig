// SPDX-FileCopyrightText: 2010, 2011 Raoul Bourquin <raoulb@bluewin.ch>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "exporter.h"

class QString;
class KigPart;
class KigWidget;

/**
 * Export to Asymptote.
 */
class AsyExporter : public KigExporter
{
public:
    ~AsyExporter();
    QString exportToStatement() const override;
    QString menuEntryName() const override;
    QString menuIcon() const override;
    void run(const KigPart &doc, KigWidget &w) override;
};
