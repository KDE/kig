// SPDX-FileCopyrightText: 2009 Petr Gajdos <pgajdos@suse.cz>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "bezier_type.h"

#include <math.h>

#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"
#include "text_imp.h"
#include "bezier_imp.h"
#include "object_calcer.h"
#include "special_imptypes.h"

#include "../misc/common.h"

#include <cmath>
#include <vector>

/*
 * Bézier curve of degree 2
 */

static const char bezier2_constructstatement[] = I18N_NOOP( "Construct a quadratic Bézier curve with this control point" );
static const char bezier2_constructstatement2[] = I18N_NOOP( "Select a point to be a control point of the new quadratic Bézier curve..." );

static const struct ArgsParser::spec argsspecBezier2[] =
{
  // mp: the central control point is not on the curve
  { PointImp::stype(), bezier2_constructstatement, bezier2_constructstatement2, true },
  { PointImp::stype(), bezier2_constructstatement, bezier2_constructstatement2, false },
  { PointImp::stype(), bezier2_constructstatement, bezier2_constructstatement2, true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( BezierQuadricType )

BezierQuadricType::BezierQuadricType()
  : ArgsParserObjectType( "BezierQuadric", argsspecBezier2, 3 )
{
}

BezierQuadricType::~BezierQuadricType()
{
}

const BezierQuadricType* BezierQuadricType::instance()
{
  static const BezierQuadricType s;
  return &s;
}

ObjectImp* BezierQuadricType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents, 3 ) ) return new InvalidImp;
  std::vector<Coordinate> points;

  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
  {
    Coordinate point = static_cast<const PointImp*>( *i )->coordinate();
    points.push_back( point );
  }
  return new BezierImp( points );
}

const ObjectImpType* BezierQuadricType::resultId() const
{
  return BezierImp::stype2();
}

bool BezierQuadricType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
}

bool BezierQuadricType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return parents[0]->isFreelyTranslatable() && 
         parents[1]->isFreelyTranslatable() &&
         parents[2]->isFreelyTranslatable();
}

void BezierQuadricType::move( ObjectTypeCalcer& o, const Coordinate& to,
                         const KigDocument& d ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  const Coordinate a = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  const Coordinate b = static_cast<const PointImp*>( parents[1]->imp() )->coordinate();
  const Coordinate c = static_cast<const PointImp*>( parents[2]->imp() )->coordinate();
  if ( parents[0]->canMove() )
    parents[0]->move( to, d );
  if ( parents[1]->canMove() )
    parents[1]->move( to + b - a, d );
  if ( parents[2]->canMove() )
    parents[2]->move( to + c - a, d );
}

const Coordinate BezierQuadricType::moveReferencePoint( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> BezierQuadricType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  std::set<ObjectCalcer*> ret;
  std::vector<ObjectCalcer*> tmp = parents[0]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[1]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[2]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  ret.insert( parents.begin(), parents.end() );
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}

/*
 * Bézier curve of degree 3
 */

static const char bezier3_constructstatement[] = I18N_NOOP( "Construct a cubic Bézier curve with this control point" );
static const char bezier3_constructstatement2[] = I18N_NOOP( "Select a point to be a control point of the new cubic Bézier curve..." );

static const struct ArgsParser::spec argsspecBezier3[] =
{
  { PointImp::stype(), bezier3_constructstatement, bezier3_constructstatement2, true },
  { PointImp::stype(), bezier3_constructstatement, bezier3_constructstatement2, false },
  { PointImp::stype(), bezier3_constructstatement, bezier3_constructstatement2, false },
  { PointImp::stype(), bezier3_constructstatement, bezier3_constructstatement2, true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( BezierCubicType )

BezierCubicType::BezierCubicType()
  : ArgsParserObjectType( "BezierCubic", argsspecBezier3, 4 )
{
}

BezierCubicType::~BezierCubicType()
{
}

const BezierCubicType* BezierCubicType::instance()
{
  static const BezierCubicType s;
  return &s;
}

ObjectImp* BezierCubicType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents, 4 ) ) return new InvalidImp;
  std::vector<Coordinate> points;

  for ( Args::const_iterator i = parents.begin(); i != parents.end(); ++i )
  {
    Coordinate point = static_cast<const PointImp*>( *i )->coordinate();
    points.push_back( point );
  }
  return new BezierImp( points );
}

const ObjectImpType* BezierCubicType::resultId() const
{
  return BezierImp::stype3();
}

bool BezierCubicType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
}

bool BezierCubicType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return parents[0]->isFreelyTranslatable() && 
         parents[1]->isFreelyTranslatable() &&
         parents[2]->isFreelyTranslatable() &&
         parents[3]->isFreelyTranslatable();
}

void BezierCubicType::move( ObjectTypeCalcer& o, const Coordinate& to,
                         const KigDocument& d ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  const Coordinate a = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  const Coordinate b = static_cast<const PointImp*>( parents[1]->imp() )->coordinate();
  const Coordinate c = static_cast<const PointImp*>( parents[2]->imp() )->coordinate();
  const Coordinate e = static_cast<const PointImp*>( parents[3]->imp() )->coordinate();
  if ( parents[0]->canMove() )
    parents[0]->move( to, d );
  if ( parents[1]->canMove() )
    parents[1]->move( to + b - a, d );
  if ( parents[2]->canMove() )
    parents[2]->move( to + c - a, d );
  if ( parents[3]->canMove() )
    parents[3]->move( to + e - a, d );
}

const Coordinate BezierCubicType::moveReferencePoint( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> BezierCubicType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  std::set<ObjectCalcer*> ret;
  std::vector<ObjectCalcer*> tmp = parents[0]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[1]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[2]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  ret.insert( parents.begin(), parents.end() );
  tmp = parents[3]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  ret.insert( parents.begin(), parents.end() );
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}

/*
 * generic Bézier curve
 */

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( BezierCurveType )

BezierCurveType::BezierCurveType()
  : ObjectType( "BezierCurve" )
{
}

BezierCurveType::~BezierCurveType()
{
}

const BezierCurveType* BezierCurveType::instance()
{
  static const BezierCurveType s;
  return &s;
}

ObjectImp* BezierCurveType::calc( const Args& parents, const KigDocument& ) const
{
  uint count = parents.size();
  assert (count >= 3);
//  if ( parents[0] != parents[count] ) return new InvalidImp;
  std::vector<Coordinate> points;

  for ( uint i = 0; i < count; ++i )
  {
    if ( ! parents[i]->inherits( PointImp::stype() ) ) return new InvalidImp;
    Coordinate point = static_cast<const PointImp*>( parents[i] )->coordinate();
    points.push_back( point );
  }
  return new BezierImp( points );
}

const ObjectImpType* BezierCurveType::resultId() const
{
  return BezierImp::stype();
}

const ObjectImpType* BezierCurveType::impRequirement( const ObjectImp*, const Args& ) const
{
  return PointImp::stype();
}

bool BezierCurveType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  // TODO: this should result as true if this is the first or last control point, false otherwise
  // in the meantime it is better to leave it as false
  return false;  /* should be true? */
}

std::vector<ObjectCalcer*> BezierCurveType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  return args;  /* should already be in correct order */
}

Args BezierCurveType::sortArgs( const Args& args ) const
{
  return args;
}

bool BezierCurveType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
}

bool BezierCurveType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  for ( uint i = 0; i < parents.size(); ++i )
  {
    if ( !parents[i]->isFreelyTranslatable() ) return false;
  }
  return true;
}

void BezierCurveType::move( ObjectTypeCalcer& o, const Coordinate& to,
                         const KigDocument& d ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  const Coordinate ref = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  for ( uint i = 0; i < parents.size(); ++i )
  {
     const Coordinate a = static_cast<const PointImp*>( parents[i]->imp() )->coordinate();
     parents[i]->move( to + a - ref, d );
  }
}

const Coordinate BezierCurveType::moveReferencePoint( const ObjectTypeCalcer& o
) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> BezierCurveType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  std::set<ObjectCalcer*> ret;
  for ( uint i = 0; i < parents.size(); ++i )
  {
    std::vector<ObjectCalcer*> tmp = parents[i]->movableParents();
    ret.insert( tmp.begin(), tmp.end() );
  }
  ret.insert( parents.begin(), parents.end() );
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}

/*
 * Rational Bézier curve of degree 2
 */

static const char rbezier2_constructstatement[] = I18N_NOOP( "Construct a quadratic rational Bézier curve with this control point" );
static const char rbezier2_constructstatement2[] = I18N_NOOP( "Select three points and three numeric values as weights to construct rational Bézier curve..." );
static const char rbezier2_constructstatement3[] = I18N_NOOP( "Select this value as weight" );


static const struct ArgsParser::spec argsspecRationalBezier2[] =
{
  { PointImp::stype(), rbezier2_constructstatement, rbezier2_constructstatement2, true },
  { &weightimptypeinstance, rbezier2_constructstatement3, rbezier2_constructstatement2, false },
  { PointImp::stype(), rbezier2_constructstatement, rbezier2_constructstatement2, false },
  { &weightimptypeinstance, rbezier2_constructstatement3, rbezier2_constructstatement2, false },
  { PointImp::stype(), rbezier2_constructstatement, rbezier2_constructstatement2, true },
  { &weightimptypeinstance, rbezier2_constructstatement3, rbezier2_constructstatement2, false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( RationalBezierQuadricType )

RationalBezierQuadricType::RationalBezierQuadricType()
  : ArgsParserObjectType( "RationalBezierQuadric", argsspecRationalBezier2, 6 )
{
}

RationalBezierQuadricType::~RationalBezierQuadricType()
{
}

const RationalBezierQuadricType* RationalBezierQuadricType::instance()
{
  static const RationalBezierQuadricType s;
  return &s;
}

ObjectImp* RationalBezierQuadricType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents, 6 ) ) return new InvalidImp;
  std::vector<Coordinate> points;
  std::vector<double> weights;

  for ( uint i = 0; i < 6; i += 2 )
  {
    bool valid;
    Coordinate point = static_cast<const PointImp*>( parents[i] )->coordinate();
    points.push_back( point );
    double value = getDoubleFromImp( parents[i+1], valid );
    if ( ! valid ) return new InvalidImp;
    weights.push_back( value );
  }

  return new RationalBezierImp( points, weights );
}

const ObjectImpType* RationalBezierQuadricType::resultId() const
{
  return RationalBezierImp::stype2();
}

bool RationalBezierQuadricType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
}

bool RationalBezierQuadricType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return parents[0]->isFreelyTranslatable() && 
         parents[2]->isFreelyTranslatable() &&
         parents[4]->isFreelyTranslatable();
}

void RationalBezierQuadricType::move( ObjectTypeCalcer& o, const Coordinate& to,
                                      const KigDocument& d ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  const Coordinate a = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  const Coordinate b = static_cast<const PointImp*>( parents[2]->imp() )->coordinate();
  const Coordinate c = static_cast<const PointImp*>( parents[4]->imp() )->coordinate();

  if ( parents[0]->canMove() )
    parents[0]->move( to, d );
  if ( parents[2]->canMove() )
    parents[2]->move( to + b - a, d );
  if ( parents[4]->canMove() )
    parents[4]->move( to + c - a, d );
}

const Coordinate RationalBezierQuadricType::moveReferencePoint( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> RationalBezierQuadricType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  std::set<ObjectCalcer*> ret;
  std::vector<ObjectCalcer*> tmp = parents[0]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[2]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[4]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  ret.insert( parents.begin(), parents.end() );
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}

/*
 * Rational Bézier curve of degree 3
 */

static const char rbezier3_constructstatement[] = I18N_NOOP( "Construct a cubic rational Bézier curve with this control point" );
static const char rbezier3_constructstatement2[] = I18N_NOOP( "Select three points and three numeric values as weights to construct rational Bézier curve..." );
static const char rbezier3_constructstatement3[] = I18N_NOOP( "Select this value as weight" );


static const struct ArgsParser::spec argsspecRationalBezier3[] =
{
  { PointImp::stype(), rbezier3_constructstatement, rbezier3_constructstatement2, true },
  { &weightimptypeinstance, rbezier3_constructstatement3, rbezier3_constructstatement2, false },
  { PointImp::stype(), rbezier3_constructstatement, rbezier3_constructstatement2, false },
  { &weightimptypeinstance, rbezier3_constructstatement3, rbezier3_constructstatement2, false },
  { PointImp::stype(), rbezier3_constructstatement, rbezier3_constructstatement2, false },
  { &weightimptypeinstance, rbezier3_constructstatement3, rbezier3_constructstatement2, false },
  { PointImp::stype(), rbezier3_constructstatement, rbezier3_constructstatement2, true },
  { &weightimptypeinstance, rbezier3_constructstatement3, rbezier3_constructstatement2, false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( RationalBezierCubicType )

RationalBezierCubicType::RationalBezierCubicType()
  : ArgsParserObjectType( "RationalBezierCubic", argsspecRationalBezier3, 8 )
{
}

RationalBezierCubicType::~RationalBezierCubicType()
{
}

const RationalBezierCubicType* RationalBezierCubicType::instance()
{
  static const RationalBezierCubicType s;
  return &s;
}

ObjectImp* RationalBezierCubicType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents, 8 ) ) return new InvalidImp;
  std::vector<Coordinate> points;
  std::vector<double> weights;

  for ( uint i = 0; i < 8; i += 2 )
  {
    bool valid;
    Coordinate point = static_cast<const PointImp*>( parents[i] )->coordinate();
    points.push_back( point );
    double value = getDoubleFromImp( parents[i+1], valid );
    if ( ! valid ) return new InvalidImp;
    weights.push_back( value );
  }

  return new RationalBezierImp( points, weights );
}

const ObjectImpType* RationalBezierCubicType::resultId() const
{
  return RationalBezierImp::stype3();
}

bool RationalBezierCubicType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
}

bool RationalBezierCubicType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return parents[0]->isFreelyTranslatable() && 
         parents[2]->isFreelyTranslatable() &&
         parents[4]->isFreelyTranslatable() &&
         parents[6]->isFreelyTranslatable();
}

void RationalBezierCubicType::move( ObjectTypeCalcer& o, const Coordinate& to,
                         const KigDocument& d ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  const Coordinate a = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  const Coordinate b = static_cast<const PointImp*>( parents[2]->imp() )->coordinate();
  const Coordinate c = static_cast<const PointImp*>( parents[4]->imp() )->coordinate();
  const Coordinate e = static_cast<const PointImp*>( parents[6]->imp() )->coordinate();
  if ( parents[0]->canMove() )
    parents[0]->move( to, d );
  if ( parents[2]->canMove() )
    parents[2]->move( to + b - a, d );
  if ( parents[4]->canMove() )
    parents[4]->move( to + c - a, d );
  if ( parents[6]->canMove() )
    parents[6]->move( to + e - a, d );
}

const Coordinate RationalBezierCubicType::moveReferencePoint( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  assert( margsparser.checkArgs( parents ) );
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> RationalBezierCubicType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  std::set<ObjectCalcer*> ret;
  std::vector<ObjectCalcer*> tmp = parents[0]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[2]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  tmp = parents[4]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  ret.insert( parents.begin(), parents.end() );
  tmp = parents[6]->movableParents();
  ret.insert( tmp.begin(), tmp.end() );
  ret.insert( parents.begin(), parents.end() );
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}


/*
 * generic rational Bézier curve
 */

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( RationalBezierCurveType )

RationalBezierCurveType::RationalBezierCurveType()
  : ObjectType( "RationalBezierCurve" )
{
}

RationalBezierCurveType::~RationalBezierCurveType()
{
}

const RationalBezierCurveType* RationalBezierCurveType::instance()
{
  static const RationalBezierCurveType s;
  return &s;
}

ObjectImp* RationalBezierCurveType::calc( const Args& parents, const KigDocument& ) const
{
  uint count = parents.size();
  std::vector<Coordinate> points;
  std::vector<double> weights;

  assert ( count % 2 == 0 );
 
  for ( uint i = 0; i < count; i += 2 )
  {
    bool valid;
    Coordinate point = static_cast<const PointImp*>( parents[i] )->coordinate();
    points.push_back( point );
    double value = getDoubleFromImp( parents[i+1], valid );
    if ( ! valid ) return new InvalidImp;
    weights.push_back( value );
  }

  return new RationalBezierImp( points, weights );
}

const ObjectImpType* RationalBezierCurveType::resultId() const
{
  return RationalBezierImp::stype();
}

const ObjectImpType* RationalBezierCurveType::impRequirement( const ObjectImp*, const Args& ) const
{
  return PointImp::stype();
}

bool RationalBezierCurveType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  // TODO: this should result as true if this is the first or last control point, false otherwise
  // in the meantime it is better to leave it as false
  return false;  /* should be true? */
}

std::vector<ObjectCalcer*> RationalBezierCurveType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  return args;  /* should already be in correct order */
}

Args RationalBezierCurveType::sortArgs( const Args& args ) const
{
  return args;
}

bool RationalBezierCurveType::canMove( const ObjectTypeCalcer& o ) const
{
  return isFreelyTranslatable( o );
}

bool RationalBezierCurveType::isFreelyTranslatable( const ObjectTypeCalcer& o ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  for ( uint i = 0; i < parents.size(); i += 2 )
  {
    if ( !parents[i]->isFreelyTranslatable() ) return false;
  }
  return true;
}

void RationalBezierCurveType::move( ObjectTypeCalcer& o, const Coordinate& to,
                                 const KigDocument& d ) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  const Coordinate ref = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  for ( uint i = 0; i < parents.size(); i += 2 )
  {
     const Coordinate a = static_cast<const PointImp*>( parents[i]->imp() )->coordinate();
     parents[i]->move( to + a - ref, d );
  }
}

const Coordinate RationalBezierCurveType::moveReferencePoint( const ObjectTypeCalcer& o
) const
{
  std::vector<ObjectCalcer*> parents = o.parents();
  return static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
}

std::vector<ObjectCalcer*> RationalBezierCurveType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  std::set<ObjectCalcer*> ret;
  for ( uint i = 0; i < parents.size(); i += 2 )
  {
    std::vector<ObjectCalcer*> tmp = parents[i]->movableParents();
    ret.insert( tmp.begin(), tmp.end() );
  }
  ret.insert( parents.begin(), parents.end() );
  return std::vector<ObjectCalcer*>( ret.begin(), ret.end() );
}

