// segment.cc
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

#include "line_type.h"

#include "bogus_imp.h"
#include "point_imp.h"
#include "line_imp.h"
#include "object.h"

#include "../misc/common.h"

SegmentABType::SegmentABType()
  : ObjectABType( "segment", "SegmentAB" )
{
}

SegmentABType::~SegmentABType()
{
}

const SegmentABType* SegmentABType::instance()
{
  static const SegmentABType s;
  return &s;
}

ObjectImp* SegmentABType::calc( const Coordinate& a, const Coordinate& b ) const
{
  return new SegmentImp( a, b );
}

LineABType::LineABType()
  : ObjectABType( "line", "LineAB" )
{
}

LineABType::~LineABType()
{
}

const LineABType* LineABType::instance()
{
  static const LineABType s;
  return &s;
}

ObjectImp* LineABType::calc( const Coordinate& a, const Coordinate& b ) const
{
  return new LineImp( a, b );
}


RayABType::RayABType()
  : ObjectABType( "ray", "RayAB" )
{
}

RayABType::~RayABType()
{
}

const RayABType* RayABType::instance()
{
  static const RayABType s;
  return &s;
}

ObjectImp* RayABType::calc( const Coordinate& a, const Coordinate& b ) const
{
  return new RayImp( a, b );
}

LinePerpendLPType* LinePerpendLPType::instance()
{
  static LinePerpendLPType l;
  return &l;
}

ObjectImp* LinePerpendLPType::calc(
  const LineData& a,
  const Coordinate& b ) const
{
  Coordinate p = calcPointOnPerpend( a, b );
  return new LineImp( b, p );
}

LineParallelLPType::LineParallelLPType()
  : ObjectLPType( "line", "LineParallel" )
{
}

LineParallelLPType::~LineParallelLPType()
{
}

LineParallelLPType* LineParallelLPType::instance()
{
  static LineParallelLPType l;
  return &l;
}

ObjectImp* LineParallelLPType::calc(
  const LineData& a,
  const Coordinate& b ) const
{
  Coordinate r = calcPointOnParallel( a, b );
  return new LineImp( r, b );
}

LinePerpendLPType::LinePerpendLPType()
  : ObjectLPType( "line", "LinePerpend" )
{
}

LinePerpendLPType::~LinePerpendLPType()
{
}
