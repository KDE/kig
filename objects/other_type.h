// other_type.h
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

#ifndef KIG_MISC_OTHER_TYPE_H
#define KIG_MISC_OTHER_TYPE_H

#include "base_type.h"
#include "../misc/object_hierarchy.h"

class AngleType
  : public ArgparserObjectType
{
  AngleType();
  ~AngleType();
public:
  static const AngleType* instance();
  ObjectImp* calc( const Args& parents ) const;
  int resultId() const;
};

class VectorType
  : public ObjectABType
{
  VectorType();
  ~VectorType();
public:
  static const VectorType* instance();
  ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const;
  int resultId() const;
};

class LocusType
  : public CustomType
{
  typedef CustomType Parent;
  ObjectHierarchy mhier;
public:
  LocusType( const ObjectHierarchy& mhier );
  ~LocusType();
  ObjectImp* calc( const Args& args ) const;

  const ObjectHierarchy& hierarchy() const;

  bool inherits( int type ) const;
  int resultId() const;
};

#endif
