// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

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

