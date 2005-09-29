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

#ifndef KIG_OBJECTS_TEXT_TYPE_H
#define KIG_OBJECTS_TEXT_TYPE_H

#include "object_type.h"

class TextType
  : public ObjectType
{
  const ArgsParser mparser;
  TextType();
  ~TextType();
public:
  static const TextType* instance();

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  const ObjectImpType* resultId() const;

  ObjectImp* calc( const Args& parents, const KigDocument& d ) const;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& os ) const;
  Args sortArgs( const Args& args ) const;

  bool canMove( const ObjectTypeCalcer& ourobj ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& ourobj ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& ourobj, const Coordinate& to,
             const KigDocument& ) const;

  QStringList specialActions() const;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const;

  const ArgsParser& argParser() const;
};

#endif
