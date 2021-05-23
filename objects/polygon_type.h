// SPDX-FileCopyrightText: 2003 Maurizio Paolini <paolini@dmf.unicatt.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_POLYGON_TYPE_H
#define KIG_OBJECTS_POLYGON_TYPE_H

#include "base_type.h"

/**
 * Triangle by its vertices
 */
class TriangleB3PType
  : public ArgsParserObjectType
{
  TriangleB3PType();
  ~TriangleB3PType();
public:
  static const TriangleB3PType* instance();

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
 * Polygon by its vertices
 */
class PolygonBNPType
  : public ObjectType
{
  PolygonBNPType();
  ~PolygonBNPType();
public:
  static const PolygonBNPType* instance();

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
 * Open Polygon (Polyline, Polygonal Line)
 */
class OpenPolygonType
  : public ObjectType
{
  OpenPolygonType();
  ~OpenPolygonType();
public:
  static const OpenPolygonType* instance();

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
 * Polygon by center and vertex
 */
class PolygonBCVType
  : public ObjectType
{
  PolygonBCVType();
  ~PolygonBCVType();
public:
  static const PolygonBCVType* instance();

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

int polygonlineintersection( const std::vector<Coordinate>& ppoints, 
      const Coordinate &a, const Coordinate &b,
      double& t1, double& t2,
      std::vector<Coordinate>::const_iterator& intersectionside );

int polygonlineintersection( const std::vector<Coordinate>& ppoints, 
      const Coordinate &a, const Coordinate &b,
      bool boundleft, bool boundright, bool inside, 
      bool openpolygon, double& t1, double& t2,
      std::vector<Coordinate>::const_iterator& intersectionside );

class PolygonLineIntersectionType
  : public ArgsParserObjectType
{
  PolygonLineIntersectionType();
protected:
  PolygonLineIntersectionType( const char fulltypename[],
                        const struct ArgsParser::spec argsspec[],
                        int n );
  ~PolygonLineIntersectionType();
public:
  static const PolygonLineIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class OPolygonalLineIntersectionType
  : public PolygonLineIntersectionType
{
  OPolygonalLineIntersectionType();
  ~OPolygonalLineIntersectionType();
public:
  static const OPolygonalLineIntersectionType* instance();
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class CPolygonalLineIntersectionType
  : public PolygonLineIntersectionType
{
  CPolygonalLineIntersectionType();
  ~CPolygonalLineIntersectionType();
public:
  static const CPolygonalLineIntersectionType* instance();
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class PolygonPolygonIntersectionType
  : public ArgsParserObjectType
{
  PolygonPolygonIntersectionType();
  ~PolygonPolygonIntersectionType();
public:
  static const PolygonPolygonIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class PolygonVertexType
  : public ArgsParserObjectType
{
  PolygonVertexType();
  ~PolygonVertexType();
public:
  static const PolygonVertexType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class PolygonSideType
  : public ArgsParserObjectType
{
  PolygonSideType();
  ~PolygonSideType();
public:
  static const PolygonSideType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ConvexHullType
  : public ArgsParserObjectType
{
  ConvexHullType();
  ~ConvexHullType();
public:
  static const ConvexHullType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

#endif
