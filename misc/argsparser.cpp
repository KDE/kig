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

ArgParser::ArgParser( const spec* args, int n, int numberofanyobjects )
  : mwantedobjscount( 0 ), margs( args, args + n ),
    manyobjectscount( numberofanyobjects )
{
  for ( int i = 0; i < n; ++i )
    mwantedobjscount += margs[i].number;
}

int ArgParser::check( const Objects& os ) const
{
  int numberofanyobjects = manyobjectscount;

  std::vector<int> numbers( margs.size() );
  for ( uint i = 0; i < margs.size(); ++i )
    numbers[i] = margs[i].number;

  for ( Objects::const_iterator o = os.begin(); o != os.end(); ++o )
  {
    for ( uint i = 0; i < margs.size(); ++i )
    {
      if ( (*o)->hasimp( margs[i].type ) && numbers[i] > 0 )
      {
        // object o is of a type that we're looking for
        --numbers[i];
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
    if ( numbers[i] > 0 ) return Valid;
  return Complete;
}

Args ArgParser::parse( const Args& os ) const
{
//  assert( check( os ) != Invalid );
  Args ret( mwantedobjscount + manyobjectscount );

  int anyobjscounter = 0;

  std::vector<int> numbers( margs.size() );
  for( uint i = 0; i < margs.size(); ++i )
    numbers[i] = margs[i].number;

  std::vector<int> counters( margs.size() );
  counters[0] = 0;
  for( uint i = 1; i < margs.size(); ++i )
    counters[i] = counters[i-1] + margs[i-1].number;

  for ( Args::const_iterator o = os.begin(); o != os.end(); ++o )
  {
    bool added = false;
    for( uint i = 0; i < margs.size(); ++i )
      if ( (*o)->inherits( margs[i].type ) && numbers[i] > 0 )
      {
        // object o is of a type that we're looking for
        ret[counters[i]++] = *o;
        added = true;
        --numbers[i];
        break;
      }
    if ( ! added )
    {
      assert( anyobjscounter < manyobjectscount );
      ret[mwantedobjscount + anyobjscounter++]= *o;
    };
  };
  return ret;
}

int CheckOneArgs::check( const Objects& os ) const
{
  return os.size() <= 1 ? Complete : 0;
}

ArgsChecker::~ArgsChecker()
{
}

Objects ArgParser::parse( const Objects& os ) const
{
  assert( check( os ) != Invalid );
  Objects ret( mwantedobjscount + manyobjectscount );

  int anyobjscounter = 0;

  std::vector<int> numbers( margs.size() );
  for( uint i = 0; i < margs.size(); ++i )
    numbers[i] = margs[i].number;

  std::vector<int> counters( margs.size() );
  counters[0] = 0;
  for( uint i = 1; i < margs.size(); ++i )
    counters[i] = counters[i-1] + margs[i-1].number;

  for ( Objects::const_iterator o = os.begin(); o != os.end(); ++o )
  {
    bool added = false;
    for( uint i = 0; i < margs.size(); ++i )
      if ( (*o)->hasimp( margs[i].type ) && numbers[i] > 0 )
      {
        // object o is of a type that we're looking for
        ret[counters[i]++] = *o;
        added = true;
        --numbers[i];
        break;
      }
    if ( ! added )
    {
      assert( anyobjscounter < manyobjectscount );
      ret[mwantedobjscount + anyobjscounter++] = *o;
    };
  };
  return ret;
}

ArgParser::ArgParser( const std::vector<spec>& args, int anyobjscount )
  : mwantedobjscount( 0 ), margs( args ), manyobjectscount( anyobjscount )
{
  for ( uint i = 0; i < margs.size(); ++i )
    mwantedobjscount += margs[i].number;
}

ArgParser ArgParser::without( int type ) const
{
  if ( type == ObjectImp::ID_AnyImp )
    return ArgParser( margs, 0 );
  std::vector<spec> ret;
  ret.reserve( margs.size() - 1 );
  for ( uint i = 0; i < margs.size(); ++i )
    if ( margs[i].type != type )
      ret.push_back( margs[i] );
  return ArgParser( ret, manyobjectscount );
}
