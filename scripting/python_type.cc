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

#include "python_type.h"

#include "python_scripter.h"

#include "../objects/object_imp.h"
#include "../objects/bogus_imp.h"

class PythonCompiledScriptImp
  : public BogusImp
{
  mutable CompiledPythonScript mscript;
public:
  typedef BogusImp Parent;
  static const ObjectImpType* stype();
  const ObjectImpType* type() const;

  PythonCompiledScriptImp( const CompiledPythonScript& s );

  void visit( ObjectImpVisitor* vtor ) const;
  ObjectImp* copy() const;
  bool equals( const ObjectImp& rhs ) const;

  bool isCache() const;

  CompiledPythonScript& data() const { return mscript; };
};

PythonCompiledScriptImp::PythonCompiledScriptImp( const CompiledPythonScript& s )
  : BogusImp(), mscript( s )
{

}

const ObjectImpType* PythonCompiledScriptImp::stype()
{
  static const ObjectImpType t( BogusImp::stype(), "python-compiled-script-imp",
                                0, 0, 0, 0, 0, 0, 0, 0, 0 );
  return &t;
}

const ObjectImpType* PythonCompiledScriptImp::type() const
{
  return PythonCompiledScriptImp::stype();
}

void PythonCompiledScriptImp::visit( ObjectImpVisitor* ) const
{
  // TODO ?
}

ObjectImp* PythonCompiledScriptImp::copy() const
{
  return new PythonCompiledScriptImp( mscript );
}

bool PythonCompiledScriptImp::equals( const ObjectImp& ) const
{
  // problem ?
  return true;
}

bool PythonCompiledScriptImp::isCache() const
{
  return true;
}

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PythonCompileType )

PythonCompileType::PythonCompileType()
  : ObjectType( "PythonCompileType" )
{
}

PythonCompileType::~PythonCompileType()
{
}

const PythonCompileType* PythonCompileType::instance()
{
  static const PythonCompileType t;
  return &t;
}

const ObjectImpType* PythonCompileType::impRequirement( const ObjectImp*, const Args& ) const
{
  return StringImp::stype();
}

const ObjectImpType* PythonCompileType::resultId() const
{
  return PythonCompiledScriptImp::stype();
}

ObjectImp* PythonCompileType::calc( const Args& parents, const KigDocument& ) const
{
  assert( parents.size() == 1 );
  if ( !parents[0]->inherits( StringImp::stype() ) ) return new InvalidImp;

  const StringImp* si = static_cast<const StringImp*>( parents[0] );
  QString s = si->data();

  CompiledPythonScript cs = PythonScripter::instance()->compile( s.latin1() );

  if ( cs.valid() )
    return new PythonCompiledScriptImp( cs );
  else
    return new InvalidImp();
}

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( PythonExecuteType )

PythonExecuteType::PythonExecuteType()
  : ObjectType( "PythonExecuteType" )
{
}

PythonExecuteType::~PythonExecuteType()
{
}

const PythonExecuteType* PythonExecuteType::instance()
{
  static const PythonExecuteType t;
  return &t;
}

ObjectImp* PythonExecuteType::calc( const Args& parents, const KigDocument& d ) const
{
  assert( parents.size() >= 1 );
  if( !parents[0]->inherits( PythonCompiledScriptImp::stype() ) ) return new InvalidImp;

  CompiledPythonScript& script = static_cast<const PythonCompiledScriptImp*>( parents[0] )->data();

  Args args( parents.begin() + 1, parents.end() );
  return script.calc( args, d );
}

const ObjectImpType* PythonExecuteType::impRequirement( const ObjectImp* o, const Args& parents ) const
{
  if ( o == parents[0] ) return PythonCompiledScriptImp::stype();
  else return ObjectImp::stype();
}

const ObjectImpType* PythonExecuteType::resultId() const
{
  return ObjectImp::stype();
}

std::vector<ObjectCalcer*> PythonCompileType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  return args;
}

Args PythonCompileType::sortArgs( const Args& args ) const
{
  return args;
}

std::vector<ObjectCalcer*> PythonExecuteType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  return args;
}

Args PythonExecuteType::sortArgs( const Args& args ) const
{
  return args;
}

bool PythonCompileType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  return false;
}

bool PythonExecuteType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  return false;
}

