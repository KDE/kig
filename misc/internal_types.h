// internal_types.h
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef INTERNAL_TYPES_H
#define INTERNAL_TYPES_H

#include <qstring.h>

#include <map>

class ObjectHierarchy;

class QDomElement;
class QDomDocument;
class QDomNode;

class Object;

// a class representing a type of object...
class TypeRepresentant
{
public:
  virtual ~TypeRepresentant() {};

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
   * MTypeBuilder's, TTypeBuilder has an empty implementation...
   */
  virtual void saveXML( QDomDocument& doc, QDomNode& parent ) const = 0;

  /**
   * build a new object of this type...
   */
  virtual Object* build() = 0;
};

/**
 * template TypeBuilder for builtin Types...
 */
template <class T>
class TTypeRepresentant
  : public TypeRepresentant
{
public:
  Object* build();
  const QCString fullName() const;
  const QCString baseTypeName() const;
  const QString descriptiveName() const;
  const QString description() const;
  const QCString iconFileName() const;
  void saveXML( QDomDocument&, QDomNode& ) const;
};

template <class T>
Object* TTypeRepresentant<T>::build()
{
  return new T;
};

template <class T>
void TTypeRepresentant<T>::saveXML( QDomDocument&, QDomNode& ) const
{
  // noop
}

template <class T>
const QCString TTypeRepresentant<T>::fullName() const
{
  return T::sFullTypeName();
};

template <class T>
const QCString TTypeRepresentant<T>::baseTypeName() const
{
  return T::sBaseTypeName();
}

template <class T>
const QString TTypeRepresentant<T>::descriptiveName() const
{
  return T::sDescriptiveName();
};

template <class T>
const QString TTypeRepresentant<T>::description() const
{
  return T::sDescription();
};

template <class T>
const QCString TTypeRepresentant<T>::iconFileName() const
{
  return T::sIconFileName();
};

/**
 * TypeRepresentant for macro types
 */
class MTypeRepresentant
  : public TypeRepresentant
{
  ObjectHierarchy* mhier;
  QString mname;
  QString mdesc;
public:
  MTypeRepresentant( ObjectHierarchy* hier, const QString name, const QString desc );
  /**
   * load info from XML...
   */
  MTypeRepresentant( const QDomElement& e );
  ~MTypeRepresentant();
  Object* build();
  const QCString fullName() const;
  const QCString baseTypeName() const;
  const QString descriptiveName() const;
  const QString description() const;
  const QCString iconFileName() const;
  void saveXML( QDomDocument&, QDomNode& ) const;
};

/**
 * InternalTypes contains information about
 * the Object types currently supported by Kig, both the builtin as
 * the macro types...  It has support for building an object from a
 * name (cf. Object::vFullTypeName()), and for saving the types to a
 * directory (i.e. only the Macro types get saved, of course...)
 */
class InternalTypes
{
public:
  typedef std::map<QCString,TypeRepresentant*> imap;
private:
  imap mmap;
public:
  // iterating:
  typedef imap::iterator iterator;
  typedef imap::const_iterator const_iterator;
  imap::iterator begin() { return mmap.begin(); };
  imap::const_iterator begin() const { return mmap.begin(); };
  imap::iterator end() { return mmap.end(); };
  imap::const_iterator end() const { return mmap.end(); };

  bool empty() const { return mmap.empty(); };

  /**
   * Constructs a new Types collection, and loads its data from the
   * file...
   */
  InternalTypes( const QString& file );

  InternalTypes() {};

  /**
   * on destruction, we delete all contained TypeRepresentant's...
   */
  ~InternalTypes();
  /**
   * find the TypeRepresentant for type t
   */
  TypeRepresentant* findType( const QCString& t );
  /**
   * builds an object of type type...
   */
  Object* buildObject( const QCString& type );
  /**
   * t should be constructed via new, and not touched anymore
   * afterwards...
   */
  void addType( TypeRepresentant* t );

  void addTypes( InternalTypes& i );

  void removeType( TypeRepresentant* t );

  void saveToDir( const QString dir );

  void saveToFile( const QString file );
};

#endif
