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
#include "../misc/i18n.h"

static const ArgParser::spec argsspecSegmentAB[] =
{
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a segment from this point" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a segment to this point" ) }
};

SegmentABType::SegmentABType()
  : ObjectABType( "SegmentAB", argsspecSegmentAB, 2 )
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

static const char* constructlineabstat = I18N_NOOP( "Construct a line through this point" );

static const ArgParser::spec argsspecLineAB[] =
{
  { ObjectImp::ID_PointImp, constructlineabstat },
  { ObjectImp::ID_PointImp, constructlineabstat }
};

LineABType::LineABType()
  : ObjectABType( "LineAB", argsspecLineAB, 2 )
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

static const ArgParser::spec argsspecRayAB[] =
{
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a ray from this point" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a ray through this point" ) }
};

RayABType::RayABType()
  : ObjectABType( "RayAB", argsspecRayAB, 2 )
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

static const ArgParser::spec argsspecLineParallel[] =
{
  { ObjectImp::ID_LineImp, I18N_NOOP( "Construct a parallel of this line" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a parallel through this point" ) }
};

LineParallelLPType::LineParallelLPType()
  : ObjectLPType( "LineParallel", argsspecLineParallel, 2 )
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

static const ArgParser::spec argsspecLinePerpend[] =
{
  { ObjectImp::ID_LineImp, I18N_NOOP( "Construct a perpendicular of this line" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a perpendicular through this point" ) }
};

LinePerpendLPType::LinePerpendLPType()
  : ObjectLPType( "LinePerpend", argsspecLinePerpend, 2 )
{
}

LinePerpendLPType::~LinePerpendLPType()
{
}

int SegmentABType::resultId() const
{
  return ObjectImp::ID_SegmentImp;
}

int LineABType::resultId() const
{
  return ObjectImp::ID_LineImp;
}

int RayABType::resultId() const
{
  return ObjectImp::ID_RayImp;
}

int LinePerpendLPType::resultId() const
{
  return ObjectImp::ID_LineImp;
}

int LineParallelLPType::resultId() const
{
  return ObjectImp::ID_LineImp;
}
