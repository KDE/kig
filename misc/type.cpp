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
