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
#include <list>

#include "type.h"

class Object;

class Types
  : public list<Type*>
{
public:
  Types() {};
  Types( KigDocument* inDoc, const QString& file_name);
  ~Types() {};
  void deleteAll();
  void saveToDir(const QString& dir_name);
  void saveToFile(const QString& file_name);
  Type* findType(const QCString& type) const;
  Object* newObject( const QCString& type );
  void add ( Type* t ) { push_back( t ); };
};

#endif
