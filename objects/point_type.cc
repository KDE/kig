// point_type.cc
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

#include "point_type.h"

#include "point_imp.h"
#include "curve_imp.h"
#include "line_imp.h"
#include "bogus_imp.h"

#include "../modes/moving.h"
#include "../misc/coordinate_system.h"
#include "../misc/common.h"
#include "../misc/calcpaths.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../kig/kig_commands.h"

#include <klocale.h>

static const ArgsParser::spec argsspecFixedPoint[] =
{
  { DoubleImp::stype(), "x" },
  { DoubleImp::stype(), "y" }
};

FixedPointType::FixedPointType()
  : ArgsParserObjectType( "FixedPoint", argsspecFixedPoint, 2 )
{
}

FixedPointType::~FixedPointType()
{
}

ObjectImp* FixedPointType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  double a = static_cast<const DoubleImp*>( parents[0] )->data();
  double b = static_cast<const DoubleImp*>( parents[1] )->data();

  return new PointImp( Coordinate( a, b ) );
}

ObjectImp* ConstrainedPointType::calc( const Args& parents, const KigDocument& doc ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  double param = static_cast<const DoubleImp*>( parents[0] )->data();
  const Coordinate nc = static_cast<const CurveImp*>( parents[1] )->getPoint( param, doc );
  if ( nc.valid() ) return new PointImp( nc );
  else return new InvalidImp;
}

const ArgsParser::spec argsspecConstrainedPoint[] =
{
  { DoubleImp::stype(), "parameter" },
  { CurveImp::stype(), "Constrain the point to this curve" }
};

ConstrainedPointType::ConstrainedPointType()
  : ArgsParserObjectType( "ConstrainedPoint", argsspecConstrainedPoint, 2 )
{
}

ConstrainedPointType::~ConstrainedPointType()
{
}

void FixedPointType::move( ObjectTypeCalcer& ourobj, const Coordinate& to,
                           const KigDocument& ) const
{
  // fetch the old coord..;
  std::vector<ObjectCalcer*> pa = ourobj.parents();
  assert( margsparser.checkArgs( pa ) );
  assert( dynamic_cast<ObjectConstCalcer*>( pa.front() ) );
  assert( dynamic_cast<ObjectConstCalcer*>( pa.back() ) );

  ObjectConstCalcer* ox = static_cast<ObjectConstCalcer*>( pa.front() );
  ObjectConstCalcer* oy = static_cast<ObjectConstCalcer*>( pa.back() );

  ox->setImp( new DoubleImp( to.x ) );
  oy->setImp( new DoubleImp( to.y ) );
}

void ConstrainedPointType::move( ObjectTypeCalcer& ourobj, const Coordinate& to,
                                 const KigDocument& d ) const
{
  // fetch the CurveImp..
  std::vector<ObjectCalcer*> parents = ourobj.parents();
  assert( margsparser.checkArgs( parents ) );

  assert( dynamic_cast<ObjectConstCalcer*>( parents[0] ) );
  ObjectConstCalcer* paramo = static_cast<ObjectConstCalcer*>( parents[0] );
  const CurveImp* ci = static_cast<const CurveImp*>( parents[1]->imp() );

  // fetch the new param..
  const double np = ci->getParam( to, d );

  paramo->setImp( new DoubleImp( np ) );
}

bool ConstrainedPointType::canMove() const
{
  return true;
}

bool FixedPointType::canMove() const
{
  return true;
}

static const ArgsParser::spec argsspecMidPoint[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct the midpoint of this point" ) },
  { PointImp::stype(), I18N_NOOP( "Construct the midpoint of this point" ) }
};

MidPointType::MidPointType()
  : ObjectABType( "MidPoint", argsspecMidPoint, 2 )
{
}

MidPointType::~MidPointType()
{
}

const MidPointType* MidPointType::instance()
{
  static const MidPointType t;
  return &t;
}

ObjectImp* MidPointType::calc( const Coordinate& a, const Coordinate& b ) const
{
  return new PointImp( ( a + b ) / 2 );
}

bool ConstrainedPointType::inherits( int type ) const
{
  return type == ID_ConstrainedPointType;
}

const ConstrainedPointType* ConstrainedPointType::instance()
{
  static const ConstrainedPointType t;
  return &t;
}

const FixedPointType* FixedPointType::instance()
{
  static const FixedPointType t;
  return &t;
}

const ObjectImpType* FixedPointType::resultId() const
{
  return PointImp::stype();
}

const ObjectImpType* ConstrainedPointType::resultId() const
{
  return PointImp::stype();
}

const ObjectImpType* MidPointType::resultId() const
{
  return PointImp::stype();
}

QStringList FixedPointType::specialActions() const
{
  QStringList ret;
  ret << i18n( "Set &Coordinate..." );
  ret << i18n( "Redefine" );
  return ret;
}

QStringList ConstrainedPointType::specialActions() const
{
  QStringList ret;
  ret << i18n( "Set &Parameter..." );
  ret << i18n( "Redefine" );
  return ret;
}

static void redefinePoint( ObjectHolder* o, KigDocument& d, KigWidget& w )
{
  PointRedefineMode pm( o, d, w );
  d.runMode( &pm );
}

void FixedPointType::executeAction(
  int i, ObjectHolder& oh, ObjectTypeCalcer& o,
  KigDocument& d, KigWidget& w, NormalMode& ) const
{
  switch( i )
  {
  case 0:
  {
    bool ok = true;
    assert ( o.imp()->inherits( PointImp::stype() ) );
    Coordinate oldc = static_cast<const PointImp*>( o.imp() )->coordinate();
    Coordinate c = d.coordinateSystem().getCoordFromUser(
      i18n( "Set Coordinate" ), i18n( "Enter the new coordinate: " ),
      d, &w, &ok, &oldc );
    if ( ! ok ) break;

    MonitorDataObjects mon( getAllParents( &o ) );
    o.move( c, d );
    KigCommand* kc = new KigCommand( d, PointImp::stype()->moveAStatement() );
    mon.finish( kc );

    d.history()->addCommand( kc );
    break;
  };
  case 1:
    redefinePoint( &oh, d, w );
    break;
  default:
    assert( false );
  };
}

void ConstrainedPointType::executeAction(
  int i, ObjectHolder& oh, ObjectTypeCalcer& o, KigDocument& d, KigWidget& w,
  NormalMode& ) const
{
  switch( i )
  {
  case 1:
    redefinePoint( &oh, d, w );
    break;
  case 0:
  {
    std::vector<ObjectCalcer*> parents = o.parents();
    assert( dynamic_cast<ObjectConstCalcer*>( parents[0] ) &&
            parents[0]->imp()->inherits( DoubleImp::stype() ) );

    ObjectConstCalcer* po = static_cast<ObjectConstCalcer*>( parents[0] );
    double oldp = static_cast<const DoubleImp*>( po->imp() )->data();

    bool ok = true;
    double newp = getDoubleFromUser(
      i18n( "Set Point Parameter" ), i18n( "Choose the new parameter: " ),
      oldp, &w, &ok, 0, 1, 4 );
    if ( ! ok ) return;

    MonitorDataObjects mon( parents );
    po->setImp( new DoubleImp( newp ) );
    KigCommand* kc = new KigCommand( d, i18n( "Change Parameter of Constrained Point" ) );
    mon.finish( kc );
    d.history()->addCommand( kc );
    break;
  };
  default:
    assert( false );
  };
}

const Coordinate FixedPointType::moveReferencePoint( const ObjectTypeCalcer& ourobj ) const
{
  assert( ourobj.imp()->inherits( PointImp::stype() ) );
  return static_cast<const PointImp*>( ourobj.imp() )->coordinate();
}

const Coordinate ConstrainedPointType::moveReferencePoint( const ObjectTypeCalcer& ourobj ) const
{
  assert( ourobj.imp()->inherits( PointImp::stype() ) );
  return static_cast<const PointImp*>( ourobj.imp() )->coordinate();
}

std::vector<ObjectCalcer*> FixedPointType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  return ourobj.parents();
}

std::vector<ObjectCalcer*> ConstrainedPointType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> ret;
  ret.push_back( ourobj.parents()[0] );
  return ret;
}

ObjectImp* MeasureTransportType::calc( const Args& parents, const KigDocument& doc ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  const CircleImp* c = static_cast<const CircleImp*>( parents[0] );
  const PointImp* p = static_cast<const PointImp*>( parents[1] );
  const SegmentImp* s = static_cast<const SegmentImp*>( parents[2] );
  double param = c->getParam( p->coordinate(), doc );
  double measure = s->length();
  measure /= 2*c->radius()*M_PI;
  param += measure;
  while (param > 1) param -= 1;

//  const Coordinate nc = static_cast<const CurveImp*>c->getPoint( param, doc );
  const Coordinate nc = c->getPoint( param, doc );
  if ( nc.valid() ) return new PointImp( nc );
  else return new InvalidImp;
}

const ArgsParser::spec argsspecMeasureTransport[] =
{
  { CircleImp::stype(), "transport measure on this circle" },
  { PointImp::stype(), "project this point onto the circle" },
  { SegmentImp::stype(), "Segment to transport" }
};

MeasureTransportType::MeasureTransportType()
  : ArgsParserObjectType( "MeasureTransport", argsspecMeasureTransport, 3 )
{
}

MeasureTransportType::~MeasureTransportType()
{
}

bool MeasureTransportType::canMove() const
{
  return false;
}

bool MeasureTransportType::inherits( int type ) const
{
  return type == ID_ConstrainedPointType;
}

const MeasureTransportType* MeasureTransportType::instance()
{
  static const MeasureTransportType t;
  return &t;
}

const ObjectImpType* MeasureTransportType::resultId() const
{
  return PointImp::stype();
}
