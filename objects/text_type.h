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

class GenericTextType
  : public ObjectType
{
  const ArgsParser mparser;
protected:
  GenericTextType( const char* fulltypename );
  ~GenericTextType();
public:
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  ObjectImp* calc( const Args& parents, const KigDocument& d ) const Q_DECL_OVERRIDE;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& os ) const Q_DECL_OVERRIDE;
  Args sortArgs( const Args& args ) const Q_DECL_OVERRIDE;

  bool canMove( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  bool isFreelyTranslatable( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  void move( ObjectTypeCalcer& ourobj, const Coordinate& to,
             const KigDocument& ) const Q_DECL_OVERRIDE;

  QStringList specialActions() const Q_DECL_OVERRIDE;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const Q_DECL_OVERRIDE;

  const ArgsParser& argParser() const;
};

class TextType
  : public GenericTextType
{
  TextType();
  ~TextType();
public:
  static const TextType* instance();

  QStringList specialActions() const Q_DECL_OVERRIDE;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const Q_DECL_OVERRIDE;
};

class NumericTextType
  : public GenericTextType
{
  NumericTextType();
  ~NumericTextType();
public:
  static const NumericTextType* instance();

  QStringList specialActions() const Q_DECL_OVERRIDE;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const Q_DECL_OVERRIDE;
};
#endif
