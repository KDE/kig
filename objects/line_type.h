// segment.h
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
};

class LineABType
  : public ObjectABType
{
  LineABType();
  ~LineABType();
public:
  static const LineABType* instance();
  ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const;
};

class RayABType
  : public ObjectABType
{
  RayABType();
  ~RayABType();
public:
  static const RayABType* instance();
  ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const;
};

class ObjectLPType
  : public ObjectType
{
  static const struct ArgParser::spec argsspec[];
protected:
  ObjectLPType( const char* basename, const char* fullname );
  ~ObjectLPType();
public:
  ObjectImp* calc( const Args& args, const KigWidget& ) const;

  virtual ObjectImp* calc( const LineData& a, const Coordinate& b ) const = 0;
};

class LinePerpendLPType
  : public ObjectLPType
{
  LinePerpendLPType();
  ~LinePerpendLPType();
public:
  static LinePerpendLPType* instance();
  ObjectImp* calc( const LineData& a, const Coordinate& b ) const;
};

class LineParallelLPType
  : public ObjectLPType
{
  LineParallelLPType();
  ~LineParallelLPType();
public:
  static LineParallelLPType* instance();
  ObjectImp* calc( const LineData& a, const Coordinate& b ) const;
};

#endif
