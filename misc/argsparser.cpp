// argsparser.cpp
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#include "argsparser.h"

#include "../objects/object.h"
#include "../objects/object_imp.h"

#include <algorithm>


ArgParser::ArgParser( const std::vector<spec>& args )
{
  for ( uint i = 0; i < args.size(); ++i )
    if ( args[i].type != ObjectImp::ID_AnyImp )
      margs.push_back( args[i] );
    else manyobjsspec.push_back( args[i].usetext );
}

ArgParser::ArgParser( const spec* args, int n )
{
  for ( int i = 0; i < n; ++i )
    if ( args[i].type != ObjectImp::ID_AnyImp )
      margs.push_back( args[i] );
    else manyobjsspec.push_back( args[i].usetext );
}

ArgParser::ArgParser( const std::vector<spec>& args, const std::vector<const char*> anyobjsspec )
  : margs( args ), manyobjsspec( anyobjsspec )
{
}

int ArgParser::check( const Objects& os ) const
{
  int numberofanyobjects = manyobjsspec.size();

  std::vector<bool> found( margs.size() );

  for ( Objects::const_iterator o = os.begin(); o != os.end(); ++o )
  {
    for ( uint i = 0; i < margs.size(); ++i )
    {
      if ( (*o)->hasimp( margs[i].type ) && !found[i] )
      {
        // object o is of a type that we're looking for
        found[i] = true;
        // i know that "goto's are evil", but they're very useful and
        // completely harmless if you use them as better "break;"
        // statements.. trust me ;)
        goto matched;
      };
    };
    // object o is not of a type in margs..
    if ( --numberofanyobjects < 0 ) return Invalid;
  matched:
    ;
  };
  if ( numberofanyobjects > 0 ) return Valid;
  for( uint i = 0; i < margs.size(); ++i )
    if ( !found[i] ) return Valid;
  return Complete;
}

static bool hasimp( const Object& o, int imptype )
{
  return o.hasimp( imptype );
};

static bool hasimp( const ObjectImp& o, int imptype )
{
  return o.inherits( imptype );
};

Args ArgParser::parse( const Args& os ) const
{
//  assert( check( os ) != Invalid );
  Args ret( margs.size() + manyobjsspec.size(), 0 );

  uint anyobjscounter = 0;

  for ( Args::const_iterator o = os.begin(); o != os.end(); ++o )
  {
    for( uint i = 0; i < margs.size(); ++i )
      if ( hasimp( **o, margs[i].type ) && ret[i] == 0 )
      {
        // object o is of a type that we're looking for
        ret[i] = *o;
        goto added;
      }
    assert( anyobjscounter < manyobjsspec.size() );
    ret[margs.size() + anyobjscounter++]= *o;
  added:
    ;
  };
  return ret;
}

Objects ArgParser::parse( const Objects& os ) const
{
//  assert( check( os ) != Invalid );
  Objects ret( margs.size() + manyobjsspec.size(), 0 );

  uint anyobjscounter = 0;

  for ( Objects::const_iterator o = os.begin(); o != os.end(); ++o )
  {
    for( uint i = 0; i < margs.size(); ++i )
      if ( hasimp( **o, margs[i].type ) && ret[i] == 0 )
      {
        // object o is of a type that we're looking for
        ret[i] = *o;
        goto added;
      }
    assert( anyobjscounter < manyobjsspec.size() );
    ret[margs.size() + anyobjscounter++]= *o;
  added:
    ;
  };
  return ret;
}

int CheckOneArgs::check( const Objects& os ) const
{
  if ( os.empty() ) return Valid;
  if ( os.size() == 1 ) return Complete;
  return Invalid;
}

ArgsChecker::~ArgsChecker()
{
}

ArgParser ArgParser::without( int type ) const
{
  if ( type == ObjectImp::ID_AnyImp )
    return ArgParser( margs, std::vector<const char*>() );
  std::vector<spec> ret;
  ret.reserve( margs.size() - 1 );
  for ( uint i = 0; i < margs.size(); ++i )
    if ( margs[i].type != type )
      ret.push_back( margs[i] );
  return ArgParser( ret, manyobjsspec );
}

ArgParser::spec ArgParser::findSpec( const ObjectImp* obj, const Args& parents ) const
{
  spec ret;
  ret.type = -1;
  ret.usetext = 0;

  int numberofanyobjects = manyobjsspec.size();

  std::vector<bool> found( margs.size() );

  for ( Args::const_iterator o = parents.begin(); o != parents.end(); ++o )
  {
    for ( uint i = 0; i < margs.size(); ++i )
    {
      if ( (*o)->inherits( margs[i].type ) && !found[i] )
      {
        // object o is of a type that we're looking for
        found[i] = true;
        // i know that "goto's are evil", but they're very useful and
        // completely harmless if you use them as better "break;"
        // statements.. trust me ;)
        goto matched;
      };
    };
    if ( --numberofanyobjects < 0 )
      return ret;
  matched:
    ;
  };

  for ( uint i = 0; i < margs.size(); ++i )
    if ( obj->inherits( margs[i].type ) && ! found[i] )
      return margs[i];

  assert( numberofanyobjects >= 0 );
  if ( numberofanyobjects == 0 ) return ret;

  ret.type = ObjectImp::ID_AnyImp;
  ret.usetext = manyobjsspec[manyobjsspec.size() - numberofanyobjects];
  return ret;
}

int ArgParser::impRequirement( const ObjectImp* o, const Args& parents ) const
{
  spec s = findSpec( o, parents );
  return s.type;
}

const char* ArgParser::usetext( const ObjectImp* obj, const Args& sel ) const
{
  spec s = findSpec( obj, sel );
  return s.usetext;
}

