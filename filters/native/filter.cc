// filter.cc
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

#include "filter.h"

#include "../../misc/hierarchy.h"
#include "../../misc/objects.h"
#include "../../misc/coordinate.h"
#include "../../objects/normalpoint.h"

#include <qfile.h>
#include <qdom.h>
#include <qtextstream.h>

KigFilterNative::KigFilterNative()
{
};

KigFilterNative::~KigFilterNative()
{
}

bool KigFilterNative::supportMime( const QString mime )
{
  if (mime == QString::fromLatin1 ("application/x-kig")) return true;
  else return false;
}

KigFilter::Result KigFilterNative::load( const QString from, Objects& os )
{
  QFile file( from );
  if (!file.open(IO_ReadOnly))
  {
    return FileNotFound;
  };
  // TODO: more error detection
  QDomDocument doc ("KigDocument");
  doc.setContent(&file);
  QDomElement main = doc.documentElement();

  // first the "independent" points, this is no longer necessary, but
  // we keep it for backwards compatibility...
  QDomNode n;
  QDomElement e;
  for ( n = main.firstChild(); !n.isNull(); n=n.nextSibling())
  {
    e = n.toElement();
    assert (!e.isNull());
    if (e.tagName() == "ObjectHierarchy") break;
    // TODO: make this more generic somehow, error detection
    // UPDATE: this has been made more generic via
    // Object::getParam/setParam and the corresponding code in
    // misc/hierarchy.cpp.  It is kept here for the holy Backward
    // Compatibility...
    bool ok;
    if (e.tagName() == "Point")
      os.push_back(
        NormalPoint::fixedPoint(
          Coordinate(
            e.attribute("x").toInt(&ok),
            e.attribute("y").toInt(&ok) ) ) );
    if ( ! ok ) return ParseError;
  };

  // next the hierarchy...
  ObjectHierarchy hier( e );
  Objects tmpOs = hier.fillUp( os );
  hier.calc();
  os |= tmpOs;
  file.close();

  return OK;
}

KigFilter::Result KigFilterNative::save( const Objects& os, const QString to )
{
  QFile file(to);
  if (file.open(IO_WriteOnly) == false)
  {
    // TODO: need to find some proper error enum values...
    return FileNotFound;
  };

  QTextStream stream(&file);

  QDomDocument doc("KigDocument");
  QDomElement elem = doc.createElement( "KigDocument" );
  elem.setAttribute( "Version", "0.5" );

  // saving is done very easily:
  // we create an ObjectHierarchy with no "given" objects, and all
  // objects as "final" objects...
  Objects given;
  ObjectHierarchy hier( given, os );
  hier.saveXML( doc, elem );
  doc.appendChild(elem);

  stream << doc.toCString();
  file.close();

  return OK;
}
