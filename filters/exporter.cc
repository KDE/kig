// exporter.cc
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "exporter.h"

#include "exporttoimagedialog.h"

#include <klocale.h>
#include <kaction.h>

class ExporterAction
  : public KAction
{
  KigExporter* mexp;
  const KigDocument* mdoc;
  KigWidget* mw;
public:
  ExporterAction( const KigDocument* doc, KigWidget* w,
                  KActionCollection* parent, KigExporter* exp );
  void slotActivated();
};

ExporterAction::ExporterAction( const KigDocument* doc, KigWidget* w,
                                KActionCollection* parent, KigExporter* exp )
  : KAction( exp->menuEntryName(), KShortcut(), 0, 0, parent ),
    mexp( exp ), mdoc( doc ), mw( w )
{
};

void ExporterAction::slotActivated()
{
  mexp->run( *mdoc, *mw );
};

KigExporter::~KigExporter()
{
}

ImageExporter::~ImageExporter()
{
}

QString ImageExporter::exportToStatement() const
{
  return i18n( "&Export to image" );
}

QString ImageExporter::menuEntryName() const
{
  return i18n( "&Image..." );
}

void ImageExporter::run( const KigDocument& doc, KigWidget& w )
{
  ExportToImageDialog* d = new ExportToImageDialog( &w, &doc );
  d->exec();
  delete d;
}

KigExportManager::KigExportManager()
{
  mexporters.push_back( new ImageExporter );
}

KigExportManager::~KigExportManager()
{
  for ( uint i = 0; i < mexporters.size(); ++i )
    delete mexporters[i];
}

void KigExportManager::addMenuAction( const KigDocument* doc, KigWidget* w,
                                      KActionCollection* coll )
{
  KActionMenu* m = new KActionMenu( i18n( "&Export to" ), coll, "file_export" );
  for ( uint i = 0; i < mexporters.size(); ++i )
    m->insert( new ExporterAction( doc, w, coll, mexporters[i] ) );
}

KigExportManager* KigExportManager::instance()
{
  static KigExportManager m;
  return &m;
}
