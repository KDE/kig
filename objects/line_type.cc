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
#include "object_holder.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../kig/kig_commands.h"
#include "../misc/common.h"
#include "../misc/calcpaths.h"
#include "../misc/i18n.h"

#include <qstringlist.h>

static const ArgsParser::spec argsspecSegmentAB[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct a segment ending at this point" ), true },
  { PointImp::stype(), I18N_NOOP( "Construct a segment starting at this point" ), true }
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

static const char constructlineabstat[] = I18N_NOOP( "Construct a line through this point" );

static const ArgsParser::spec argsspecLineAB[] =
{
  { PointImp::stype(), constructlineabstat, true },
  { PointImp::stype(), constructlineabstat, true }
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

static const ArgsParser::spec argsspecRayAB[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct a ray starting at this point" ), true },
  { PointImp::stype(), I18N_NOOP( "Construct a ray through this point" ), true }
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

static const ArgsParser::spec argsspecLineParallel[] =
{
  { AbstractLineImp::stype(), I18N_NOOP( "Construct a parallel of this line" ), false },
  { PointImp::stype(), I18N_NOOP( "Construct a parallel through this point" ), true }
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

static const ArgsParser::spec argsspecLinePerpend[] =
{
  { AbstractLineImp::stype(), I18N_NOOP( "Construct a perpendicular of this line" ), false },
  { PointImp::stype(), I18N_NOOP( "Construct a perpendicular through this point" ), true }
};

LinePerpendLPType::LinePerpendLPType()
  : ObjectLPType( "LinePerpend", argsspecLinePerpend, 2 )
{
}

LinePerpendLPType::~LinePerpendLPType()
{
}

const ObjectImpType* SegmentABType::resultId() const
{
  return SegmentImp::stype();
}

const ObjectImpType* LineABType::resultId() const
{
  return LineImp::stype();
}

const ObjectImpType* RayABType::resultId() const
{
  return RayImp::stype();
}

const ObjectImpType* LinePerpendLPType::resultId() const
{
  return LineImp::stype();
}

const ObjectImpType* LineParallelLPType::resultId() const
{
  return LineImp::stype();
}

QStringList SegmentABType::specialActions() const
{
  QStringList ret;
  ret << i18n( "Set &Length..." );
  return ret;
}

void SegmentABType::executeAction( int i, ObjectHolder&, ObjectTypeCalcer& c,
                                   KigDocument& d, KigWidget& w, NormalMode& ) const
{
  assert( i == 0 );
  // pretend to use this var..
  (void) i;

  std::vector<ObjectCalcer*> parents = c.parents();
  assert( margsparser.checkArgs( parents ) );

  Coordinate a = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  Coordinate b = static_cast<const PointImp*>( parents[1]->imp() )->coordinate();

  bool ok = true;
  double length = getDoubleFromUser(
    i18n( "Set Segment Length" ), i18n( "Choose the new length: " ),
    (b-a).length(), &w, &ok, -2147483647, 2147483647, 3 );
  if ( ! ok ) return;

  Coordinate nb = a + ( b - a ).normalize( length );

  MonitorDataObjects mon( getAllParents( parents ) );
  parents[1]->move( nb, d );
  KigCommand* cd = new KigCommand( d, i18n( "Resize Segment" ) );
  mon.finish( cd );
  d.history()->addCommand( cd );
}
