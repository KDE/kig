// text_type.h
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

#ifndef KIG_OBJECTS_TEXT_TYPE_H
#define KIG_OBJECTS_TEXT_TYPE_H

#include "object_type.h"

class TextType
  : public ObjectType
{
  const ArgParser mparser;
public:
  TextType();
  ~TextType();
  static const TextType* instance();

  int impRequirement( const ObjectImp* o, const Args& parents ) const;
  int resultId() const;

  // we're one of those special types that need the KigDocument in our
  // calc function, so the other calc is an assert( false ), and we do
  // the real work in the other calc..
  ObjectImp* calc( const Args& parents ) const;
  ObjectImp* calc( const Args& parents, const KigDocument& d ) const;

  bool canMove() const;
  void move( RealObject* ourobj, const Coordinate& from,
             const Coordinate& dist ) const;
};

#endif
