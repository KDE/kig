// object_constructor_list.cc
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "object_constructor_list.h"

#include "object_constructor.h"
#include "i18n.h"

#include "../objects/line_type.h"

ObjectConstructorList::ObjectConstructorList()
{
}

ObjectConstructorList::~ObjectConstructorList()
{
  for ( vectype::iterator i = mctors.begin(); i != mctors.end(); ++i )
    delete *i;
}

ObjectConstructorList* ObjectConstructorList::instance()
{
  static ObjectConstructorList s;
  return &s;
}

ObjectConstructorList::vectype ObjectConstructorList::ctorsThatWantArgs(
  const Objects& os, const KigDocument& d,
  const KigWidget& w, bool co ) const
{
  vectype ret;
  for ( vectype::const_iterator i = mctors.begin(); i != mctors.end(); ++i )
  {
    int r = (*i)->wantArgs( os, d, w );
    if ( r == ArgsChecker::Complete || ( !co && r == ArgsChecker::Valid ) )
      ret.push_back( *i );
  };
  return ret;
}

void ObjectConstructorList::add( ObjectConstructor* a )
{
  mctors.push_back( a );
}

void ObjectConstructorList::setupBuiltinTypes()
{
  add( new SimpleObjectTypeConstructor(
         SegmentABType::instance(),
         I18N_NOOP( "Segment" ),
         I18N_NOOP( "A segment constructed from its start and end point" ),
         "segment" ) );
  add( new SimpleObjectTypeConstructor(
         LineABType::instance(),
         I18N_NOOP( "Line by Two Points" ),
         I18N_NOOP( "A line constructed through two points"),
         "line" ) );
  add( new SimpleObjectTypeConstructor(
         RayABType::instance(),
         I18N_NOOP( "Ray" ),
         I18N_NOOP( "A ray by its start point, and another point somewhere on it." ),
         "ray" ) );
  add( new SimpleObjectTypeConstructor(
         LinePerpendLPType::instance(),
         I18N_NOOP( "Perpendicular" ),
         I18N_NOOP( "A line constructed through a point, perpendicular on another line or segment." ),
         "perpendicular" ) );
  add( new SimpleObjectTypeConstructor(
         LineParallelLPType::instance(),
         I18N_NOOP( "Parallel" ),
         I18N_NOOP( "A line constructed through a point, and parallel to another line or segment" ),
         "parallel" ) );
}
