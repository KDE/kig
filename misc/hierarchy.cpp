#include "hierarchy.h"

#include "../kig/kig_part.h"

#include <map>

#include <kdebug.h>

#include "../objects/point.h"

ObjectHierarchy::ObjectHierarchy(const Objects& inGegObjs, 
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
      elem = new HierarchyElement(o->vBaseTypeName(), allElems.size() + 1);
      elem->actual = o;
      elem->setParams(o->getParams());
      gegElems.push_back(elem);
      allElems.push_back(elem);
      elemHash[o]=elem;
    };
  // next: we do the final objects
  for (Objects::iterator i(inFinalObjs); (o = i.current()); ++i)
    {
      elem = new HierarchyElement(o->vFullTypeName(), allElems.size() + 1);
      elem->actual = o;
      elem->setParams(o->getParams());
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
		elem3->actual = j;
		elem3->setParams(j->getParams());
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
      // doesn't seem necessary, but you never know...
      (*elem)->setParams(obj->getParams());
      ++elem; ++it;
    };
  for (ElemList::const_iterator i = finElems.begin(); i != finElems.end(); ++i)
    {
      (*i)->actual = doc->newObject((*i)->getTypeName());
      (*i)->actual->setParams((*i)->getParams());
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
		Object* appel;
		appel = doc->newObject((*j)->getTypeName());
		assert(appel);
		appel->setParams((*j)->getParams());
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
  // an int-hash is a vector :)
  typedef vector<HierarchyElement*> Hash;
  Hash tmphash;

  // load data:
  // we pass over the dom hierarchy twice:
  // first we construct all the HierarchyElement, an then we pass each
  // object its parents...
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

      // fetch the typeName
      QString tmpTN = e.attribute("typeName");
      assert(tmpTN);
      QCString typeName = tmpTN.utf8();
      
      HierarchyElement* tmpE = new HierarchyElement(typeName, id);
      if( id > tmphash.size() ) tmphash.resize( id );
      tmphash[id - 1] = tmpE;

      allElems.push_back(tmpE);
    };
  
  // now take care of the parents and the params:
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

      assert( id <= tmphash.size() );
      HierarchyElement* tmpE = tmphash[id -1];

      // two params we handle:
      QString tmpGiven = e.attribute("given");
      assert(tmpGiven);
      QCString tmpS = tmpGiven.utf8();
      if (tmpS == "true" || tmpS == "yes") gegElems.push_back(tmpE);

      QString tmpFinal = e.attribute("final");
      assert(tmpFinal);
      tmpS = tmpFinal.utf8();
      if (tmpS == "true" || tmpS == "yes") finElems.push_back(tmpE);

      // the children of this tag:
      for (QDomNode node = n.firstChild(); !node.isNull(); node = node.nextSibling())
	{
	  // fetch the element of the child tag
	  QDomElement e = node.toElement();
	  assert (!e.isNull());
	  if (e.tagName() == "parent")
	    {
	      // fetch the id
	      QString tmpId = e.attribute("id");
	      bool ok;
	      int id = tmpId.toInt(&ok);
	      assert(ok);
	      
	      HierarchyElement* i = tmphash[id -1];
	      tmpE->addParent(i);
	    } // e.tagName() == "parent"
	  else
	    { // it's not a parent, so it's a param..
	      assert (e.tagName() == "param");
	      QString name = e.attribute("name");
	      QDomText t = e.firstChild().toText();
	      Q_ASSERT (!t.isNull());
	      bool ok;
	      double value = t.data().toDouble(&ok);
	      Q_ASSERT(ok);
	      tmpE->setParam(name.latin1(), value);
	    }; // we just handled the param child tag
	}; // end of loop over the child tags of the HierarchyElement
    };
};

void HierarchyElement::saveXML(QDomDocument& doc, QDomElement& p, bool
			       ref, bool given, bool final) const
{
  QDomElement e = doc.createElement(ref?"parent":"HierarchyElement");
  e.setAttribute("id", id);

  // if we're only writing a reference to ourselves, we don't need all
  // this information..
  if (!ref) {
    // our typename
    e.setAttribute("typeName", typeName);

    // whether we are given/final:
    e.setAttribute("given", given?"true":"false");
    e.setAttribute("final", final?"true":"false");
    // references to our parents
    for (ElemList::const_iterator i = parents.begin(); i != parents.end(); ++i)
      (*i)->saveXML(doc,e,true);
    // save our params
    for (pMap::const_iterator i = params.begin(); i != params.end(); ++i)
      {
	QDomElement p = doc.createElement("param");
	p.setAttribute("name", i->first);
	QDomText t = doc.createTextNode(QString::number(i->second));
	p.appendChild(t);
	e.appendChild(p);
      };
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
	       j != (*i)->getChildren().end();
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
