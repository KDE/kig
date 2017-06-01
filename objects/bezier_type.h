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

  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
  bool canMove( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const Q_DECL_OVERRIDE;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
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

  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
  bool canMove( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const Q_DECL_OVERRIDE;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
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

  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const Q_DECL_OVERRIDE;
  Args sortArgs( const Args& args ) const Q_DECL_OVERRIDE;

  bool canMove( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const Q_DECL_OVERRIDE;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
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

  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
  bool canMove( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const Q_DECL_OVERRIDE;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
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

  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
  bool canMove( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const Q_DECL_OVERRIDE;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
};

/**
 * Rational Bézier curve of degree n
 */
class RationalBezierCurveType
  : public ObjectType
{
  RationalBezierCurveType();
  ~RationalBezierCurveType();
public:
  static const RationalBezierCurveType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const Q_DECL_OVERRIDE;
  Args sortArgs( const Args& args ) const Q_DECL_OVERRIDE;

  bool canMove( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  bool isFreelyTranslatable( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  void move( ObjectTypeCalcer& o, const Coordinate& to,
             const KigDocument& d ) const Q_DECL_OVERRIDE;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& o ) const Q_DECL_OVERRIDE;
}; 


#endif
