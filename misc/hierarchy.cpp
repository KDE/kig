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

#include "hierarchy.h"

#include "types.h"
#include "type.h"

#include <map>

#include <kdebug.h>
#include <qdom.h>

#include "../objects/point.h"

Objects ElemList::actuals()
{
  Objects tmp;
  for ( iterator i = begin(); i != end(); ++i )
  {
    tmp.push_back( (*i)->mactual );
  };
  return tmp;
}

void ElemList::clean() const
{
  for( const_iterator i = begin(); i != end(); ++i )
  {
    (*i)->mactual = 0;
    (*i)->mcpc = 0;
  };
}

ObjectHierarchy::ObjectHierarchy( QDomElement& ourElement )
{
  loadXML(ourElement);
}

ObjectHierarchy::ObjectHierarchy(const Objects& inGegObjs,
				 const Objects& inFinalObjs )
{
  // here we construct the hierarchy
  // the hash map contains all HierarchyElement's we already constructed
  typedef std::map<Object*, HierarchyElement*> ElemHash;
  ElemHash elemHash;
  // a temporary
  HierarchyElement* elem;
  // first we add the given (dutch: gegeven, if you're wondering what
  // the geg in gegObjs stands for) objects
  for( Objects::const_iterator i = inGegObjs.begin(); i != inGegObjs.end(); ++i )
  {
    elem = new HierarchyElement( *i, allElems.size() + 1 );
    gegElems.push_back(elem);
    allElems.push_back(elem);
    elemHash[*i]=elem;
  };
  // next: we do the final objects
  for( Objects::const_iterator i = inFinalObjs.begin(); i != inFinalObjs.end(); ++i )
  {
    elem = new HierarchyElement( *i, allElems.size() + 1 );
    finElems.push_back(elem);
    allElems.push_back(elem);
    elemHash[*i] = elem;
  };

  Objects tmp = inFinalObjs;
  Objects tmp2;
  ElemHash::iterator elem2;
  HierarchyElement* elem3;
  // tmp contains objects whose parents need to be handled
  while ( !(tmp.empty()) )
  {
    // tmp2 is a temporary, used to contain objects which will form
    // tmp in the next round, if we encounter an object whose parents
    // need to be handled, we add it to tmp2
    tmp2.clear();
    // we make a pass over all objects in tmp
    for( Objects::iterator i = tmp.begin(); i != tmp.end(); ++i )
    {
      // *i should already be in the hash
      elem = elemHash.find(*i)->second;
      // tmp3 are the parents of the object we're handling
      Objects tmp3 = (*i)->getParents();
      for (Objects::iterator j = tmp3.begin(); j != tmp3.end(); ++j )
      {
        // elem2 is a map::iterator, we use it to find elem3,
        // which is a HierarchyElement*
        elem2 = elemHash.find( *j );
        if (elem2 == elemHash.end())
        {
          elem3 = new HierarchyElement( *j, allElems.size() + 1);
          tmp2.upush(*j);
          allElems.push_back(elem3);
          elemHash[*j] = elem3;
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
  // init our return value
  Objects cos;

  // cleanups
  assert (gegElems.size() == inGegObjs.size());
  allElems.clean();

  // init our gegElems...
  ElemList::const_iterator elem = gegElems.begin();
  for( Objects::const_iterator i = inGegObjs.begin();
       i != inGegObjs.end(); ++i, ++elem )
    (*elem)->setActual( *i );
  // we need some independent elements to start constructing from.  We
  // can use the given elems of course, but in some situations, there
  // are no given elems, and then we need to find some independent
  // elements..
  ElemList indElems = gegElems;
  for ( ElemList::const_iterator i = allElems.begin(); i != allElems.end(); ++i )
  {
    if ( (*i)->parents().empty() && ! gegElems.contains( *i ) )
    {
      assert((*i)->tryBuild());
      cos.push_back( (*i)->actual() );
      indElems.push_back( *i );
    };
  };

  ElemList tmp = indElems, tmp2;
  // tmp contains elems we're constructing the children of...
  while ( !tmp.empty() )
  {
    // these are the next objects we'll be making a pass over, they
    // will be copied onto tmp at the end of this loop... tmp2 is only
    // there to not disturb tmp within a run.
    tmp2.clear();
    // we iterate over tmp...
    for ( ElemList::iterator i = tmp.begin(); i != tmp.end(); ++i)
    {
      // pass over all of the current object's children
      for ( ElemList::const_iterator j = (*i)->children().begin();
            j != (*i)->children().end(); ++j )
      {
        // we try to construct the object.. this only works if all of
        // its parents have been constructed already..
        if ((*j)->tryBuild())
        {
          cos.push_back((*j)->actual());
          // we now handle its children..
          tmp2.push_back(*j);
        }; // if( tryBuild() )
      }; // loop over (*i)->children()
    }; // loop over tmp
    tmp = tmp2;
  }; // while ( ! tmp.empty() );
  return cos;
}

void ObjectHierarchy::loadXML( QDomElement& ourElement)
{
  // clear everything...
  gegElems.clear();
  finElems.clear();
  delete_all( allElems.begin(), allElems.end() );
  allElems.clear();

  // std::vector containing the elements we already constructed.  ( the
  // elements are in the order of their id()'s...
  std::vector<HierarchyElement*> tac;

  // load data:
  // we pass over the dom hierarchy twice:
  // first we construct all the HierarchyElements, an then we pass each
  // object its parents...
  for (QDomNode n = ourElement.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    QDomElement e = n.toElement();

    HierarchyElement* tmpE = new HierarchyElement( e );
    // static cast, to prevent warnings about "comparison between
    // signed and unsigned..."
    // yes, the numbers are correct, there are no off-by-ones here..
    if( tmpE->id() > tac.size() ) tac.resize( tmpE->id() );
    tac[tmpE->id() - 1] = tmpE;

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
    uint id = tmpId.toInt(&ok);
    assert(ok);

    // find the HierarchyElement..
    assert( id <= tac.size() );
    HierarchyElement* tmpE = tac[id -1];
    assert( id == tmpE->id() );

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

        HierarchyElement* i = tac[id -1];
        tmpE->addParent(i);
      } // e.tagName() == "parent"
      else
      { // it's not a parent, so it's a param..
        assert (e.tagName() == "param");
        QString name = e.attribute("name");
        QDomText t = e.firstChild().toText();
        Q_ASSERT (!t.isNull());
        tmpE->setParam(name.latin1(), t.data());
      }; // we just handled the param child tag
    }; // end of loop over the child tags of the HierarchyElement
  };
};

void HierarchyElement::saveXML(QDomDocument& doc, QDomElement& p, bool
			       ref, bool given, bool final) const
{
  QDomElement e = doc.createElement(ref?"parent":"HierarchyElement");
  e.setAttribute("id", mid);
  // if we're only writing a reference to ourselves, this is all the
  // info we need...
  if ( ! ref )
  {
    // our typename
    e.setAttribute("typeName", fullTypeName() );
    // are we given/final ?
    e.setAttribute("given", given?"true":"false");
    e.setAttribute("final", final?"true":"false");
    // given objects need not know their parents or params..
    if ( ! given )
    {
      // references to our parents
      for( ElemList::const_iterator i = mparents.begin();
           i != mparents.end(); ++i )
        (*i)->saveXML(doc,e,true);
      // save our params
      for( pMap::const_iterator i = mparams.begin(); i != mparams.end(); ++i )
      {
        QDomElement p = doc.createElement("param");
        p.setAttribute("name", i->first);
        QDomText t = doc.createTextNode(i->second);
        p.appendChild(t);
        e.appendChild(p);
      };
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

HierarchyElement::HierarchyElement( Object* rep, int id )
  : mtype( Object::types().findType( rep->vFullTypeName() ) ),
    mid( id ), mparams( rep->getParams() ), mactual( rep ), mcpc( 0 )
{
  assert( mtype );
  assert( mactual );
}

void HierarchyElement::setParam( const QCString name, const QString value )
{
  mparams[name] = value;
}

void HierarchyElement::addParent( HierarchyElement* e )
{
  assert( ! mparents.contains( e ) );
  mparents.push_back(e);
  e->addChild(this);
}

uint HierarchyElement::id() const
{
  return mid;
}

const ElemList& HierarchyElement::parents()
{
  return mparents;
}

const ElemList& HierarchyElement::children()
{
  return mchildren;
}

QCString HierarchyElement::fullTypeName() const
{
  return mtype->fullName();
}

QCString HierarchyElement::baseTypeName() const
{
  return mtype->baseTypeName();
};

void HierarchyElement::addChild(HierarchyElement* e )
{
  mchildren.push_back( e );
}

bool HierarchyElement::tryBuild()
{
  if ( mactual ) return false;
  if ( mcpc == mparents.size() )
  {
    Object* o = mtype->build( mparents.actuals(), mparams );
    assert( o );
    setActual( o );
    return true;
  };
  return false;
}

void HierarchyElement::setActual( Object* a )
{
  assert( ! mactual );
  mactual = a;
  for ( ElemList::iterator i = mchildren.begin(); i != mchildren.end(); ++i )
    ++((*i)->mcpc);
}
Object* HierarchyElement::actual()
{
  return mactual;
}

HierarchyElement::HierarchyElement( const QDomElement& e )
  : mactual( 0 ), mcpc( 0 )
{
  assert (!e.isNull());
  assert (e.tagName() == "HierarchyElement");

  // fetch the typeName
  QString tmpTN = e.attribute("typeName");
  assert(tmpTN);
  QCString typeName = tmpTN.utf8();

  mtype = Object::types().findType( typeName );
  assert( mtype );

  // fetch the id
  QString tmpId = e.attribute("id");
  bool ok;
  mid = tmpId.toInt(&ok);
  assert(ok);
};

HierarchyElement::HierarchyElement( const QCString type, uint id )
  : mtype( Object::types().findType( type ) ), mid( id ), mactual( 0 ),
    mcpc( 0 )
{
}

ObjectHierarchy::ObjectHierarchy( const ElemList& all )
  : allElems( all ), finElems( all )
{
}
