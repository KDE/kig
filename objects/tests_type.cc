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

#include "tests_type.h"

#include "line_imp.h"
#include "polygon_imp.h"
#include "point_imp.h"
#include "bogus_imp.h"
#include "other_imp.h"

#include <cmath>

static const ArgsParser::spec argsspecAreParallel[] =
{
  { AbstractLineImp::stype(), I18N_NOOP( "Is this line parallel?" ),
    I18N_NOOP( "Select the first of the two possibly parallel lines..." ), false },
  { AbstractLineImp::stype(), I18N_NOOP( "Parallel to this line?" ),
    I18N_NOOP( "Select the other of the two possibly parallel lines..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( AreParallelType )

AreParallelType::AreParallelType()
  : ArgsParserObjectType( "AreParallel",
                         argsspecAreParallel, 2 )
{
}

AreParallelType::~AreParallelType()
{
}

const AreParallelType* AreParallelType::instance()
{
  static const AreParallelType t;
  return &t;
}

ObjectImp* AreParallelType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;
  const LineData& l1 = static_cast<const AbstractLineImp*>( parents[0] )->data();
  const LineData& l2 = static_cast<const AbstractLineImp*>( parents[1] )->data();

  if ( l1.isParallelTo( l2 ) )
    return new TestResultImp( i18n( "These lines are parallel." ) );
  else
    return new TestResultImp( i18n( "These lines are not parallel." ) );

}

const ObjectImpType* AreParallelType::resultId() const
{
  return TestResultImp::stype();
}

static const ArgsParser::spec argsspecAreOrthogonal[] =
{
  { AbstractLineImp::stype(), I18N_NOOP( "Is this line orthogonal?" ),
    I18N_NOOP( "Select the first of the two possibly orthogonal lines..." ), false },
  { AbstractLineImp::stype(), I18N_NOOP( "Orthogonal to this line?" ),
    I18N_NOOP( "Select the other of the two possibly orthogonal lines..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( AreOrthogonalType )

AreOrthogonalType::AreOrthogonalType()
  : ArgsParserObjectType( "AreOrthogonal",
                         argsspecAreOrthogonal, 2 )
{
}

AreOrthogonalType::~AreOrthogonalType()
{
}

const AreOrthogonalType* AreOrthogonalType::instance()
{
  static const AreOrthogonalType t;
  return &t;
}

ObjectImp* AreOrthogonalType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;
  const LineData& l1 = static_cast<const AbstractLineImp*>( parents[0] )->data();
  const LineData& l2 = static_cast<const AbstractLineImp*>( parents[1] )->data();

  if ( l1.isOrthogonalTo( l2 ) )
    return new TestResultImp( i18n( "These lines are orthogonal." ) );
  else
    return new TestResultImp( i18n( "These lines are not orthogonal." ) );

}

const ObjectImpType* AreOrthogonalType::resultId() const
{
  return TestResultImp::stype();
}

static const ArgsParser::spec argsspecAreCollinear[] =
{
  { PointImp::stype(), I18N_NOOP( "Check collinearity of this point" ),
    I18N_NOOP( "Select the first of the three possibly collinear points..." ), false },
  { PointImp::stype(), I18N_NOOP( "and this second point" ),
    I18N_NOOP( "Select the second of the three possibly collinear points..." ), false },
  { PointImp::stype(), I18N_NOOP( "with this third point" ),
    I18N_NOOP( "Select the last of the three possibly collinear points..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( AreCollinearType )

AreCollinearType::AreCollinearType()
  : ArgsParserObjectType( "AreCollinear",
                         argsspecAreCollinear, 3 )
{
}

AreCollinearType::~AreCollinearType()
{
}

const AreCollinearType* AreCollinearType::instance()
{
  static const AreCollinearType t;
  return &t;
}

ObjectImp* AreCollinearType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;
  const Coordinate& p1 = static_cast<const PointImp*>( parents[0] )->coordinate();
  const Coordinate& p2 = static_cast<const PointImp*>( parents[1] )->coordinate();
  const Coordinate& p3 = static_cast<const PointImp*>( parents[2] )->coordinate();

  if ( areCollinear( p1, p2, p3 ) )
    return new TestResultImp( i18n( "These points are collinear." ) );
  else
    return new TestResultImp( i18n( "These points are not collinear." ) );
}

const ObjectImpType* AreCollinearType::resultId() const
{
  return TestResultImp::stype();
}

static const ArgsParser::spec containsTestArgsSpec[] =
{
  { PointImp::stype(), I18N_NOOP( "Check whether this point is on a curve" ),
    I18N_NOOP( "Select the point you want to test..." ), false },
  { CurveImp::stype(), I18N_NOOP( "Check whether the point is on this curve" ),
    I18N_NOOP( "Select the curve that the point might be on..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ContainsTestType )

ContainsTestType::ContainsTestType()
  : ArgsParserObjectType( "ContainsTest", containsTestArgsSpec, 2 )
{
}

ContainsTestType::~ContainsTestType()
{
}

const ContainsTestType* ContainsTestType::instance()
{
  static const ContainsTestType t;
  return &t;
}

ObjectImp* ContainsTestType::calc( const Args& parents, const KigDocument& doc ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;
  const Coordinate& p = static_cast<const PointImp*>( parents[0] )->coordinate();
  const CurveImp* c = static_cast<const CurveImp*>( parents[1] );

  if ( c->containsPoint( p, doc ) )
    return new TestResultImp( i18n( "This curve contains the point." ) );
  else
    return new TestResultImp( i18n( "This curve does not contain the point." ) );
}

const ObjectImpType* ContainsTestType::resultId() const
{
  return TestResultImp::stype();
}

/*
 * containment test of a point in a polygon
 */

static const ArgsParser::spec InPolygonTestArgsSpec[] =
{
  { PointImp::stype(), I18N_NOOP( "Check whether this point is in a polygon" ),
    I18N_NOOP( "Select the point you want to test..." ), false },
  { PolygonImp::stype(), I18N_NOOP( "Check whether the point is in this polygon" ),
    I18N_NOOP( "Select the polygon that the point might be in..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( InPolygonTestType )

InPolygonTestType::InPolygonTestType()
  : ArgsParserObjectType( "InPolygonTest", InPolygonTestArgsSpec, 2 )
{
}

InPolygonTestType::~InPolygonTestType()
{
}

const InPolygonTestType* InPolygonTestType::instance()
{
  static const InPolygonTestType t;
  return &t;
}

ObjectImp* InPolygonTestType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;
  const Coordinate& p = static_cast<const PointImp*>( parents[0] )->coordinate();
  const PolygonImp* pol = static_cast<const PolygonImp*>( parents[1] );

  if ( pol->isInPolygon( p ) )
    return new TestResultImp( i18n( "This polygon contains the point." ) );
  else
    return new TestResultImp( i18n( "This polygon does not contain the point." ) );
}

const ObjectImpType* InPolygonTestType::resultId() const
{
  return TestResultImp::stype();
}

/*
 * test if a polygon is convex
 */

static const ArgsParser::spec ConvexPolygonTestArgsSpec[] =
{
  { PolygonImp::stype(), I18N_NOOP( "Check whether this polygon is convex" ),
    I18N_NOOP( "Select the polygon you want to test for convexity..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ConvexPolygonTestType )

ConvexPolygonTestType::ConvexPolygonTestType()
  : ArgsParserObjectType( "ConvexPolygonTest", ConvexPolygonTestArgsSpec, 1 )
{
}

ConvexPolygonTestType::~ConvexPolygonTestType()
{
}

const ConvexPolygonTestType* ConvexPolygonTestType::instance()
{
  static const ConvexPolygonTestType t;
  return &t;
}

ObjectImp* ConvexPolygonTestType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;
  const PolygonImp* pol = static_cast<const PolygonImp*>( parents[0] );

  if ( pol->isConvex() )
    return new TestResultImp( i18n( "This polygon is convex." ) );
  else
    return new TestResultImp( i18n( "This polygon is not convex." ) );
}

const ObjectImpType* ConvexPolygonTestType::resultId() const
{
  return TestResultImp::stype();
}

/*
 * same distance test
 */

static const ArgsParser::spec argsspecSameDistanceType[] =
{
  { PointImp::stype(), I18N_NOOP( "Check if this point has the same distance" ),
    I18N_NOOP( "Select the point which might have the same distance from two other points..." ), false },
  { PointImp::stype(), I18N_NOOP( "from this point" ),
    I18N_NOOP( "Select the first of the two other points..." ), false },
  { PointImp::stype(), I18N_NOOP( "and from this second point" ),
    I18N_NOOP( "Select the other of the two other points..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( SameDistanceType )

SameDistanceType::SameDistanceType()
  : ArgsParserObjectType( "SameDistanceType", argsspecSameDistanceType, 3 )
{
}

SameDistanceType::~SameDistanceType()
{
}

const SameDistanceType* SameDistanceType::instance()
{
  static const SameDistanceType t;
  return &t;
}

ObjectImp* SameDistanceType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;
  const Coordinate& p1 = static_cast<const PointImp*>( parents[0] )->coordinate();
  const Coordinate& p2 = static_cast<const PointImp*>( parents[1] )->coordinate();
  const Coordinate& p3 = static_cast<const PointImp*>( parents[2] )->coordinate();

  if ( fabs( ( p1 - p2 ).length() - ( p1 - p3 ).length() ) < 10e-5  )
    return new TestResultImp( i18n( "The two distances are the same." ) );
  else
    return new TestResultImp( i18n( "The two distances are not the same." ) );
}

const ObjectImpType* SameDistanceType::resultId() const
{
  return TestResultImp::stype();
}

static const ArgsParser::spec vectorEqualityArgsSpec[] =
{
  { VectorImp::stype(), I18N_NOOP( "Check whether this vector is equal to another vector" ),
    I18N_NOOP( "Select the first of the two possibly equal vectors..." ), false },
  { VectorImp::stype(), I18N_NOOP( "Check whether this vector is equal to the other vector" ),
    I18N_NOOP( "Select the other of the two possibly equal vectors..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( VectorEqualityTestType )

VectorEqualityTestType::VectorEqualityTestType()
  : ArgsParserObjectType( "VectorEquality", vectorEqualityArgsSpec, 2 )
{
}

VectorEqualityTestType::~VectorEqualityTestType()
{
}

const VectorEqualityTestType* VectorEqualityTestType::instance()
{
  static const VectorEqualityTestType t;
  return &t;
}

ObjectImp* VectorEqualityTestType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;
  const Coordinate& v1 = static_cast<const VectorImp*>( parents[0] )->dir();
  const Coordinate& v2 = static_cast<const VectorImp*>( parents[1] )->dir();

  if ( ( v1 - v2 ).length() < 10e-5  )
    return new TestResultImp( i18n( "The two vectors are the same." ) );
  else
    return new TestResultImp( i18n( "The two vectors are not the same." ) );
}

const ObjectImpType* VectorEqualityTestType::resultId() const
{
  return TestResultImp::stype();
}
