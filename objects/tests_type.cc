// tests_type.cc
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "tests_type.h"

#include "line_imp.h"
#include "point_imp.h"
#include "bogus_imp.h"
#include "other_imp.h"

#include <cmath>

static const ArgsParser::spec argsspecAreParallel[] =
{
  { AbstractLineImp::stype(), I18N_NOOP( "Is this line parallel?" ), false },
  { AbstractLineImp::stype(), I18N_NOOP( "Parallel to this line?" ), false }
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
  { AbstractLineImp::stype(), I18N_NOOP( "Is this line orthogonal?" ), false },
  { AbstractLineImp::stype(), I18N_NOOP( "Orthogonal to this line?" ), false }
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
  { PointImp::stype(), I18N_NOOP( "Check collinearity of this point" ), false },
  { PointImp::stype(), I18N_NOOP( "and this second point" ), false },
  { PointImp::stype(), I18N_NOOP( "with this third point" ), false }
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
  { PointImp::stype(), I18N_NOOP( "Check whether this point is on a curve" ), false },
  { CurveImp::stype(), I18N_NOOP( "Check whether the point is on this curve" ), false }
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

static const ArgsParser::spec argsspecSameDistanceType[] =
{
  { PointImp::stype(), I18N_NOOP( "Check if this point have the same distance" ), false },
  { PointImp::stype(), I18N_NOOP( "from this point" ), false },
  { PointImp::stype(), I18N_NOOP( "and from this second point" ), false }
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
  { VectorImp::stype(), I18N_NOOP( "Check whether this vector is equal to another vector" ), false },
  { VectorImp::stype(), I18N_NOOP( "Check whether this vector is equal to the other vector" ), false }
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
