// Copyright (C)  2009  Petr Gajdos <pgajdos@suse.cz>

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

#include "bezier_type.h"

#include <math.h>

#include "bogus_imp.h"
#include "line_imp.h"
#include "point_imp.h"
#include "text_imp.h"
#include "bezier_imp.h"
#include "object_calcer.h"

#include "../misc/common.h"

#include <klocale.h>
#include <cmath>
#include <vector>

/*
 * Bézier curve of degree 2
 */

static const char bezier2_constructstatement[] = I18N_NOOP( "Construct a quadric with this control point" );
static const char bezier2_constructstatement2[] = I18N_NOOP( "Select a point to be a control point of the new quadric..." );

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

static const char bezier3_constructstatement[] = I18N_NOOP( "Construct a cubic with this control point" );
static const char bezier3_constructstatement2[] = I18N_NOOP( "Select a point to be a control point of the new cubic..." );

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

  uint npoints = 0;
  for ( uint i = 0; i < count; ++i )
  {
    npoints++;
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

