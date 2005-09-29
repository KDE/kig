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

#ifndef KIG_FILTERS_EXPORTER_H
#define KIG_FILTERS_EXPORTER_H

#include <vector>

class QString;
class KigDocument;
class KigPart;
class KigWidget;
class KActionCollection;

class KigExporter;

class KigExportManager
{
  std::vector<KigExporter*> mexporters;
  KigExportManager();
  ~KigExportManager();
public:
  static KigExportManager* instance();
  void addMenuAction( const KigPart* doc, KigWidget* w,
                      KActionCollection* coll );
};

/**
 * Base class for a Kig exporter.
 *
 * Subclass it and implement its methods to have it working.
 */
class KigExporter
{
public:
  virtual ~KigExporter();

  /**
   * Returns a statement like i18n( "Export to image" )
   */
  virtual QString exportToStatement() const = 0;
  /**
   * Returns a string like i18n( "Image..." )
   */
  virtual QString menuEntryName() const = 0;
  /**
   * Returns a string with the name of the icon
   */
  virtual QString menuIcon() const = 0;

  /**
   * Do what you need to do.  It's up to the individual exporters to
   * ask the user for which file to export to etc., because they can
   * do a much better job at that..
   */
  virtual void run( const KigPart& doc, KigWidget& w ) = 0;
};

/**
 * This exporter takes care of the "Export to Image" stuff..
 */
class ImageExporter
  : public KigExporter
{
public:
  ~ImageExporter();
  QString exportToStatement() const;
  QString menuEntryName() const;
  QString menuIcon() const;
  void run( const KigPart& doc, KigWidget& w );
};

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
  QString exportToStatement() const;
  QString menuEntryName() const;
  QString menuIcon() const;
  void run( const KigPart& doc, KigWidget& w );
};
#endif
