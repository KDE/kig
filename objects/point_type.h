// point_type.h
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef KIG_OBJECTS_POINT_TYPE_H
#define KIG_OBJECTS_POINT_TYPE_H

#include "base_type.h"
#include "common.h"

class FixedPointType
  : public ArgparserObjectType
{
  FixedPointType();
  ~FixedPointType();

  static const ArgsParser::spec argsspec[1];
public:
  static const FixedPointType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  bool canMove() const;
  const Coordinate moveReferencePoint( const RealObject* ourobj ) const;
  void move( RealObject* ourobj, const Coordinate& to,
             const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  QStringList specialActions() const;
  void executeAction( int i, RealObject* o, KigDocument& d, KigWidget& w,
                      NormalMode& m ) const;
};

class ConstrainedPointType
  : public ArgparserObjectType
{
  ConstrainedPointType();
  ~ConstrainedPointType();
public:
  static const ConstrainedPointType* instance();

  bool inherits( int type ) const;

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;

  bool canMove() const;
  const Coordinate moveReferencePoint( const RealObject* ourobj ) const;
  void move( RealObject* ourobj, const Coordinate& to,
             const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  QStringList specialActions() const;
  void executeAction( int i, RealObject* o, KigDocument& d, KigWidget& w,
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

#endif
