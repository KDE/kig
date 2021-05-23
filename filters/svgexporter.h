// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_FILTERS_SVGEXPORTER_H
#define KIG_FILTERS_SVGEXPORTER_H

#include "exporter.h"

class QString;
class KigPart;
class KigWidget;

/**
 * Export to Scalable Vector Graphics (SVG)
 */
class SVGExporter
  : public KigExporter
{
public:
  ~SVGExporter();
  QString exportToStatement() const Q_DECL_OVERRIDE;
  QString menuEntryName() const Q_DECL_OVERRIDE;
  QString menuIcon() const Q_DECL_OVERRIDE;
  void run( const KigPart& part, KigWidget& w ) Q_DECL_OVERRIDE;
};

#endif
