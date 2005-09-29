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

#include "common.h"
#include "object_holder.h"

std::vector<ObjectCalcer*> getAllCalcers( const std::vector<ObjectHolder*>& os )
{
  std::set<ObjectCalcer*> ret;
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
  {
    if (  ( *i )->nameCalcer() )
      ret.insert( ( *i )->nameCalcer() );
    ret.insert( ( *i )->calcer() );
  }
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}

std::vector<ObjectCalcer*> getCalcers( const std::vector<ObjectHolder*>& os )
{
  std::vector<ObjectCalcer*> ret;
  ret.reserve( os.size() );
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
    ret.push_back( ( *i )->calcer() );
  return ret;
}

