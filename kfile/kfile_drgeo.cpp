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

#include "kfile_drgeo.h"

#include <tqdom.h>
#include <tqfile.h>

#include <kgenericfactory.h>

typedef KGenericFactory<DrgeoPlugin> drgeoFactory;

K_EXPORT_COMPONENT_FACTORY( kfile_drgeo, drgeoFactory( "kfile_drgeo" ) )

DrgeoPlugin::DrgeoPlugin( TQObject *parent, const char *name, const TQStringList &args )
    : KFilePlugin( parent, name, args )
{
  info = addMimeTypeInfo( "application/x-drgeo" );

  KFileMimeTypeInfo::GroupInfo* group = addGroupInfo( info, "DrgeoInfo", i18n( "Summary" ) );
  KFileMimeTypeInfo::ItemInfo* item;
  item = addItemInfo( group, "NumOfFigures", i18n( "Figures" ), TQVariant::Int );
  item = addItemInfo( group, "NumOfTexts", i18n( "Texts" ), TQVariant::Int );
  item = addItemInfo( group, "NumOfMacros", i18n( "Macros" ), TQVariant::Int );
  
  group_contents = addGroupInfo( info, "DrgeoContents", i18n( "Translators: what this drgeo "
                                                              "file tqcontains", "Contents" ) );
}

bool DrgeoPlugin::readInfo( KFileMetaInfo& metainfo, uint /*what*/ )
{
  KFileMetaInfoGroup metagroup = appendGroup( metainfo, "DrgeoContents");

  KFileMimeTypeInfo::ItemInfo* item;

  TQFile f( metainfo.path() );
  TQDomDocument doc( "drgenius" );
  if ( !doc.setContent( &f ) )
    return false;
  TQDomElement main = doc.documentElement();
  int numfig = 0;
  int numtext = 0;
  int nummacro = 0;
  TQString sectname;
  // reading figures...
  for ( TQDomNode n = main.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    TQDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    else if ( e.tagName() == "drgeo" )
    {
      numfig++;
      sectname = TQString( "Figure" ) + TQString::number( numfig );
      item = addItemInfo( group_contents, sectname, i18n( "Figure" ), TQVariant::String );
      appendItem( metagroup, sectname, e.attribute( "name" ) );
    }
    else if ( e.tagName() == "text" )
    {
      numtext++;
      sectname = TQString( "Text" ) + TQString::number( numtext );
      item = addItemInfo( group_contents, sectname, i18n( "Text" ), TQVariant::String );
      appendItem( metagroup, sectname, e.attribute( "name" ) );
    }
    else if ( e.tagName() == "macro" )
    {
      nummacro++;
      sectname = TQString( "Macro" ) + TQString::number( nummacro );
      item = addItemInfo( group_contents, sectname, i18n( "Macro" ), TQVariant::String );
      appendItem( metagroup, sectname, e.attribute( "name" ) );
    }
  }
  
  metagroup = appendGroup( metainfo, "DrgeoInfo");
  appendItem( metagroup, "NumOfFigures", numfig );
  appendItem( metagroup, "NumOfTexts", numtext );
  appendItem( metagroup, "NumOfMacros", nummacro );

  return true;
}

#include "kfile_drgeo.moc"

