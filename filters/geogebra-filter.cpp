/*
 * GeoGebra Filter for Kig
 * Copyright 2013  David E. Narvaez <david.narvaez@computer.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "geogebra-filter.h"

#include <kig/kig_document.h>
#include <objects/object_factory.h>
#include <objects/object_calcer.h>
#include <objects/object_holder.h>
#include <objects/bogus_imp.h>
#include <objects/object_type_factory.h>
#include <geogebra/geogebratransformer.h>

#include <KZip>
#include <QDebug>

#include <QFile>
#include <QXmlQuery>

#include <algorithm>

KigFilterGeogebra* KigFilterGeogebra::instance()
{
  static KigFilterGeogebra f;
  return &f;
}

bool KigFilterGeogebra::supportMime( const QString& mime )
{
  return mime == QLatin1String("application/vnd.geogebra.file");
}

static ObjectHolder* holderFromCalcerAndDrawer( ObjectCalcer* oc, ObjectDrawer* od )
{
  return new ObjectHolder( oc, od );
}

KigDocument* KigFilterGeogebra::load( const QString& sFrom )
{
  KZip geogebraFile( sFrom );
  KigDocument * document = new KigDocument();

  if ( geogebraFile.open( QIODevice::ReadOnly ) )
  {
    const KZipFileEntry* geogebraXMLEntry = dynamic_cast<const KZipFileEntry*>( geogebraFile.directory()->entry( QStringLiteral("geogebra.xml") ) );

    if ( geogebraXMLEntry )
    {
      QXmlNamePool np;
      QXmlQuery geogebraXSLT( QXmlQuery::XSLT20, np );
      const QString encodedData = QString::fromUtf8( geogebraXMLEntry->data().constData() );
      QFile queryDevice( QStringLiteral(":/kig/geogebra/geogebra.xsl") );
      GeogebraTransformer ggbtransform( document, np );

      queryDevice.open( QFile::ReadOnly );
      geogebraXSLT.setFocus( encodedData );
      geogebraXSLT.setQuery( &queryDevice );
      geogebraXSLT.evaluateTo( &ggbtransform );
      queryDevice.close();

      assert( ggbtransform.getNumberOfSections() == 1 );

      const GeogebraSection & gs = ggbtransform.getSection( 0 );
      const std::vector<ObjectCalcer *> & f = gs.getOutputObjects();
      const std::vector<ObjectDrawer *> & d = gs.getDrawers();
      std::vector<ObjectHolder *> holders( f.size() );

      std::transform( f.cbegin(), f.cend(), d.begin(), holders.begin(), holderFromCalcerAndDrawer );

      document->addObjects( holders );
    }
  }
  else
  {
    qWarning() << "Failed to open zip archive";
  }

  return document;
}


