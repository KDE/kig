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

void ArgsParser::initialize( const struct spec* args, int n )
{
  std::vector<spec> vect( args, args + n );
  initialize( vect );
}

ArgsParser::ArgsParser()
{
}

ArgsParser::ArgsParser( const std::vector<spec>& args )
{
  initialize( args );
}

void ArgsParser::initialize( const std::vector<spec>& args )
{
  margs.clear();
  manyobjsspec.clear();
  for ( uint i = 0; i < args.size(); ++i )
    if ( args[i].type != ObjectImp::stype() )
      margs.push_back( args[i] );
    else manyobjsspec.push_back( args[i].usetext );
}

ArgsParser::ArgsParser( const spec* args, int n )
{
  initialize( args, n );
}

ArgsParser::ArgsParser( const std::vector<spec>& args, const std::vector<const char*> anyobjsspec )
  : margs( args ), manyobjsspec( anyobjsspec )
{
}


static bool hasimp( const Object& o, const ObjectImpType* imptype )
{
  return o.hasimp( imptype );
}

static bool hasimp( const ObjectImp& o, const ObjectImpType* imptype )
{
  return o.inherits( imptype );
}

static bool isvalid( const ObjectImp& o )
{
  return o.valid();
}

static bool isvalid( const Object& o )
{
  return o.valid();
}

// we use a template method that is used for both Objects and Args to
// not have to write the same thing twice..
template <class Collection>
static int check( const Collection& c, int numberofanyobjects,
                  const std::vector<ArgsParser::spec>& margs )
{
  std::vector<bool> found( margs.size() );

  for ( typename Collection::const_reverse_iterator o = c.rbegin(); o != c.rend(); ++o )
  {
    for ( int i = margs.size() - 1; i >= 0; --i )
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
    if ( --numberofanyobjects < 0 ) return ArgsParser::Invalid;
  matched:
    ;
  };
  if ( numberofanyobjects > 0 ) return ArgsParser::Valid;
  for( uint i = 0; i < margs.size(); ++i )
    if ( !found[i] ) return ArgsParser::Valid;
  return ArgsParser::Complete;
}

int ArgsParser::check( const Args& os ) const
{
  return ::check( os, manyobjsspec.size(), margs );
}

int ArgsParser::check( const Objects& os ) const
{
  return ::check( os, manyobjsspec.size(), margs );
}

template <class Collection>
static Collection parse( const Collection& os, uint numberofanyobjects,
                         const std::vector<ArgsParser::spec> margs )
{
  Collection ret( margs.size() + numberofanyobjects,
                  static_cast<typename Collection::value_type>( 0 ) );

  uint anyobjscounter = 0;

  for ( typename Collection::const_iterator o = os.begin(); o != os.end(); ++o )
  {
    if( anyobjscounter < numberofanyobjects )
    {
      ret[anyobjscounter++]= *o;
      goto added;
    }
    for( uint i = 0; i < margs.size(); ++i )
      if ( hasimp( **o, margs[i].type ) && ret[numberofanyobjects + i] == 0 )
      {
        // object o is of a type that we're looking for
        ret[numberofanyobjects + i] = *o;
        goto added;
      }
  added:
    ;
  };
  // remove 0's from the output..
  ret.erase(
    std::remove( ret.begin(), ret.end(),
                 static_cast<typename Collection::value_type>( 0 ) ),
    ret.end() );
  return ret;
}

Args ArgsParser::parse( const Args& os ) const
{
  return ::parse( os, manyobjsspec.size(), margs );
}

Objects ArgsParser::parse( const Objects& os ) const
{
  return ::parse( os, manyobjsspec.size(), margs );
}

ArgsParser::~ArgsParser()
{
}

ArgsParser ArgsParser::without( const ObjectImpType* type ) const
{
  if ( type == ObjectImp::stype() )
    return ArgsParser( margs, std::vector<const char*>() );
  std::vector<spec> ret;
  ret.reserve( margs.size() - 1 );
  for ( uint i = 0; i < margs.size(); ++i )
    if ( margs[i].type != type )
      ret.push_back( margs[i] );
  return ArgsParser( ret, manyobjsspec );
}

ArgsParser::spec ArgsParser::findSpec( const ObjectImp* obj, const Args& parents ) const
{
  spec ret;
  ret.type = 0;
  ret.usetext = 0;

  uint numberofanyobjects = manyobjsspec.size();
  uint anyobjscounter = 0;

  std::vector<bool> found( margs.size(), false );

  for ( Args::const_iterator o = parents.begin();
        o != parents.end(); ++o )
  {
    if ( anyobjscounter < numberofanyobjects )
    {
      if ( *o == obj )
      {
        ret.type = ObjectImp::stype();
        ret.usetext = manyobjsspec[anyobjscounter];
        return ret;
      };
      ++anyobjscounter;
      goto matched;
    }
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
  matched:
    ;
  };
  kdDebug() << k_funcinfo << "no proper spec found :(" << endl;
  return ret;
}

const ObjectImpType* ArgsParser::impRequirement(
  const ObjectImp* o, const Args& parents ) const
{
  spec s = findSpec( o, parents );
  return s.type;
}

const char* ArgsParser::usetext( const ObjectImp* obj, const Args& sel ) const
{
  spec s = findSpec( obj, sel );
  return s.usetext;
};

template<typename Collection>
static bool checkArgs( const Collection& os, uint min, const std::vector<ArgsParser::spec>& argsspec, uint anyobjsspecsize )
{
  assert( os.size() <= argsspec.size() + anyobjsspecsize );
  if( os.size() < min ) return false;
  uint checknum = std::min( os.size(), argsspec.size() + anyobjsspecsize );
  for ( uint i = anyobjsspecsize; i < checknum; ++i )
  {
    if( !isvalid( *os[i] ) ) return false;
    if( !hasimp( *os[i], argsspec[i - anyobjsspecsize].type ) ) return false;
  }
  return true;
}

bool ArgsParser::checkArgs( const Args& os ) const
{
  return checkArgs( os, margs.size() + manyobjsspec.size() );
}

bool ArgsParser::checkArgs( const Args& os, uint min ) const
{
  return ::checkArgs( os, min, margs, manyobjsspec.size() );
}

bool ArgsParser::checkArgs( const Objects& os ) const
{
  return checkArgs( os, margs.size() + manyobjsspec.size() );
}

bool ArgsParser::checkArgs( const Objects& os, uint min ) const
{
  return ::checkArgs( os, min, margs, manyobjsspec.size() );
}

