// calcPaths.cc
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

#include "calcpaths.h"

#include "objects.h"
#include "../objects/object.h"

// these first two functions were written before i read stuff about
// graph theory and algorithms, so i'm sure they're far from optimal.
// However, they seem to work fine, and i don't think there's a real
// need for optimisation here..
Objects calcPath( const Objects& os )
{
  // this is a little experiment of mine, i don't know if it is the
  // fastest way to do it, but it seems logical to me...

  // the general idea here:
  // first we build a new Objects variable.  For every object in os,
  // we put all of its children at the end of it, and we do the same
  // for the ones we add..

  // "all" is the Objects var we're building...
  Objects all = os;
  // tmp is the var containing the objects we're iterating over.  The
  // first time around this is the os variable, the next time, this
  // contains the variables we added in the first round...
  Objects tmp = os;
  // tmp2 is a temporary var.  During a round, it receives all the
  // variables we add ( to "all" ) in that round, and at the end of
  // the round, it is assigned to tmp.
  Objects tmp2;
  while ( ! tmp.empty() )
  {
    for ( Objects::const_iterator i = tmp.begin(); i != tmp.end(); ++i )
    {
      const Objects& o = (*i)->children();
      std::copy( o.begin(), o.end(), std::back_inserter( all ) );
      std::copy( o.begin(), o.end(), std::back_inserter( tmp2 ) );
    };
    tmp = tmp2;
    tmp2.clear();
  };

  // now we know that if all objects appear at least once after all of
  // their parents.  So, we take all, and of every object, we remove
  // every reference except the last one...
  Objects ret;
  ret.reserve( os.size() );
  for ( Objects::reverse_iterator i = all.rbegin(); i != all.rend(); ++i )
  {
    if ( ! ret.contains( *i ) ) ret.push_back( *i );
  };
  std::reverse( ret.begin(), ret.end() );
  return ret;
};

bool addBranch( const Objects& o, const Object* to, Objects& ret )
{
  bool rb = false;
  for ( Objects::const_iterator i = o.begin(); i != o.end(); ++i )
  {
    if ( *i == to )
    {
      rb = true;
    }
    else
    {
      if ( addBranch( (*i)->children(), to, ret ) )
      {
        rb = true;
        ret.push_back( *i );
      };
    };
  };
  return rb;
};

Objects calcPath( const Objects& from, const Object* to )
{
  Objects all;

  for ( Objects::const_iterator i = from.begin(); i != from.end(); ++i )
  {
    (void) addBranch( (*i)->children(), to, all );
  };

  Objects ret;
  for ( Objects::iterator i = all.begin(); i != all.end(); ++i )
  {
    if ( ! ret.contains( *i ) ) ret.push_back( *i );
  };
  std::reverse( ret.begin(), ret.end() );
  return ret;
};

bool sideOfTreePathVisit( const Object* o, const Objects& from, Objects& ret )
{
  // this function returns true if the visited object depends on one
  // of the objects in from.  If we encounter objects that are on the
  // side of the tree path ( they do not depend on from themselves,
  // but their direct children do ), then we add them to ret.
  if ( from.contains( const_cast<Object*>( o ) ) ) return true;

  std::vector<bool> deps( o->parents().size(), false );
  bool somedepend = false;
  bool alldepend = true;
  for ( uint i = 0; i < o->parents().size(); ++i )
  {
    bool v = sideOfTreePathVisit( o->parents()[i], from, ret );
    somedepend |= v;
    alldepend &= v;
    deps[i] = v;
  };
  if ( somedepend && ! alldepend )
  {
    for ( uint i = 0; i < deps.size(); ++i )
      if ( ! deps[i] )
        ret.upush( o->parents()[i] );
  };

  return somedepend;
};

Objects sideOfTreePath( const Objects& from, const Object* to )
{
  Objects ret;
  sideOfTreePathVisit( to, from, ret );
  return ret;
};
