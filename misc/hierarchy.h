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

#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <list>
#include <vector>
#include <algorithm>

#include <qcstring.h>

#include "../objects/object.h"
#include "objects.h"

class KigDocument;
class HierarchyElement;
class ObjectHierarchy;
class ScreenInfo;
class Type;
class QDomElement;
class QDomDocument;

class ElemList
  : public myvector<HierarchyElement*>
{
public:
  // sets all HierarchyElement::actual 's to 0
  void clean() const;
  Objects actuals();
};

class HierarchyElement
{
protected:
  typedef std::map<QCString,QString> pMap;
  Type* mtype;
  // TODO ? remove mid ?
  uint mid;
  pMap mparams;
  // these two are temporary variables used on filling up the
  // hierarchy.  What we do is: mcpc contains the number of parents
  // whose mactual has already been built or set.  We only build our
  // mactual if ( mcpc == mparents.size() ), and when we do, we inform
  // our children that this has been done by increasing their mcpc...
  Object* mactual;
  uint mcpc;
  ElemList mparents;
  ElemList mchildren;
  void addChild (HierarchyElement* e );
  friend class ElemList;
public:
  HierarchyElement( Object* rep, int id );
  HierarchyElement( const QDomElement& e );
  HierarchyElement( const QCString type, uint id );
  // see the explanation of mactual and mcpc above...
  bool tryBuild();
  void setActual( Object* a );

  Object* actual();
  void setParam( const QCString name, const QString value );
  void addParent( HierarchyElement* e );
  void saveXML( QDomDocument& d, QDomElement& parentElem,
                bool reference, bool given=false,
                bool final=false) const;
  QCString fullTypeName() const;
  QCString baseTypeName() const;
  uint id() const;
  const ElemList& parents();
  const ElemList& children();
};

/** an ObjectHierarchy keeps a structure of objects, and their
 * interrelationships
 * it is used in loading and saving, and for MacroObjects
 * the Locus class uses this class too...
 */
class ObjectHierarchy
{
  ElemList allElems;
  ElemList gegElems;
  ElemList finElems;
public:
  // this constructs the hierarchy, so that its internal structure
  // represents the structure of its arguments, so that fillUp will be
  // able to do something intelligent.
  ObjectHierarchy( const Objects& inGegObjs, const Objects& inFinalObjs );
  ObjectHierarchy( QDomElement& ourElement );
  // this is only used by the input filters.  They construct their
  // ElemList themselves, and then only use ObjectHierarchy to
  // fillUp()..
  ObjectHierarchy( const ElemList& allElems );
  const ElemList& getGegElems() { return gegElems; };
  const ElemList& getFinElems() { return finElems; };
  const ElemList& getAllElems() { return allElems; };

  /**
   * Constructs from the inGegObjs other objects, according
   * to the hierarchy it keeps.  All newly created objects are
   * returned.
   * NOTE: you _must_ still call calc() on all of the objects after
   * you call this function...
   */
  Objects fillUp( const Objects& inGegObjs ) const;

  void saveXML( QDomDocument& doc, QDomElement& parent ) const;
  void loadXML( QDomElement& ourElement);
};

#endif
