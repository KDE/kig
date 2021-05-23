// SPDX-FileCopyrightText: 2009 Petr Gajdos <pgajdos@suse.cz>

// SPDX-License-Identifier: GPL-2.0-or-later

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
