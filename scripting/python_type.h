// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_SCRIPTING_PYTHON_TYPE_H
#define KIG_SCRIPTING_PYTHON_TYPE_H

#include "../objects/object_type.h"

class PythonCompileType
  : public ObjectType
{
  PythonCompileType();
  ~PythonCompileType();
public:
  static const PythonCompileType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& d ) const;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  const ObjectImpType* resultId() const;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const;
  Args sortArgs( const Args& args ) const;
};

class PythonExecuteType
  : public ObjectType
{
  PythonExecuteType();
  ~PythonExecuteType();
public:
  static const PythonExecuteType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& d ) const;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  const ObjectImpType* resultId() const;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const;
  Args sortArgs( const Args& args ) const;

//   virtual QStringList specialActions() const;
//   virtual void executeAction( int i, RealObject* o, KigDocument& d, KigWidget& w,
//                               NormalMode& m ) const;
};

#endif
