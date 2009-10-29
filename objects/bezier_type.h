// Copyright (C)  2009  Petr Gajdos <pgajdos@suse.cz>

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

#ifndef KIG_OBJECTS_BEZIER_TYPE_H
#define KIG_OBJECTS_BEZIER_TYPE_H

#include "base_type.h"

/**
 * Bézier curve of degree 2
 */
class BezierQuadricType
  : public ArgsParserObjectType
{
  BezierQuadricType();
  ~BezierQuadricType();
public:
  static const BezierQuadricType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  bool canMove( const ObjectTypeCalcer& o ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const;
};

/**
 * Bézier curve of degree 3
 */
class BezierCubicType
  : public ArgsParserObjectType
{
  BezierCubicType();
  ~BezierCubicType();
public:
  static const BezierCubicType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  bool canMove( const ObjectTypeCalcer& o ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const;
};

/**
 * Bézier curve of degree n
 */
class BezierCurveType
  : public ObjectType
{
  BezierCurveType();
  ~BezierCurveType();
public:
  static const BezierCurveType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const;
  Args sortArgs( const Args& args ) const;

  bool canMove( const ObjectTypeCalcer& o ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const;
}; 

/**
 * Rational Bézier curve of degree 2
 */
class RationalBezierQuadricType
  : public ArgsParserObjectType
{
  RationalBezierQuadricType();
  ~RationalBezierQuadricType();
public:
  static const RationalBezierQuadricType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  bool canMove( const ObjectTypeCalcer& o ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const;
};

/**
 * Rational Bézier curve of degree 3
 */
class RationalBezierCubicType
  : public ArgsParserObjectType
{
  RationalBezierCubicType();
  ~RationalBezierCubicType();
public:
  static const RationalBezierCubicType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  bool canMove( const ObjectTypeCalcer& o ) const;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const;
};


#endif
