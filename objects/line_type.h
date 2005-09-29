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

#ifndef KIG_OBJECTS_SEGMENT_H
#define KIG_OBJECTS_SEGMENT_H

#include "base_type.h"

class LineData;

class SegmentABType
  : public ObjectABType
{
  SegmentABType();
  ~SegmentABType();
public:
  static const SegmentABType* instance();

  ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const;
  const ObjectImpType* resultId() const;

  QStringList specialActions() const;
  /**
   * execute the \p i 'th action from the specialActions above..
   */
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const;
};

class LineABType
  : public ObjectABType
{
  LineABType();
  ~LineABType();
public:
  static const LineABType* instance();
  ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const;
  const ObjectImpType* resultId() const;
};

class RayABType
  : public ObjectABType
{
  RayABType();
  ~RayABType();
public:
  static const RayABType* instance();
  ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const;
  const ObjectImpType* resultId() const;
};

class LinePerpendLPType
  : public ObjectLPType
{
  LinePerpendLPType();
  ~LinePerpendLPType();
public:
  static LinePerpendLPType* instance();
  ObjectImp* calc( const LineData& a, const Coordinate& b ) const;
  const ObjectImpType* resultId() const;
};

class LineParallelLPType
  : public ObjectLPType
{
  LineParallelLPType();
  ~LineParallelLPType();
public:
  static LineParallelLPType* instance();
  ObjectImp* calc( const LineData& a, const Coordinate& b ) const;
  const ObjectImpType* resultId() const;
};

class LineByVectorType
  : public ArgsParserObjectType
{
  LineByVectorType();
  ~LineByVectorType();
public:
  static const LineByVectorType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class HalflineByVectorType
  : public ArgsParserObjectType
{
  HalflineByVectorType();
  ~HalflineByVectorType();
public:
  static const HalflineByVectorType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif
