#include "hierarchy.h"

#include "../kig/kig_part.h"

#include <map>

#include <kdebug.h>

#include "../objects/point.h"

ObjectHierarchy::ObjectHierarchy (const Objects& inGegObjs, 
				  const Objects& inFinalObjs, 
				  KigDocument* inDoc)
  : doc (inDoc)
{
  // here we construct the hierarchy
  // the hash map contains all HierarchyElement's we already constructed
  typedef map<Object*, HierarchyElement*> ElemHash;
  ElemHash elemHash;
  // a temporary
  HierarchyElement* elem;
  // first we add the given (dutch: gegeven, if you're wondering what
  // the geg in gegObjs stands for) objects
  Object* o;
  for (Objects::iterator i(inGegObjs); (o = i.current()); ++i)
    {
      double param = 0;
      ConstrainedPoint* cp;
      if ((cp = Object::toConstrainedPoint(o))) param = cp->getP();
      elem = new HierarchyElement(o->vBaseTypeName(), allElems.size() + 1, param);
      gegElems.push_back(elem);
      allElems.push_back(elem);
      elemHash[o] = elem;
    };
  // next: we do the final objects
  for (Objects::iterator i(inFinalObjs); (o = i.current()); ++i)
    {
      elem = new HierarchyElement(o->vFullTypeName(), allElems.size() + 1);
      finElems.push_back(elem);
      allElems.push_back(elem);
      elemHash[o] = elem;
    };

  // some temporary stuff i use
  Objects tmp = inFinalObjs;
  Objects tmp2, tmp3;
  ElemHash::iterator elem2;
  HierarchyElement* elem3;
  // tmp contains objects whose parents need to be handled
  while ( !(tmp.isEmpty()) )
  {
    // tmp2 is a temporary, used to contain objects which will form
    // tmp in the next round, if we encounter an object whose parents
    // need to be handled, we add it to tmp2, so that it will be
    // handled in the next round
    tmp2.clear();
    // we make a pass over all objects in tmp
    for (Objects::iterator i(tmp); (o = i.current()); ++i)
      {
	// *i should already be in the hash
	elem = elemHash.find(i)->second;
	// tmp3 are the parents of the object we're handling
	tmp3 = o->getParents();
	Object* j;
	for (Objects::iterator it(tmp3); (j = it.current()); ++it)
	  {
	    // elem2 is a map::iterator, we use it to find elem3,
	    // which is a HierarchyElement*
	    elem2 = elemHash.find(j);
	    if (elem2 == elemHash.end())
	      {
		elem3 = new HierarchyElement(j->vFullTypeName(), allElems.size() + 1);
		tmp2.add(j);
		allElems.add(elem3);
		elemHash[j] = elem3;
	      }
	    else elem3 = elem2->second;
	    elem->addParent(elem3);
	  };
      };
    tmp = tmp2;
  };
};

Objects ObjectHierarchy::fillUp( const Objects& inGegObjs ) const
{
  Objects cos;
  assert (gegElems.size() == inGegObjs.count());
  allElems.cleanActuals();
  ElemList::const_iterator elem = gegElems.begin();
  Objects::iterator it (inGegObjs);
  Object* obj;
  while (( obj = it.current()))
    {
      (*elem)->actual = obj;
      ++elem; ++it;
    };
  for (ElemList::const_iterator i = finElems.begin(); i != finElems.end(); ++i)
    {
      if ((*i)->getTypeName() == "ConstrainedPoint")
	{
	  (*i)->actual = new ConstrainedPoint((*i)->getParam());
	}
      else
	{
	  (*i)->actual = doc->newObject((*i)->getTypeName());
	};
      cos.add((*i)->actual);
    };
  // temporary's
  ElemList tmp = finElems;
  ElemList tmp2, tmp3;
  // tmp contains elems we're constructing the children of...
  while ( !tmp.empty() )
  {
    // these are the next objects we'll be making a pass over, they
    // will be copied onto tmp. tmp2 is only there to not disturb tmp
    // within a run.
    tmp2.clear();
    // we pass over all objects we're constructing the children of
    for ( ElemList::const_iterator i = tmp.begin(); i != tmp.end(); ++i)
      {
	// pass over all of the current object's children
	for ( ElemList::const_iterator j = (*i)->getParents().begin();
	      j != (*i)->getParents().end();
	      ++j)
	  {
	    // if we haven't constructed the object yet...
	    if (!(*j)->actual)
	      {
		// we construct it, and add it to the necessary places
		Object* appel = doc->newObject((*j)->getTypeName());
		assert(appel);
		(*j)->actual = appel;
		cos.add(appel);
		// we should also construct its parents
		tmp2.add(*j);
	      };

	    // select its parent as an arg...
	    (*i)->actual->selectArg((*j)->actual);
	    // we don't do addChild since that should be done by the
	    // child's selectArg() function
	  };
      };
    tmp = tmp2;
  };
  return cos;
}

void ElemList::deleteAll()
{
  for (iterator i = begin(); i != end(); ++i) delete *i;
}

void ElemList::cleanActuals() const
{
  for (const_iterator i = begin(); i != end(); ++i) (*i)->actual = 0;
}

void ObjectHierarchy::loadXML( QDomElement& ourElement)
{
  // clear everything...
  gegElems.clear();
  finElems.clear();
  allElems.deleteAll();
  allElems.clear();

  // a hash to know which elements we already constructed...
  typedef map<int, HierarchyElement*> Hash;
  Hash tmphash;

  // load data: we work like this.
  // for every "HierarchyElement" xml element, we look if the
  // corresponding HierarchyElement has already been constructed, and
  // if it hasn't, we construct it.  Then, we pass over all "parent"
  // child nodes of the node we're handling.  We check if they have
  // already been constructed, and if not, we construct them here.
  // Then, we call addParent.  So, an HierarchyElement gets its
  // parents when we find the main tag, but we can already have
  // constructed it before, as the parent of another.  Adding the
  // elements to allElems, gegElems and finElems is done when they get
  // their parents.
  for (QDomNode n = ourElement.firstChild(); !n.isNull(); n = n.nextSibling())
    {
      QDomElement e = n.toElement();
      assert (!e.isNull());
      assert (e.tagName() == "HierarchyElement");

      // fetch the id
      QString tmpId = e.attribute("id");
      bool ok;
      int id = tmpId.toInt(&ok);
      assert(ok);

      HierarchyElement* tmpE;
      // here, we look if we have constructed this one already, and if
      // not, we construct it.  tmpE should contain the correct
      // element after this.
      Hash::iterator i = tmphash.find(id);
      if ( i != tmphash.end() ) {
	// already constructed:
	tmpE = i->second;
      } else {
	// not constructed yet, so we construct it here...
	
	// fetch the typeName
	QString tmpTN = e.attribute("typeName");
	assert(tmpTN);
	QCString typeName = tmpTN.utf8();
	
	// fetch the param if it is a ConstrainedPoint
	if (tmpTN == "ConstrainedPoint")
	  {
	    QString tmpP = e.attribute("param");
	    assert (tmpP);
	    double param = tmpP.toDouble(&ok);
	    assert(ok);
	    tmpE = new HierarchyElement(typeName, id, param);
	  }
	else tmpE = new HierarchyElement(typeName,id);
	tmphash[id] = tmpE;
      };

      // now take care of the parents:
      for (QDomNode node = n.firstChild(); !node.isNull(); node = node.nextSibling())
	{
	  // fetch the element
	  QDomElement e = node.toElement();
	  assert (!e.isNull());
	  assert (e.tagName() == "parent");
	  
	  // fetch the id
	  QString tmpId = e.attribute("id");
	  bool ok;
	  int id = tmpId.toInt(&ok);
	  assert(ok);

	  // we look if we have already constructed this element, and
	  // if not, we construct it.
	  Hash::iterator i = tmphash.find(id);
	  if ( i != tmphash.end() ) {
	    // already constructed:
	    tmpE->addParent(i->second);
	  } else {
	    // not constructed yet, so we construct it here...
	    
	    // fetch the typeName
	    QString tmpTN = e.attribute("typeName");
	    assert(tmpTN);
	    QCString typeName = tmpTN.utf8();
	    HierarchyElement* tmp = new HierarchyElement(typeName,id);
	    tmpE->addParent(tmp);
	    tmphash[id] = tmp;
	  };
	};
      // next, we add tmpE to the relevant places:
      allElems.push_back(tmpE);

      QString tmpGiven = e.attribute("given");
      assert(tmpGiven);
      QCString tmpS = tmpGiven.utf8();
      if (tmpS == "true" || tmpS == "yes") gegElems.push_back(tmpE);

      QString tmpFinal = e.attribute("final");
      assert(tmpFinal);
      tmpS = tmpFinal.utf8();
      if (tmpS == "true" || tmpS == "yes") finElems.push_back(tmpE);
    };
};

void HierarchyElement::saveXML(QDomDocument& doc, QDomElement& p, bool
			       ref, bool given, bool final) const
{
  QDomElement e = doc.createElement(ref?"parent":"HierarchyElement");
  e.setAttribute("typeName", typeName);
  e.setAttribute("id", id);

  // if we're only writing a reference to ourselves, we don't need all
  // this information..
  if (!ref) {
    // whether we are given/final:
    e.setAttribute("given", given?"true":"false");
    e.setAttribute("final", final?"true":"false");
    if (typeName == "ConstrainedPoint") e.setAttribute ("param", param);
    // references to our parents
    for (ElemList::const_iterator i = parents.begin(); i != parents.end(); ++i)
      (*i)->saveXML(doc,e,true);
  };
  p.appendChild(e);
};

void ObjectHierarchy::saveXML( QDomDocument& doc, QDomElement& p ) const
{
//   // this is what we look like:
//   <ObjectHierarchy>
//     <HierarchyElement id="1" typename="appel" given="true" final="false"/>
//     <HierarchyElement id="2" typename="flap" given="false" final="true>
//       <parent id="1" typename="appel"/>
//     </HierarchyElement>
//   </ObjectHierarchy>
  QDomElement m = doc.createElement("ObjectHierarchy");
  // allElems
  for (ElemList::const_iterator i = allElems.begin(); i != allElems.end(); ++i) {
    (*i)->saveXML(doc, m, false, gegElems.contains(*i), finElems.contains(*i));
  };
  p.appendChild(m);
}

void ObjectHierarchy::calc()
{
  ElemList tmp = gegElems, tmp2;
  while (!tmp.empty())
    {
      for (ElemList::iterator i = tmp.begin(); i != tmp.end(); ++i)
	{
	  for (ElemList::const_iterator j = (*i)->getChildren().begin();
	       j != (*i)->getChildren().begin();
	       ++j)
	    {
	      (*j)->actual->calc();
	      tmp2.push_back(*j);
	    };
	};
      tmp = tmp2;
      tmp2.clear();
    };
}
