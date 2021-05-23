// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "line_type.h"

#include "bogus_imp.h"
#include "line_imp.h"
#include "object_holder.h"
#include "other_imp.h"
#include "point_imp.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../kig/kig_commands.h"
#include "../misc/common.h"
#include "../misc/calcpaths.h"

#include <QStringList>

static const ArgsParser::spec argsspecSegmentAB[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct a segment starting at this point" ),
    I18N_NOOP( "Select the start point of the new segment..." ), true },
  { PointImp::stype(), I18N_NOOP( "Construct a segment ending at this point" ),
    I18N_NOOP( "Select the end point of the new segment..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( SegmentABType )

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

ObjectImp* SegmentABType::calcx( const Coordinate& a, const Coordinate& b ) const
{
  return new SegmentImp( a, b );
}

static const char constructlineabstat[] = I18N_NOOP( "Construct a line through this point" );

static const ArgsParser::spec argsspecLineAB[] =
{
  { PointImp::stype(), constructlineabstat,
    I18N_NOOP( "Select a point for the line to go through..." ), true },
  { PointImp::stype(), constructlineabstat,
    I18N_NOOP( "Select another point for the line to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( LineABType )

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

ObjectImp* LineABType::calcx( const Coordinate& a, const Coordinate& b ) const
{
  return new LineImp( a, b );
}

static const char constructhalflinestartingstat[] = I18N_NOOP( "Construct a half-line starting at this point" );

static const ArgsParser::spec argsspecRayAB[] =
{
  { PointImp::stype(), constructhalflinestartingstat,
    I18N_NOOP( "Select the start point of the new half-line..." ), true },
  { PointImp::stype(), I18N_NOOP( "Construct a half-line through this point" ),
    I18N_NOOP( "Select a point for the half-line to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( RayABType )

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

ObjectImp* RayABType::calcx( const Coordinate& a, const Coordinate& b ) const
{
  return new RayImp( a, b );
}

static const ArgsParser::spec argspecSegmentAxisABType[] =
{
  { SegmentImp::stype(), I18N_NOOP( "Construct the axis of this segment" ),
    I18N_NOOP( "Select the segment of which you want to draw the axis..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( SegmentAxisType );

SegmentAxisType::SegmentAxisType()
  : ArgsParserObjectType( "Segment Axis", argspecSegmentAxisABType, 1 )
{
}

SegmentAxisType::~SegmentAxisType()
{
}

const SegmentAxisType* SegmentAxisType::instance()
{
  static const SegmentAxisType s;
  return &s;
}

ObjectImp* SegmentAxisType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  const SegmentImp * l = static_cast< const SegmentImp * >( args[0] );
  const Coordinate a = l->data().a;
  const Coordinate b = l->data().b;
  const Coordinate mp = ( a + b ) / 2;
  const Coordinate dir( b - a );
  const Coordinate perpPoint = calcPointOnPerpend( dir, mp );
  return new LineImp( mp, perpPoint  );
}

const ObjectImpType* SegmentAxisType::resultId() const
{
  return LineImp::stype();
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
  { AbstractLineImp::stype(), I18N_NOOP( "Construct a line parallel to this line" ),
    I18N_NOOP( "Select a line parallel to the new line..." ), false },
  { PointImp::stype(), I18N_NOOP( "Construct the parallel line through this point" ),
    I18N_NOOP( "Select a point for the new line to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( LineParallelLPType )

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
  { AbstractLineImp::stype(), I18N_NOOP( "Construct a line perpendicular to this line" ),
    I18N_NOOP( "Select a line perpendicular to the new line..." ), false },
  { PointImp::stype(), I18N_NOOP( "Construct a perpendicular line through this point" ),
    I18N_NOOP( "Select a point for the new line to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( LinePerpendLPType )

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
                                   KigPart& d, KigWidget& w, NormalMode& ) const
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
  parents[1]->move( nb, d.document() );
  KigCommand* cd = new KigCommand( d, i18n( "Resize Segment" ) );
  mon.finish( cd );
  d.history()->push( cd );
}

static const ArgsParser::spec argsspecLineByVector[] =
{
  { VectorImp::stype(), I18N_NOOP( "Construct a line by this vector" ),
    I18N_NOOP( "Select a vector in the direction of the new line..." ), true },
  { PointImp::stype(), constructlineabstat,
    I18N_NOOP( "Select a point for the new line to go through..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( LineByVectorType )

LineByVectorType::LineByVectorType()
  : ArgsParserObjectType( "LineByVector", argsspecLineByVector, 2 )
{
}

LineByVectorType::~LineByVectorType()
{
}

const LineByVectorType* LineByVectorType::instance()
{
  static const LineByVectorType s;
  return &s;
}

ObjectImp* LineByVectorType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  const VectorImp& a = *static_cast<const VectorImp*>( args[0] );
  const PointImp& b = *static_cast<const PointImp*>( args[1] );

  return new LineImp(  b.coordinate(), b.coordinate() + a.dir() );
}

const ObjectImpType* LineByVectorType::resultId() const
{
  return LineImp::stype();
}

static const ArgsParser::spec argsspecHalflineByVector[] =
{
  { VectorImp::stype(), I18N_NOOP( "Construct a half-line by this vector" ),
    I18N_NOOP( "Select a vector in the direction of the new half-line..." ), true },
  { PointImp::stype(), constructhalflinestartingstat,
    I18N_NOOP( "Select the start point of the new half-line..." ), true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( HalflineByVectorType )

HalflineByVectorType::HalflineByVectorType()
  : ArgsParserObjectType( "HalflineByVector", argsspecHalflineByVector, 2 )
{
}

HalflineByVectorType::~HalflineByVectorType()
{
}

const HalflineByVectorType* HalflineByVectorType::instance()
{
  static const HalflineByVectorType s;
  return &s;
}

ObjectImp* HalflineByVectorType::calc( const Args& args, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;

  const VectorImp& a = *static_cast<const VectorImp*>( args[0] );
  const PointImp& b = *static_cast<const PointImp*>( args[1] );

  return new RayImp(  b.coordinate(), b.coordinate() + a.dir() );
}

const ObjectImpType* HalflineByVectorType::resultId() const
{
  return RayImp::stype();
}
