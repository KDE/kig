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


#include "cabri.h"

#include <qcstring.h>
#include <qfile.h>
#include <qtextstream.h>

#include <string>

#include "../objects/object.h"
#include "../misc/hierarchy.h"

KigFilterCabri::KigFilterCabri()
{
    
}

KigFilterCabri::~KigFilterCabri()
{
    
}

bool KigFilterCabri::supportMime( const QString mime )
{
  if (mime == QString::fromLatin1( "application/x-cabri" ) )
    return true;
  return false;
}

struct ObjectData
{
  Object* o;
  std::vector<int> p;
  int id;
  bool valid;
  operator bool() { return valid; };
};

// this function reads a line, and converts all line delimiters
// ("\r\n" or "\n" to unix-style "\n").
QCString readLine( QFile& f, bool& eof )
{
  QCString s;
  char r;
  while( true )
    {
      r = f.getch();
      if( r == 0 || r == -1 ) { eof = true; break; };
      if( r == '\n' ) { eof = false; break; }
      if( r == '\r' )
	{
	  r = f.getch();
	  if( r == '\n' ) { eof = false; break; }
	  f.ungetch( r );
	  r = '\r';
	};
      s += r;
    };
  s += '\n';
  return s;
}


ObjectData readObject( QFile& f )
{
  ObjectData n;
  n.valid = false;
  bool eof;
  QCString l = readLine( f, eof );
  // n.valid == false, so this is cool...
  if( eof ) return n;

  // first a number and a colon to indicate the id:
  int colonPos = l.find(':');
  QCString idS = l.left( colonPos );
  bool ok = true;
  int id = idS.toInt( &ok );
  if( ! ok ) return n;
  n.id = id;

  // we skip the colon, and the space after it...
  l = l.mid( colonPos + 2 );

  // next the object Type..
  int commaPos = l.find(',');
  QCString idT = l.left( commaPos );

  // TODO...
  n.valid = true;
  return n;  
};

KigFilter::Result KigFilterCabri::convert( const QString from, KTempFile& to)
{
  // NOT READY...
  return NotSupported;
  // we fill up objs with the objects from the file, and then save
  // those objects...
  std::vector<ObjectData> objs;
  
  QFile f( from );
  f.open( IO_ReadOnly );

  // this code will look more like C than C++, but that's cause i
  // think parsing text is not something that C++ is good at...

  bool eof;

  QCString s = readLine( f, eof );
  QCString a = s.left( 21 );
  QCString b = s.mid( 22 );
  if( eof || ( a != "Figure CabriII vers. " ) || ( b != "DOS 1.0" && b != "MS-Windows 1.0" ) )
    return NotSupported;
  // next we have:
  // line 2: empty line
  // line 3: window dimensions -> we don't need/use that...
  // line 4: empty line
  // line 5 through 8: center point
  // line 9 through 12: axes
  // so we skip 11 lines...
  for( int i = 0; i != 11; ++i)
    (void) readLine( f, eof );

  // all Cabri files seem to at least have these center and axes...
  if( eof ) return NotSupported;

  // next, we get the objects we want...
  ObjectData o;
  // fetch the objects...
  while( ( o = readObject( f ) ) ) objs.push_back(o);
  // get the dependencies right...
  for( std::vector<ObjectData>::iterator i = objs.begin(); i != objs.end(); ++i )
    {
      for( std::vector<int>::iterator j = i->p.begin(); j != i->p.end(); ++j )
	{
	  i->o->addChild( objs[*j].o );
	};
    };

  // the output file...
  QFile* fTo = new QFile (to.name());
  fTo->open(IO_WriteOnly);

  // start building our file...
  QTextStream stream (fTo);

  // cf. KigDocument::saveFile()...
  QDomDocument doc ("KigDocument");
  QDomElement elem = doc.createElement ("KigDocument");
  elem.setAttribute( "Version", "2.0.000");

  Objects go;
  Objects fo;
  for (std::vector<ObjectData>::iterator i = objs.begin(); i != objs.end(); ++i)
    {
      fo.add(i->o);
    };
  ObjectHierarchy hier ( go, fo, 0);
  hier.calc();
  hier.saveXML ( doc, elem );
  doc.appendChild(elem);

  stream << doc.toCString() << endl;
  kdDebug() << k_funcinfo << endl << doc.toCString() << endl;
  fTo->close();
  delete fTo;
  return OK;
}


