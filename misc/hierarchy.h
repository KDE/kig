#ifndef HIERARCHY_H
#define HIERARCHY_H

#include "../objects/object.h"

#include <list>
#include <vector>
#include <algorithm>

#include <qcstring.h>

class KigDocument;
class HierarchyElement;
class ObjectHierarchy;

// the stl hash function is solely composed of template
// specializations, and wasn't so foreseeing to include one that works
// on Object pointers, this one does.
// see also the stl documentation (e.g. the sgi stl docs, which are great)
struct myHash
{
  size_t operator()(const Object* o) const { return size_t(o); };
};

class ElemList
  : public vector<HierarchyElement*>
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
protected:
  QCString typeName;
  ElemList parents;
  ElemList children;
  int id;
//   QString description;
public:
  HierarchyElement(QCString inTN, 
// 		   QString description,
		   int inId
		   ) : typeName(inTN), id(inId), actual(0) {};
  void addParent ( HierarchyElement* e ) { parents.push_back(e); e->addChild(this);};
  void addChild (HierarchyElement* e ) { children.push_back(e); };
  void saveXML ( QDomDocument& d, QDomElement& parentElem,
		 bool reference, bool given=false,
		 bool final=false) const;
  int getId() { return id; };
  QCString getTypeName() { return typeName;};
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
  KigDocument* doc;
public:
  // this constructs the hierarchy, so that its internal structure
  // represents the structure of its arguments, so that fillUp will be
  // able to do something intelligent.
  // the HierarchyElement::actual's will contain pointers to the
  // objects in the structure of the arguments, so calc() can do
  // something intelligent too...
  ObjectHierarchy 
  (const Objects& inGegObjs, 
   const Objects& inFinalObjs, 
   KigDocument* inDoc);
  ObjectHierarchy ( QDomElement& ourElement, KigDocument* inDoc ) : doc(inDoc) { loadXML(ourElement); };
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
