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


#include "type.h"

#include "hierarchy.h"

#include "../kig/constructactions.h"
#include "../kig/kig_part.h"
#include "../modes/constructing.h"
#include "../objects/macro.h"

#include <qregexp.h>
#include <qfile.h>
#include <qdom.h>

#include <kmessagebox.h>

MType::~MType()
{
  delete mhier;
}

MType::MType( ObjectHierarchy* hier, const QString name, const QString desc )
  : mhier( hier ),
    mname( name ),
    mdesc( desc )
{
}

Object* MType::build()
{
  return new MacroObjectOne( mhier );
}

void MType::saveXML( QDomDocument& doc, QDomNode& p ) const
{
  QDomElement e = doc.createElement("MType");
  e.setAttribute("name", mname);
  mhier->saveXML( doc, e );
  p.appendChild(e);
}

MType::MType( const QDomElement& e )
{
  assert(e.tagName() == "MType");
  QString tmpAN = e.attribute("name");
  assert(tmpAN);
  mname = tmpAN;
  QDomNode n = e.firstChild();
  QDomElement el = n.toElement();
  assert(!el.isNull());
  mhier = new ObjectHierarchy( el );
}

const QCString MType::fullName() const
{
  QString s = mname;
  s = s.replace( QRegExp( " " ), "_" );
  return s.utf8();
}

const QCString MType::baseTypeName() const
{
  return mhier->getFinElems().front()->type()->baseTypeName();
}

const QString MType::descriptiveName() const
{
  return mname;
}

const QString MType::description() const
{
  return mname;
}

const QCString MType::iconFileName() const
{
  // TODO ?
  return "";
}

KAction* Type::constructAction( KigDocument* d )
{
  KAction* a = new ConstructAction( d, this );
  mactions.push_back( a );
  return a;
}

KigMode* MType::constructMode( NormalMode* mode, KigDocument* doc )
{
  return new StdConstructionMode( build(), mode, doc );
}

void deleteAction( KAction* a )
{
  myvector<KigDocument*>& vect = KigDocument::documents();
  typedef myvector<KigDocument*>::iterator myit;
  for ( myit i = vect.begin(); i != vect.end(); ++i )
    (*i)->removeAction( a );
  delete a;
};

void Type::deleteActions()
{
  for_each( mactions.begin(), mactions.end(), &deleteAction );
}

const char* MType::actionName() const
{
  return "";
}
