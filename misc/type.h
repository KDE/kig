/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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

#ifndef KIG_MISC_TYPE_H
#define KIG_MISC_TYPE_H

#include "objects.h"

#include <map>
#include <qstring.h>
#include <qcstring.h>

class Object;
class ObjectHierarchy;

class QCString;
class QDomDocument;
class QDomElement;
class QDomNode;
class KAction;
class KigDocument;
class KigPainter;
class KigMode;
class NormalMode;

#include <qstring.h>

class MType;
class StdConstructibleType;

/**
 * this is a class which represents a type to kig, it is necessary
 * to support user defined types etc.  This is an abstract class, use
 * TType for predefined object types ( written in C++ ),  and
 * MType for user defined object types ( macros ),  which are
 * loaded from a file, or constructed by the user
 */
class Type
{
  myvector<KAction*> mactions;
public:
  virtual ~Type() {};

  virtual MType* toMType() { return 0; };
  virtual const MType* toMType() const { return 0; };

  virtual StdConstructibleType* toStdConstructible() { return 0; };
  virtual const StdConstructibleType* toStdConstructible() const { return 0; };

  /**
   * return the name of this type.  This is the same name as
   * Object::vFullTypeName()...
   */
  virtual const QCString fullName() const = 0;

  /**
   * @see Object::vBaseTypeName()
   */
  virtual const QCString baseTypeName() const = 0;

  /**
   * @see Object::vDescriptiveName
   */
  virtual const QString descriptiveName() const = 0;

  /**
   * @see Object::vTypeDescription()
   */
  virtual const QString description() const = 0;

  /**
   * @see Object::vIconName()
   */
  virtual const QCString iconFileName() const = 0;

  /**
   * saves type information to a file.  As this is only meaningful for
   * MType's, TType has an empty implementation...
   */
  virtual void saveXML( QDomDocument& doc, QDomNode& parent ) const = 0;

  typedef std::map<QCString, QString> ParamMap;

  /**
   * build a new object of this type...
   */
  virtual Object* build( const Objects& parents,
                         const ParamMap& params = ParamMap() ) = 0;

  /**
   * returns a ConstructMode, which allows the user to construct a new
   * Object... This calls Object::sConstructMode for TType and most of
   * the times just returns new StdConstructionMode...
   */
  virtual KigMode* constructMode( NormalMode* prev, KigDocument* doc ) = 0;

  /**
   * build an action which, when clicked, calls d->setMode(
   * new some_constructing_mode );
   * we keep pointers to all actions we build here, so we can delete
   * them when asked ( deleteActions() )...
   */
  KAction* constructAction( KigDocument* d );

  /**
   * this returns an internal name for the construct action... return
   * something like "objects_new_normalpoint".. MTypes return "" cause
   * they don't need this ...
   */
  virtual const char* actionName() const = 0;

  /**
   * This is used when the user "deletes" a type.. We delete all
   * actions so it seems to the user the type has been deleted...
   */
  void deleteActions();
};

template<class T>
class TType
  : public Type
{
  Object* build( const Objects& parents,
                 const ParamMap& params = ParamMap() );
  const QCString fullName() const;
  const QCString baseTypeName() const;
  const QString descriptiveName() const;
  const QString description() const;
  const QCString iconFileName() const;
  const char* actionName() const;
  void saveXML( QDomDocument&, QDomNode& ) const;
  KigMode* constructMode( NormalMode* prev, KigDocument* doc );
};

template<class T>
const char* TType<T>::actionName() const
{
  return T::sActionName();
}

template<class T>
KigMode* TType<T>::constructMode( NormalMode* prev, KigDocument* doc )
{
  return T::sConstructMode( this, doc, prev );
}

template <class T>
Object* TType<T>::build( const Objects& parents,
                         const Type::ParamMap& params )
{
  T* t = new T( parents );
  t->setParams( params );
  return t;
};

template <class T>
void TType<T>::saveXML( QDomDocument&, QDomNode& ) const
{
  // noop
}

template <class T>
const QCString TType<T>::fullName() const
{
  return T::sFullTypeName();
};

template <class T>
const QCString TType<T>::baseTypeName() const
{
  return T::sBaseTypeName();
}

template <class T>
const QString TType<T>::descriptiveName() const
{
  return T::sDescriptiveName();
};

template <class T>
const QString TType<T>::description() const
{
  return T::sDescription();
};

template <class T>
const QCString TType<T>::iconFileName() const
{
  return T::sIconFileName();
};

/**
 * A refinement of Type for types of objects that want to use
 * StdConstructingMode...
 */
class StdConstructibleType
  : public Type
{
public:
  virtual StdConstructibleType* toStdConstructible() { return this; };
  virtual const StdConstructibleType* toStdConstructible() const { return this; };
  virtual int wantArgs( const Objects& ) = 0;
  virtual QString useText( const Objects&, const Object* ) = 0;
  virtual void drawPrelim( KigPainter&, const Objects& ) = 0;
};

/**
 * template Type for builtin Types...
 */
template <class T>
class TStdType
  : public StdConstructibleType
{
public:
  Object* build( const Objects& parents,
                 const ParamMap& params = ParamMap() )
    {
      T* o = new T( parents );
      o->setParams( params );
      return o;
    };
  const QCString fullName() const
    {
      return T::sFullTypeName();
    };
  const QCString baseTypeName() const
    {
      return T::sBaseTypeName();
    };
  const QString descriptiveName() const
    {
      return T::sDescriptiveName();
    };
  const QString description() const
    {
      return T::sDescription();
    };
  const QCString iconFileName() const
    {
      return T::sIconFileName();
    };
  const char* actionName() const
    {
      return T::sActionName();
    };
  void saveXML( QDomDocument&, QDomNode& ) const
    {
      return;
    };
  KigMode* constructMode( NormalMode* prev, KigDocument* doc )
    {
      return T::sConstructMode( this, doc, prev );
    };
  int wantArgs( const Objects& );
  QString useText( const Objects&, const Object* );
  void drawPrelim( KigPainter&, const Objects& );
};

template<class T>
int TStdType<T>::wantArgs( const Objects& os )
{
  return T::sWantArgs( os );
};

template<class T>
QString TStdType<T>::useText( const Objects& os, const Object* o )
{
  return T::sUseText( os, o );
};

template<class T>
void TStdType<T>::drawPrelim( KigPainter& p, const Objects& o )
{
  T::sDrawPrelim( p, o );
};

/**
 * Type for macro types
 */
class MType
  : public StdConstructibleType
{
protected:
  ObjectHierarchy* mhier;
  QString mname;
  QString mdesc;
public:
  MType( ObjectHierarchy* inHier, const QString name, const QString desc );
  ~MType();
  /**
   * load info from XML...
   */
  MType( const QDomElement& e );
  Object* build( const Objects& parents,
                 const ParamMap& params = ParamMap() );
  MType* toMType() { return this; };
  const MType* toMType() const { return this; };
  const QCString fullName() const;
  const QCString baseTypeName() const;
  const QString descriptiveName() const;
  const QString description() const;
  const QCString iconFileName() const;
  const char* actionName() const;
  void saveXML( QDomDocument&, QDomNode& ) const;
  KigMode* constructMode( NormalMode* mode, KigDocument* doc );
  int wantArgs( const Objects& os );
  QString useText( const Objects& os, const Object* o );
  void drawPrelim( KigPainter&, const Objects& os );
};

#endif
