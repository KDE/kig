// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

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
