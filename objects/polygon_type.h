// Copyright (C)  2003  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#ifndef KIG_OBJECTS_POLIGON_TYPE_H
#define KIG_OBJECTS_POLIGON_TYPE_H

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
 * Polygon by its vertices
 */
class PolygonBNPType
  : public ObjectType
{
  PolygonBNPType();
  ~PolygonBNPType();
public:
  static const PolygonBNPType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const;
  Args sortArgs( const Args& args ) const;
};

/**
 * Poligon by center and vertex
 */
class PoligonBCVType
  : public ArgsParserObjectType
{
  PoligonBCVType();
  ~PoligonBCVType();
public:
  static const PoligonBCVType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class PolygonLineIntersectionType
  : public ArgsParserObjectType
{
  PolygonLineIntersectionType();
  ~PolygonLineIntersectionType();
public:
  static const PolygonLineIntersectionType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class PolygonVertexType
  : public ArgsParserObjectType
{
  PolygonVertexType();
  ~PolygonVertexType();
public:
  static const PolygonVertexType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class PolygonSideType
  : public ArgsParserObjectType
{
  PolygonSideType();
  ~PolygonSideType();
public:
  static const PolygonSideType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif
