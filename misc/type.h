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


#ifndef TYPEREPR_H
#define TYPEREPR_H

#include "hierarchy.h"

#include <klocale.h>

#include <qobject.h>

class KigDocument;
class TypeRepository;
class Object;
class MTypeOne;
class Type;
class KAction;

// @see Type
class SlotWrapper
  :public QObject
{
  Q_OBJECT
public:
  SlotWrapper( KigDocument* inDoc, Type* inT ) : doc( inDoc ), t( inT ) {};
public slots:
  void newObc();
protected:
  KigDocument* doc;
  Type* t;
};

/**
 * this is a class which represents a type to kig, it is necessary
 * to support user defined types etc.  This is an abstract class, use
 * TType for predefined object types ( written in C++ ),  and
 * MType for user defined object types ( macros ),  which are
 * loaded from a file, or constructed by the user
 * i was going to have a slot here "void newObc()",  but that wasn't
 * possible since qt2 doesn't support template qobjects.  Instead, i
 * now have a class SlotWrapper, which has the slot, and forwards
 * requests to its parent.
 */
class Type
{
public:
  Type( KigDocument* inDoc ) : w( inDoc , this ), action(0)  {};
  virtual ~Type(){};
  // returns a new object of the type
  virtual Object* newObject() = 0;
  // naming: cf. object.h
  virtual QCString fullTypeName() const = 0;
  virtual QCString baseTypeName() const = 0;
  // the name of the picture associated with this object
  virtual QCString getPicName() const = 0;
  // short cut key
  virtual int getShortCutKey() const = 0;
  // a description for this type, it is shown on the "What's this"
  // help function
  virtual QString getDescription() const = 0;
  // (external) name we should give to the action
  virtual QString getActionName() const = 0;
  // internal name we should give to the action
  virtual QCString getInternalActionName() const = 0;
  QString tFullTypeName() const { return i18n( fullTypeName() );};
  SlotWrapper* getSlotWrapper(){ return &w; };

  // handy, but simply equals a dynamic_cast...
  MTypeOne* toMTypeOne();

  void setAction(KAction* a) { action = a; };
  KAction* getAction() { return action; };

protected:
  SlotWrapper w;
  KAction* action;
};

template < class ObjectType >
class TType
  : public Type
{
protected:
  QCString picName;
  QString actionName;
  QString description;
  int shortCutKey;
public:
  TType( KigDocument* inDoc,
	 const QCString& inPicName,
	 const QString& inActionName,
	 const QString& inDescription,
	 const int inShortCutKey = 0
	 )
    : Type( inDoc ),
      picName( inPicName ),
      actionName (inActionName),
      description (inDescription),
      shortCutKey( inShortCutKey )
  {};
  Object* newObject() { return new ObjectType;};
  QCString fullTypeName() const { return ObjectType::sFullTypeName(); };
  QCString baseTypeName() const { return ObjectType::sBaseTypeName(); };
  QCString getPicName() const { return picName;};
  int getShortCutKey() const { return shortCutKey;};
  QString getActionName() const { return actionName; };
  QString getDescription() const { return description; };
  QCString getInternalActionName() const { return QCString("new_")+fullTypeName();};
};

class MType
    :public Type
{
protected:
  ObjectHierarchy* hier;
public:
  MType( ObjectHierarchy* inHier, KigDocument*);
  ~MType() { delete hier; };
};

class MTypeOne
  : public MType
{
  Type* finalType;
  QString actionName;
  QString description;
public:
  MTypeOne( ObjectHierarchy* inHier,
	    const QString& actionName,
	    const QString& description,
	    KigDocument*);
  MTypeOne( const QDomElement& e, KigDocument*);
  ~MTypeOne() {};
  // TODO: check this..;
  QCString fullTypeName() const { return actionName.utf8(); };
  // this is correct.
  QCString baseTypeName() const { return finalType->baseTypeName(); };
  // for now, this is correct, maybe support the user defining the
  // picture someday...
  QCString getPicName() const { return finalType->getPicName(); };
  // this is correct, the user can always changed his keybindings with
  // standard kaction/xmlgui stuff
  int getShortCutKey() const { return 0; };
  // yup, this is ok
  QString getActionName() const { return actionName; };
  // ...
  QString getDescription() const { return description; };
  // not ok, obviously, this way, the user can't put these actions in
  // his toolbars and such...
  QCString getInternalActionName() const { return 0; };
  
  void saveXML(QDomDocument& doc, QDomNode& parent) const;

  Object* newObject();
};

#endif
