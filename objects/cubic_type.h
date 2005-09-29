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

#ifndef KIG_OBJECTS_CUBIC_TYPE_H
#define KIG_OBJECTS_CUBIC_TYPE_H

#include "object_type.h"

class CubicB9PType
  : public ArgsParserObjectType
{
  CubicB9PType();
  ~CubicB9PType();
public:
  static const CubicB9PType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class CubicNodeB6PType
  : public ArgsParserObjectType
{
  CubicNodeB6PType();
  ~CubicNodeB6PType();
public:
  static const CubicNodeB6PType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class CubicCuspB4PType
  : public ArgsParserObjectType
{
  CubicCuspB4PType();
  ~CubicCuspB4PType();
public:
  static const CubicCuspB4PType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif
