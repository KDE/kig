/***************************************************************************
 *   Copyright (C) 2004 by Pino Toscano                                    *
 *   toscano.pino@tiscali.it                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <config.h>

#include "kfile_kig.h"

#include <qdom.h>
#include <qfile.h>

#include <kgenericfactory.h>

typedef KGenericFactory<KigPlugin> kigFactory;

K_EXPORT_COMPONENT_FACTORY( kfile_kig, kigFactory( "kfile_kig" ) )

KigPlugin::KigPlugin( QObject *parent, const char *name, const QStringList &args )
    : KFilePlugin( parent, name, args )
{
  KFileMimeTypeInfo::ItemInfo* item;
  
  info = addMimeTypeInfo( "application/x-kig" );

  group = addGroupInfo( info, "KigInfo", i18n( "Summary" ) );
  item = addItemInfo( group, "Version", i18n( "Version" ), QVariant::String );
  item = addItemInfo( group, "CoordSystem", i18n( "Coordinate System" ), QVariant::String );
}

bool KigPlugin::readInfo( KFileMetaInfo& metainfo, uint /*what*/ )
{
  KFileMetaInfoGroup metagroup = appendGroup( metainfo, "KigInfo");

  QFile f( metainfo.path() );
  QDomDocument doc( "KigDocument" );
  if ( !doc.setContent( &f ) )
    return false;
  QDomElement main = doc.documentElement();

  // reading the version...
  QString version = main.attribute( "Version" );
  if ( ! version ) version = main.attribute( "version" );
  if ( ! version )
    return false;
  appendItem( metagroup, "Version", version );

  // reading the Coordinate System...
  QCString coordsystem;
  for ( QDomNode n = main.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    QDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    if ( e.tagName() == "CoordinateSystem" )
      coordsystem = e.text().latin1();
  }
  appendItem( metagroup, "CoordSystem", coordsystem );

  return true;
}

#include "kfile_kig.moc"
