/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/


#include "types.h"

#include <kdebug.h>
#include <kmessagebox.h>

#include <qfile.h>
#include <qtextstream.h>

Object* Types::newObject( const QCString& type )
{
  Type* theType = findType(type);
  if (theType) return theType->newObject();
  else return 0;
}

Type* Types::findType(const QCString& type) const
{
  for ( const_iterator i = begin(); i != end(); ++i )
    if ( type == ( *i )->fullTypeName() )
      return ( *i );
  kdError() << "Types::newObject: no TypeRepr found for typename \"" << type << "\"." << endl;
  return 0;
}
void Types::deleteAll()
{
  for ( iterator i = begin(); i != end();++i ) delete *i;
}

void Types::saveToDir(const QString& dir_name)
{
  // one file per type
  for (iterator i = begin(); i != end(); ++i)
    {
      MTypeOne* appel = (*i)->toMTypeOne();
      // we only save user defined types...
      if (!appel) continue;
      // find the file name
      QString filename = dir_name + QString::fromLatin1((*i)->fullTypeName()) + ".kigt";
      QFile file (QFile::encodeName(filename));
      // open the file
      if (!file.open(IO_WriteOnly))
      {
	KMessageBox::sorry
	  (0,
	   i18n("Unable to open file %1 for writing...").arg(filename),
	   i18n("Unable to Open File"));
	return;
      }
      // get a stream
      QTextStream stream(&file);
      // start a xml file
      QDomDocument doc ("KigMacroFile");
      QDomElement e = doc.createElement("Types");
      // save the macro type
      appel->saveXML(doc, e);
      // throw the xml file in the stream
      doc.appendChild(e);
      stream << doc.toCString();
      // close the file...
      file.close();
    };
}

Types::Types( KigDocument* doc, const QString& file_name)
{
  QFile file(file_name);
  if (!file.open(IO_ReadOnly)) {
    KMessageBox::sorry
      (0,
       i18n("Unable to open file %1 for reading...").arg(file_name),
       i18n("Unable to Open File"));
    return;
  };
  QDomDocument qdoc;
  qdoc.setContent(&file);
  QDomElement el = qdoc.documentElement();
  for (QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
    {
      QDomElement e = n.toElement();
      if (e.isNull()) continue;
      if (e.tagName() != "MTypeOne")
	{
	  kdWarning() << "weird tag in file: " << e.tagName() << "skipping..." << endl;
	  continue;
	};
      add(new MTypeOne ( e, doc ) );
    };
}
void Types::saveToFile(const QString& filename)
{
  QFile file (QFile::encodeName(filename));
  if (!file.open(IO_WriteOnly))
    {
      KMessageBox::sorry
	(0,
	 i18n("Unable to open file %1 for writing...").arg(filename),
	 i18n("Unable to Open File"));
      return;
    }
  // get a stream
  QTextStream stream(&file);
  // start a xml file
  QDomDocument doc ("KigMacroFile");
  QDomElement e = doc.createElement("Types");
  for (iterator i = begin(); i != end(); ++i)
    {
      MTypeOne* appel = (*i)->toMTypeOne();
      if (!appel) continue;
      appel->saveXML(doc, e);
    };
  doc.appendChild(e);
  // throw the xml file in the stream
  stream << doc.toCString();
  // close the file...
  file.close();
  
}
