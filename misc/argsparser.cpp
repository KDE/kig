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

#include <algorithm>

ArgParser::ArgParser( const spec args[], int n )
  : mndt( n ), mwantedobjscount( 0 ), margs( args )
{
  for ( int i = 0; i < n; ++i )
    mwantedobjscount += margs[i].number;
}

int ArgParser::check( const Objects& os ) const
{
  std::vector<int> numbers( mndt );
  for ( int i = 0; i < mndt; ++i )
    numbers[i] = margs[i].number;

  for ( Objects::const_iterator o = os.begin(); o != os.end(); ++o )
  {
    for ( int i = 0; i < mndt; ++i )
    {
      if ( (*o)->isa( margs[i].type ) && numbers[i] > 0 )
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
    return Invalid;
  matched:
    ;
  };
  for ( int i = 0; i < mndt; ++i )
    if ( numbers[i] > 0 ) return Valid;
  return Complete;
}

Objects ArgParser::parse( const Objects& os ) const
{
  assert( check( os ) != Invalid );
  Objects ret( mwantedobjscount );

  std::vector<int> numbers( mndt );
  for ( int i = 0; i < mndt; ++i )
    numbers[i] = margs[i].number;

  std::vector<int> counters( mndt );
  counters[0] = margs[0].number;
  for ( int i = 1; i < mndt; ++i )
    counters[i] = counters[i-1] + margs[i].number;

  for ( Objects::const_iterator o = os.begin(); o != os.end(); ++o )
    for ( int i = 0; i < mndt; ++i )
      if ( (*o)->isa( margs[i].type ) && numbers[i] > 0 )
      {
        // object o is of a type that we're looking for
        ret[--counters[i]] = *o;
        --numbers[i];
        break;
      }
  return ret;
}

int CheckOneArgs::check( const Objects& os ) const
{
  return os.size() <= 1 ? Complete : 0;
}

ArgsChecker::~ArgsChecker()
{
}
