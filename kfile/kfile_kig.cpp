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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include "kfile_kig.h"

#include <tqdom.h>
#include <tqfile.h>
#include <tqregexp.h>

#include <karchive.h>
#include <kgenericfactory.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ktar.h>

typedef KGenericFactory<KigPlugin> kigFactory;

K_EXPORT_COMPONENT_FACTORY( kfile_kig, kigFactory( "kfile_kig" ) )

KigPlugin::KigPlugin( TQObject *parent, const char *name, const TQStringList &args )
    : KFilePlugin( parent, name, args )
{
  KFileMimeTypeInfo::ItemInfo* item;
  
  info = addMimeTypeInfo( "application/x-kig" );

  group = addGroupInfo( info, "KigInfo", i18n( "Summary" ) );
  item = addItemInfo( group, "Version", i18n( "Version" ), TQVariant::String );
  item = addItemInfo( group, "CompatVersion", i18n( "Compatibility Version" ), TQVariant::String );
  item = addItemInfo( group, "CoordSystem", i18n( "Coordinate System" ), TQVariant::String );
  item = addItemInfo( group, "Grid", i18n( "Grid" ), TQVariant::String );
  item = addItemInfo( group, "Axes", i18n( "Axes" ), TQVariant::String );
  item = addItemInfo( group, "Compressed", i18n( "Compressed" ), TQVariant::String );
}

bool KigPlugin::readInfo( KFileMetaInfo& metainfo, uint /*what*/ )
{
  KFileMetaInfoGroup metagroup = appendGroup( metainfo, "KigInfo");

  TQString sfile =  metainfo.path();
  bool iscompressed = false;
  TQFile f( sfile );
  if ( !sfile.endsWith( ".kig", false ) )
  {
    iscompressed = true;

    TQString tempdir = KGlobal::dirs()->saveLocation( "tmp" );
    if ( tempdir.isEmpty() )
      return false;

    TQString tempname = sfile.section( '/', -1 );
    if ( sfile.endsWith( ".kigz", false ) )
    {
      tempname.remove( TQRegExp( "\\.[Kk][Ii][Gg][Zz]$" ) );
    }
    else
      return false;
    // reading compressed file
    KTar* ark = new KTar( sfile, "application/x-gzip" );
    ark->open( IO_ReadOnly );
    const KArchiveDirectory* dir = ark->directory();
    TQStringList entries = dir->entries();
    TQStringList kigfiles = entries.grep( TQRegExp( "\\.kig$" ) );
    if ( kigfiles.count() != 1 )
      return false;
    const KArchiveEntry* kigz = dir->entry( kigfiles[0] );
    if ( !kigz->isFile() )
      return false;
    dynamic_cast<const KArchiveFile*>( kigz )->copyTo( tempdir );

    f.setName( tempdir + kigz->name() );
  }

  if ( !f.open( IO_ReadOnly ) )
    return false;

  TQDomDocument doc( "KigDocument" );
  if ( !doc.setContent( &f ) )
    return false;

  f.close();

  // removing temp file
  if ( iscompressed )
    f.remove();

  TQDomElement main = doc.documentElement();

  // reading the version...
  TQString version = main.attribute( "Version" );
  if ( version.isEmpty() ) version = main.attribute( "version" );
  if ( version.isEmpty() ) version = i18n( "Translators: Not Available", "n/a" );
  appendItem( metagroup, "Version", version );

  // reading the compatibility version...
  TQString compatversion = main.attribute( "CompatibilityVersion" );
  if ( compatversion.isEmpty() )
    compatversion = i18n( "%1 represents Kig version",
                          "%1 (as the version)" ).arg( version );
  appendItem( metagroup, "CompatVersion", compatversion );

  // reading the Coordinate System...
  TQCString coordsystem;
  for ( TQDomNode n = main.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    TQDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    if ( e.tagName() == "CoordinateSystem" )
      coordsystem = e.text().latin1();
  }
  appendItem( metagroup, "CoordSystem", coordsystem );

  // has Kig document the grid?
  bool btmp = true;
  TQString stmp = main.attribute( "grid" );
  if ( !( stmp.isEmpty() || ( stmp != "0" ) ) )
    btmp = ( stmp != "0" );
  TQString stmp2 = btmp ? i18n( "Yes" ) : i18n( "No" );
  appendItem( metagroup, "Grid", stmp2 );

  // has Kig document the axes?
  btmp = true;
  stmp = main.attribute( "axes" );
  if ( !( stmp.isEmpty() || ( stmp != "0" ) ) )
    btmp = ( stmp != "0" );
  stmp2 = btmp ? i18n( "Yes" ) : i18n( "No" );
  appendItem( metagroup, "Axes", stmp2 );

  stmp2 = iscompressed ? i18n( "Yes" ) : i18n( "No" );
  appendItem( metagroup, "Compressed", stmp2 );

  return true;
}

#include "kfile_kig.moc"
