// SPDX-FileCopyrightText: 2010, 2011 Raoul Bourquin <raoulb@bluewin.ch>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_FILTERS_ASYEXPORTER_H
#define KIG_FILTERS_ASYEXPORTER_H

#include "exporter.h"

class QString;
class KigPart;
class KigWidget;

/**
 * Export to Asymptote.
 */
class AsyExporter
  : public KigExporter
{
public:
  ~AsyExporter();
  QString exportToStatement() const Q_DECL_OVERRIDE;
  QString menuEntryName() const Q_DECL_OVERRIDE;
  QString menuIcon() const Q_DECL_OVERRIDE;
  void run( const KigPart& doc, KigWidget& w ) Q_DECL_OVERRIDE;
};

#endif
