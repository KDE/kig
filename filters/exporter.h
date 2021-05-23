// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_FILTERS_EXPORTER_H
#define KIG_FILTERS_EXPORTER_H

#include <QAction>

#include <vector>

class QString;
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

class ExporterAction
  : public QAction
{
  Q_OBJECT

  KigExporter* mexp;
  const KigPart* mdoc;
  KigWidget* mw;
public:
  ExporterAction( const KigPart* doc, KigWidget* w,
                  KActionCollection* parent, KigExporter* exp );
private slots:
  void slotActivated();
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
 * This exporter takes care of the "Export to Image" stuff.
 */
class ImageExporter
  : public KigExporter
{
public:
  ~ImageExporter();
  QString exportToStatement() const Q_DECL_OVERRIDE;
  QString menuEntryName() const Q_DECL_OVERRIDE;
  QString menuIcon() const Q_DECL_OVERRIDE;
  void run( const KigPart& doc, KigWidget& w ) Q_DECL_OVERRIDE;
};

#endif
