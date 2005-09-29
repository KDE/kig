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

#ifndef KIG_OBJECTS_BASE_TYPE_H
#define KIG_OBJECTS_BASE_TYPE_H

#include "object_type.h"

#include "../misc/argsparser.h"

class LineData;

class ObjectABType
  : public ArgsParserObjectType
{
protected:
  ObjectABType( const char* fulltypename, const ArgsParser::spec* argsspec, int n );
  ~ObjectABType();
public:
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  bool canMove( const ObjectTypeCalcer& o ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const;

  virtual ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const = 0;
};

class ObjectLPType
  : public ArgsParserObjectType
{
protected:
  ObjectLPType( const char* fullname, const ArgsParser::spec* spec, int n );
  ~ObjectLPType();
public:
  ObjectImp* calc( const Args& args, const KigDocument& ) const;

  virtual ObjectImp* calc( const LineData& a, const Coordinate& b ) const = 0;
};

#endif
