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


#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <list>
#include <vector>
#include <algorithm>

using namespace std;

#include <qcstring.h>

#include "../objects/object.h"

class KigDocument;
class HierarchyElement;
class ObjectHierarchy;
class Type;

class ElemList
  : public std::vector<HierarchyElement*>
{
public:
  void add (HierarchyElement* e)
  {
    if (find (begin(), end(), e) == end())
      push_back(e);
  };
  bool contains(const HierarchyElement* h) const
  {
    return find(begin(),end(),h) != end();
  };
  // sets all HierarchyElement::actual 's to 0
  void cleanActuals() const;
  void deleteAll();
};

class HierarchyElement
{
  typedef map<QCString,QString> pMap;
protected:
  Type* mtype;
  ElemList parents;
  ElemList children;
  pMap params;
  int id;
public:
  const Type* type() { return mtype; };
  void setParam( const QCString name, const QString value ) { params[name] = value; };
  void setParams(const pMap& p ) { params = p; };
  const pMap getParams() { return params; };
  HierarchyElement(QCString inTN, int inId );
  void addParent ( HierarchyElement* e ) { parents.push_back(e); e->addChild(this);};
  void addChild (HierarchyElement* e ) { children.push_back(e); };
  void saveXML ( QDomDocument& d, QDomElement& parentElem,
		 bool reference, bool given=false,
		 bool final=false) const;
  int getId() { return id; };
  QCString getTypeName() const;
  const ElemList& getParents() { return parents; };
  const ElemList& getChildren() { return children; };
  Object* actual;
};

// an ObjectHierarchy keeps a structure of objects, and their
// interrelationships
// it is used in loading and saving, and for MacroObjects
// the Locus class uses this class too...
class ObjectHierarchy
{
  ElemList allElems;
  ElemList gegElems;
  ElemList finElems;
public:
  // this constructs the hierarchy, so that its internal structure
  // represents the structure of its arguments, so that fillUp will be
  // able to do something intelligent.
  // the HierarchyElement::actual's will contain pointers to the
  // objects in the structure of the arguments, so calc() can do
  // something intelligent too...
  ObjectHierarchy( const Objects& inGegObjs, const Objects& inFinalObjs );
  ObjectHierarchy ( QDomElement& ourElement ) { loadXML(ourElement); };
  const ElemList& getGegElems() { return gegElems; };
  const ElemList& getFinElems() { return finElems; };
  const ElemList& getAllElems() { return allElems; };

  // fillUp() constructs from the inGegObjs other objects, according
  // to the hierarchy it keeps.  All newly created objects are
  // returned.
  // the HierarchyElement::actual's will contain pointers to the
  // relevant newly created objects, so calc() will be able to do
  // something useful...
  Objects fillUp( const Objects& inGegObjs ) const;

  // calls Object::calc() on all contained objects in an intelligent
  // order (given objs first, since the others depend on their values...)
  void calc();

  void saveXML( QDomDocument& doc, QDomElement& parent ) const;
  void loadXML( QDomElement& ourElement);
};

#endif
