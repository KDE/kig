// circle_type.h
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

#ifndef KIG_NW_OBJECTS_CIRCLE_TYPE_H
#define KIG_NW_OBJECTS_CIRCLE_TYPE_H

#include "base_type.h"

class CircleBCPType
  : public ObjectABType
{
public:
  CircleBCPType();
  ~CircleBCPType();
  ObjectType* copy() const;

  ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const;
};

class CircleBTPType
  : public ObjectABCType
{
  const Coordinate calcCenter( const Coordinate& a, const Coordinate& b,
                               const Coordinate& b ) const;
public:
  CircleBTPType();
  ~CircleBTPType();
  ObjectType* copy() const;

  ObjectImp* calc( const Args& args ) const;
};

#endif
