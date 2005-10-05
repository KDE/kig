// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_OBJECTS_POINT_TYPE_H
#define KIG_OBJECTS_POINT_TYPE_H

#include "base_type.h"
#include "common.h"
#include "circle_imp.h"

class FixedPointType
  : public ArgsParserObjectType
{
  FixedPointType();
  ~FixedPointType();

  static const ArgsParser::spec argsspec[1];
public:
  static const FixedPointType* instance();

  bool inherits( int type ) const;

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  bool canMove( const ObjectTypeCalcer& ourobj ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& ourobj ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& ourobj, const Coordinate& to,
             const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  QStringList specialActions() const;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& t,
                      KigPart& d, KigWidget& w, NormalMode& m ) const;
};

class RelativePointType
  : public ArgsParserObjectType
{
  RelativePointType();
  ~RelativePointType();

  static const ArgsParser::spec argsspec[1];
public:
  static const RelativePointType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  bool canMove( const ObjectTypeCalcer& ourobj ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& ourobj ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& ourobj, const Coordinate& to,
             const KigDocument& ) const;
  const ObjectImpType* resultId() const;

//  QStringList specialActions() const;
//  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& t,
//                      KigPart& d, KigWidget& w, NormalMode& m ) const;
};

class CursorPointType
  : public ObjectType
{
  CursorPointType();
  ~CursorPointType();

public:
  static const CursorPointType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const;
  Args sortArgs( const Args& args ) const;
  bool canMove( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& ourobj, const Coordinate& to,
             const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConstrainedPointType
  : public ArgsParserObjectType
{
  ConstrainedPointType();
  ~ConstrainedPointType();
public:
  static const ConstrainedPointType* instance();

  bool inherits( int type ) const;

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;

  bool canMove( const ObjectTypeCalcer& ourobj ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& ourobj ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& ourobj, const Coordinate& to,
             const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  QStringList specialActions() const;
  void executeAction( int i, ObjectHolder&, ObjectTypeCalcer& o, KigPart& d, KigWidget& w,
                      NormalMode& m ) const;
};

class MidPointType
  : public ObjectABType
{
  MidPointType();
  ~MidPointType();
public:
  static const MidPointType* instance();
  ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const;
  const ObjectImpType* resultId() const;
};

class MeasureTransportType
  : public ObjectType
{
  MeasureTransportType();
  ~MeasureTransportType();
public:
  static const MeasureTransportType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args )const;
  Args sortArgs( const Args& args ) const;
};

class MeasureTransportTypeOld
  : public ArgsParserObjectType
{
  MeasureTransportTypeOld();
  ~MeasureTransportTypeOld();
public:
  static const MeasureTransportTypeOld* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif
