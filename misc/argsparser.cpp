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

ArgParser::ArgParser( const spec args[] )
{
  for ( const spec* s = args; s != 0; ++s )
    mmap[s->type] = s->number;
  mwantedobjscount = 0;
  for ( const spec* s = args; s != 0; ++s )
    mwantedobjscount += s->number;
}

int ArgParser::check( const Objects& os ) const
{
  // we take a copy, so we can change its contents..
  maptype map = mmap;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    maptype::iterator r = map.find( (*i)->type() );
    if ( r == map.end() )       // an obj of a type we don't want
      return 0;
    else if ( r->second <= 0 )  // too much objs of a type that we do
                                // want..
      return 0;
    else r->second--;           // a good obj
  };
  for ( maptype::const_iterator i = map.begin(); i != map.end(); ++i )
    if ( i->second > 0 ) return Valid; // we need more objs
  return Valid | Complete;      // and we're done..
}

Objects ArgParser::parse( const Objects& os ) const
{
  assert( check( os ) & Valid );

  Objects ret( mwantedobjscount, static_cast<Object*>( 0 ) );

  int count = 0;
  maptype map = mmap;
  for ( maptype::iterator i = map.begin(); i != map.end(); ++i )
  {
    count += i->second;
    i->second = count;
  };

  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    ret[--map[(*i)->type()]] = *i;

  return ret;
}

int CheckOneArgs::check( const Objects& os ) const
{
  return os.size() == 1 ? Valid | Complete : 0;
}
