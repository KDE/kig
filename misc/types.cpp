/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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

#include "type.h"
#include "../objects/object.h"
#include "objects.h"
#include "../kig/kig_part.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qdom.h>

Object* Types::buildObject( const QCString& type,
                            const Objects& parents,
                            const Type::ParamMap& params ) const
{
  Type* b = findType( type );
  if ( b ) return b->build( parents, params );
  else return 0;
}

Type* Types::findType(const QCString& type) const
{
  const_iterator i = find( type );
  if ( i == end() ) return 0;
  else return i->second;
}

void Types::saveToDir(const QString dir_name) const
{
  // one file per type
  for ( const_iterator i = begin(); i != end(); ++i )
  {
    MType* appel = dynamic_cast<MType*>( i->second );
    // we only save user defined types...
    if (!appel) continue;
    // find the file name
    QString filename = dir_name + QString::fromLatin1(
      QFile::encodeName(i->second->fullName()) ) + ".kigt";
    QFile file (filename);
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

Types::Types( const QString& file_name)
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
    if (e.tagName() != "MType")
    {
      kdWarning() << "weird tag in file: " << e.tagName() << "skipping..." << endl;
      continue;
    };
    addType( new MType ( e ) );
  };
}

void Types::saveToFile( const QString filename ) const
{
  QFile file (filename);
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
  // all types in one file...
  QDomElement e = doc.createElement("Types");
  for ( const_iterator i = begin(); i != end(); ++i )
    i->second->saveXML(doc, e);
  // throw the xml file in the stream
  doc.appendChild(e);
  stream << doc.toCString();
  // close the file...
  file.close();
}

void Types::addType( Type* t )
{
  insert( value_type( t->fullName(), t ) );
}

Types::~Types()
{
  for ( iterator i = begin(); i != end(); ++i )
    delete i->second;
}

void Types::addTypes( Types& i )
{
  insert( i.begin(), i.end() );
  i.clear();
}

void Types::removeType( Type* t )
{
  for ( iterator i = begin(); i != end(); ++i )
    if ( i->second == t )
    {
      erase( i );
      break;
    };
}

std::vector<Type*> Types::whoWantsArgs( const Objects& args, bool completeOnly ) const
{
  std::vector<Type*> ret;
  for ( const_iterator i = begin(); i != end(); ++i )
  {
    StdConstructibleType* t = i->second->toStdConstructible();
    if ( t )
    {
      switch ( t->wantArgs( args ) )
      {
      case Object::Complete:
        ret.push_back( t );
        break;
      case Object::NotComplete:
        if ( ! completeOnly ) ret.push_back( t );
        break;
      default:
        break;
      };
    };
  };
  return ret;
}
