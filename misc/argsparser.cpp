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
#include <kdebug.h>

void ArgParser::initialize( const struct spec* args, int n )
{
  std::vector<spec> vect( args, args + n );
  initialize( vect );
}

ArgParser::ArgParser()
{
}

ArgParser::ArgParser( const std::vector<spec>& args )
{
  initialize( args );
}

void ArgParser::initialize( const std::vector<spec>& args )
{
  margs.clear();
  manyobjsspec.clear();
  for ( uint i = 0; i < args.size(); ++i )
    if ( args[i].type != ObjectImp::ID_AnyImp )
      margs.push_back( args[i] );
    else manyobjsspec.push_back( args[i].usetext );
}

ArgParser::ArgParser( const spec* args, int n )
{
  initialize( args, n );
}

ArgParser::ArgParser( const std::vector<spec>& args, const std::vector<const char*> anyobjsspec )
  : margs( args ), manyobjsspec( anyobjsspec )
{
}


static bool hasimp( const Object& o, int imptype )
{
  return o.hasimp( imptype );
};

static bool hasimp( const ObjectImp& o, int imptype )
{
  return o.inherits( imptype );
};

// we use a template method that is used for both Objects and Args to
// not have to write the same thing twice..
template <class Collection>
static int check( const Collection& c, int numberofanyobjects,
                  const std::vector<ArgParser::spec>& margs )
{
  std::vector<bool> found( margs.size() );

  for ( typename Collection::const_iterator o = c.begin(); o != c.end(); ++o )
  {
    for ( uint i = 0; i < margs.size(); ++i )
    {
      if ( hasimp( **o, margs[i].type ) && !found[i] )
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
    if ( --numberofanyobjects < 0 ) return ArgsChecker::Invalid;
  matched:
    ;
  };
  if ( numberofanyobjects > 0 ) return ArgsChecker::Valid;
  for( uint i = 0; i < margs.size(); ++i )
    if ( !found[i] ) return ArgsChecker::Valid;
  return ArgsChecker::Complete;
};

int ArgParser::check( const Args& os ) const
{
  return ::check( os, manyobjsspec.size(), margs );
}

int ArgParser::check( const Objects& os ) const
{
  return ::check( os, manyobjsspec.size(), margs );
}

template <class Collection>
Collection parse( const Collection& os, uint numberofanyobjects,
                  const std::vector<ArgParser::spec> margs )
{
  Collection ret( margs.size() + numberofanyobjects, 0 );

  uint anyobjscounter = 0;

  for ( typename Collection::const_reverse_iterator o = os.rbegin(); o != os.rend(); ++o )
  {
    for( uint i = 0; i < margs.size(); ++i )
      if ( hasimp( **o, margs[i].type ) && ret[i] == 0 )
      {
        // object o is of a type that we're looking for
        ret[i] = *o;
        goto added;
      }
    if( anyobjscounter < numberofanyobjects )
      ret[margs.size() + anyobjscounter++]= *o;
  added:
    ;
  };
  return ret;
};

Args ArgParser::parse( const Args& os ) const
{
  return ::parse( os, manyobjsspec.size(), margs );
}

Objects ArgParser::parse( const Objects& os ) const
{
  return ::parse( os, manyobjsspec.size(), margs );
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

  uint numberofanyobjects = manyobjsspec.size();
  uint anyobjscounter = 0;

  // special case hack for some transformation types that take an
  // argument and an AnyImp..  if anyone has a better solution, please
  // tell me :)
  if ( parents.size() == 1 && numberofanyobjects > 0 )
  {
    assert( parents[0] == obj );
    ret.type = ObjectImp::ID_AnyImp;
    ret.usetext = manyobjsspec[0];
    return ret;
  }

  std::vector<bool> found( margs.size() );

  for ( Args::const_reverse_iterator o = parents.rbegin();
        o != parents.rend(); ++o )
  {
    for ( uint i = 0; i < margs.size(); ++i )
    {
      if ( (*o)->inherits( margs[i].type ) && !found[i] )
      {
        // object o is of a type that we're looking for
        found[i] = true;
        if ( *o == obj ) return margs[i];
        // i know that "goto's are *evil*", but they're very useful
        // and completely harmless if you use them as better "break;"
        // statements.. trust me ;)
        goto matched;
      };
    };
    if ( anyobjscounter < numberofanyobjects )
    {
      if ( *o == obj )
      {
        ret.type = ObjectImp::ID_AnyImp;
        ret.usetext = manyobjsspec[anyobjscounter];
        return ret;
      };
      ++anyobjscounter;
    };
  matched:
    ;
  };
  kdDebug() << k_funcinfo << "no proper spec found :(" << endl;
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

