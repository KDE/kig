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

#ifndef types_h
#define types_h

#include <qcstring.h>
#include <qstring.h>

#include <map>

#include "type.h"

class Object;

typedef std::map<QCString, Type*> _tm;

/**
 * Types is a collection of types ( hm... ).  It lets you build
 * objects by name ( cf. Object::vFullTypeName() ), and for saving
 * macro types to a file or a number of files in a directory...
 */
class Types
  : protected _tm
{
public:
  // iterating:
  using _tm::iterator;
  using _tm::const_iterator;
  iterator begin() { return _tm::begin(); };
  const_iterator begin() const { return _tm::begin(); };
  iterator end() { return _tm::end(); };
  const_iterator end() const { return _tm::end(); };
  bool empty() const { return _tm::empty(); };

  /**
   * Constructs a new Types collection, and loads its data from the
   * file...
   */
  Types( const QString& file );

  Types() {};

  /**
   * on destruction, we delete all contained TypeRepresentant's...
   */
  ~Types();

  /**
   * find the TypeRepresentant for type t
   */
  Type* findType( const QCString& t ) const;
  /**
   * builds an object of type type...
   */
  Object* buildObject( const QCString& type,
                       const Objects& parents,
                       const Type::ParamMap& params = Type::ParamMap() ) const;
  /**
   * t should be constructed via new, and not touched anymore
   * afterwards...  this also adds a new action to every
   * KigDocument...
   */
  void addType( Type* t );

  void addTypes( Types& i );

  /**
   * remove a type, this also removes the appropriate actions from all
   * KigDocuments
   */
  void removeType( Type* t );

  void saveToDir( const QString dir );

  void saveToFile( const QString file );
};

#endif
