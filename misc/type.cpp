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

#include "../kig/kig_part.h"
#include "../objects/macro.h"

#include <kdebug.h>

#include <qfile.h>
#include <qtextstream.h>

void SlotWrapper::newObc()
{
    doc->newObc( t->newObject() );
};

MTypeOne::MTypeOne( ObjectHierarchy* inHier, const QString& inActionName, const QString& inDesc, KigDocument* inDoc )
  : MType(inHier, inDoc ), 
    finalType(inDoc->getTypes()->findType(inHier->getFinElems()[0]->getTypeName())),
    actionName(inActionName),
    description(inDesc)
{    
}

MType::MType( ObjectHierarchy* inHier, KigDocument* inDoc)
  : Type(inDoc), hier(inHier)
{
}

Object* MTypeOne::newObject()
{
  Object* o = new MacroObjectOne(hier);
  kdDebug() << k_funcinfo << o << endl;
  return o;
}

void MTypeOne::saveXML( QDomDocument& doc, QDomNode& p) const
{
  QDomElement e = doc.createElement("MTypeOne");
  e.setAttribute("actionName", actionName);
  kdDebug() << k_funcinfo << " at line no. " << __LINE__ << endl;
  hier->saveXML(doc,e);
  kdDebug() << k_funcinfo << " at line no. " << __LINE__ << endl;
  p.appendChild(e);
};

MTypeOne::MTypeOne(const QDomElement& e, KigDocument* kdoc)
  : MType(0,kdoc )
{
  assert(e.tagName() == "MTypeOne");
  QString tmpAN = e.attribute("actionName");
  assert(tmpAN);
  actionName=tmpAN;
  QDomNode n = e.firstChild();
  QDomElement el = n.toElement();
  assert(!el.isNull());
  hier = new ObjectHierarchy(el, kdoc);
  finalType = kdoc->getTypes()->findType(hier->getFinElems()[0]->getTypeName());
};

MTypeOne* Type::toMTypeOne()
{
  return dynamic_cast<MTypeOne*>(this);
};
