// internal_types.cc
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

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

#include "internal_types.h"

#include "hierarchy.h"

#include "../objects/macro.h"

#include <qregexp.h>
#include <qfile.h>
#include <qdom.h>

#include <kmessagebox.h>
void InternalTypes::addType( TypeRepresentant* t )
{
  mmap[t->fullName()] = t;
}

Object* InternalTypes::buildObject( const QCString& type )
{
  TypeRepresentant* b = findType( type );
  if ( b ) return b->build();
  else return 0;
}

InternalTypes::~InternalTypes()
{
  for ( imap::iterator i = mmap.begin(); i != mmap.end(); ++i )
  {
    delete i->second;
  };
}

MTypeRepresentant::~MTypeRepresentant()
{
  delete mhier;
}

MTypeRepresentant::MTypeRepresentant( ObjectHierarchy* hier,
                                      const QString name, const QString desc )
  : mhier( hier ),
    mname( name ),
    mdesc( desc )
{

}

Object* MTypeRepresentant::build()
{
  return new MacroObjectOne( mhier );
}

void MTypeRepresentant::saveXML( QDomDocument& doc, QDomNode& p ) const
{
  QDomElement e = doc.createElement("MTypeRepresentant");
  e.setAttribute("name", mname);
  mhier->saveXML( doc, e );
  p.appendChild(e);
}

MTypeRepresentant::MTypeRepresentant( const QDomElement& e )
{
  assert(e.tagName() == "MTypeRepresentant");
  QString tmpAN = e.attribute("name");
  assert(tmpAN);
  mname = tmpAN;
  QDomNode n = e.firstChild();
  QDomElement el = n.toElement();
  assert(!el.isNull());
  mhier = new ObjectHierarchy( el );
}

const QCString MTypeRepresentant::fullName() const
{
  QString s = mname;
  s = s.replace( QRegExp( " " ), "_" );
  return s.utf8();
}

const QCString MTypeRepresentant::baseTypeName() const
{
  return mhier->getFinElems().front()->typeRepresentant()->baseTypeName();
}

const QString MTypeRepresentant::descriptiveName() const
{
  return mname;
}

const QString MTypeRepresentant::description() const
{
  return mname;
}

const QCString MTypeRepresentant::iconFileName() const
{
  // TODO ?
  return "";
}

InternalTypes::InternalTypes( const QString& file_name )
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
      if (e.tagName() != "MTypeRepresentant")
	{
	  kdWarning() << "weird tag in file: " << e.tagName() << "skipping..." << endl;
	  continue;
	};
      addType( new MTypeRepresentant ( e ) );
    };
}

void InternalTypes::addTypes( InternalTypes& i )
{
  mmap.insert( i.mmap.begin(), i.mmap.end() );
  i.mmap.clear();
}

TypeRepresentant* InternalTypes::findType( const QCString& t )
{
  imap::iterator i = mmap.find( t );
  if ( i == mmap.end() ) return 0;
  else return i->second;
}

void InternalTypes::saveToDir( const QString dir_name )
{
  // one file per type
  for (imap::iterator i = mmap.begin(); i != mmap.end(); ++i)
    {
      MTypeRepresentant* appel = dynamic_cast<MTypeRepresentant*>( i->second );
      // we only save user defined types...
      if (!appel) continue;
      // find the file name
      QString filename = dir_name + QString::fromLatin1( QFile::encodeName(i->second->fullName()) ) + ".kigt";
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

void InternalTypes::saveToFile( const QString filename )
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
  for (imap::iterator i = mmap.begin(); i != mmap.end(); ++i)
  {
    MTypeRepresentant* appel = dynamic_cast<MTypeRepresentant*>( i->second );
    // we only save user defined types...
    if (!appel) continue;

    // save the macro type
    appel->saveXML(doc, e);
  };
  // throw the xml file in the stream
  doc.appendChild(e);
  stream << doc.toCString();
  // close the file...
  file.close();
}

void InternalTypes::removeType( TypeRepresentant* t )
{
  for ( iterator i = begin(); i != end(); ++i )
  {
    if ( i->second == t )
    {
      mmap.erase( i );
      break;
    };
  };
}
