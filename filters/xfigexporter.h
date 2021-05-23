// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_FILTERS_XFIGEXPORTER_H
#define KIG_FILTERS_XFIGEXPORTER_H

#include "exporter.h"

/**
 * Guess what this one does ;)
 * It exports to the XFig file format, as documented in the file
 * FORMAT3.2 in the XFig source distribution.
 */
class XFigExporter
  : public KigExporter
{
public:
  ~XFigExporter();
  QString exportToStatement() const Q_DECL_OVERRIDE;
  QString menuEntryName() const Q_DECL_OVERRIDE;
  QString menuIcon() const Q_DECL_OVERRIDE;
  void run( const KigPart& doc, KigWidget& w ) Q_DECL_OVERRIDE;
};
#endif
