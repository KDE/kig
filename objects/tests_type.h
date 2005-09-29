// Copyright (C)  2004  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_OBJECTS_TESTS_TYPE_H
#define KIG_OBJECTS_TESTS_TYPE_H

#include "base_type.h"

class AreParallelType
  : public ArgsParserObjectType
{
  AreParallelType();
  ~AreParallelType();
public:
  static const AreParallelType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class AreOrthogonalType
  : public ArgsParserObjectType
{
  AreOrthogonalType();
  ~AreOrthogonalType();
public:
  static const AreOrthogonalType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class AreCollinearType
  : public ArgsParserObjectType
{
  AreCollinearType();
  ~AreCollinearType();
public:
  static const AreCollinearType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ContainsTestType
  : public ArgsParserObjectType
{
  ContainsTestType();
  ~ContainsTestType();
public:
  static const ContainsTestType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class InPolygonTestType
  : public ArgsParserObjectType
{
  InPolygonTestType();
  ~InPolygonTestType();
public:
  static const InPolygonTestType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConvexPolygonTestType
  : public ArgsParserObjectType
{
  ConvexPolygonTestType();
  ~ConvexPolygonTestType();
public:
  static const ConvexPolygonTestType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class SameDistanceType
  : public ArgsParserObjectType
{
  SameDistanceType();
  ~SameDistanceType();
public:
  static const SameDistanceType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class VectorEqualityTestType
  : public ArgsParserObjectType
{
  VectorEqualityTestType();
  ~VectorEqualityTestType();
public:
  static const VectorEqualityTestType* instance();
  ObjectImp* calc( const Args& parents,  const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif
