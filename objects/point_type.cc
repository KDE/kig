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

#include "object.h"
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

#include <qstringlist.h>

static const ArgsParser::spec argsspecFixedPoint[] =
{
  { DoubleImp::stype(), "x" },
  { DoubleImp::stype(), "y" }
};

FixedPointType::FixedPointType()
  : ArgparserObjectType( "FixedPoint", argsspecFixedPoint, 1 )
{
}

FixedPointType::~FixedPointType()
{
}

ObjectImp* FixedPointType::calc( const Args& parents, const KigDocument& ) const
{
  assert( parents.size() == 2 );
  assert( parents[0]->inherits( DoubleImp::stype() ) );
  assert( parents[1]->inherits( DoubleImp::stype() ) );
  double a = static_cast<const DoubleImp*>( parents[0] )->data();
  double b = static_cast<const DoubleImp*>( parents[1] )->data();
  PointImp* d = new PointImp( Coordinate( a, b ) );
  return d;
}

ObjectImp* ConstrainedPointType::calc( const Args& tparents, const KigDocument& doc ) const
{
  Args parents = margsparser.parse( tparents );
  if( ! parents[0] || ! parents[1] )
    return new InvalidImp;
  double param = static_cast<const DoubleImp*>( parents[0] )->data();
  bool valid = true;
  const Coordinate nc = static_cast<const CurveImp*>( parents[1] )->getPoint( param, valid, doc );
  if ( valid ) return new PointImp( nc );
  else return new InvalidImp;
}

const ArgsParser::spec argsspecConstrainedPoint[] =
{
  { DoubleImp::stype(), "parameter" },
  { CurveImp::stype(), "Constrain the point to this curve" }
};

ConstrainedPointType::ConstrainedPointType()
  : ArgparserObjectType( "ConstrainedPoint", argsspecConstrainedPoint, 2 )
{
}

ConstrainedPointType::~ConstrainedPointType()
{
}

void FixedPointType::move( RealObject* ourobj, const Coordinate& to,
                           const KigDocument& ) const
{
  // fetch the old coord..;
  Objects pa = ourobj->parents();
  assert( pa.size() == 2 );
  assert( pa.front()->inherits( Object::ID_DataObject ) );
  assert( pa.back()->inherits( Object::ID_DataObject ) );

  DataObject* ox = static_cast<DataObject*>( pa.front() );
  DataObject* oy = static_cast<DataObject*>( pa.back() );

  ox->setImp( new DoubleImp( to.x ) );
  oy->setImp( new DoubleImp( to.y ) );
}

void ConstrainedPointType::move( RealObject* ourobj, const Coordinate& to,
                                 const KigDocument& d ) const
{
  // fetch the CurveImp..
  Objects parents = ourobj->parents();
  assert( parents.size() == 2 );
  const CurveImp* ci = 0;
  if( parents.back()->hasimp( CurveImp::stype() ) )
    ci = static_cast<const CurveImp*>( parents.back()->imp() );
  else ci = static_cast<const CurveImp*>( parents.front()->imp() );

  // fetch the new param..
  const double np = ci->getParam( to, d );

  Object* paramo = 0;
  if ( parents[0]->hasimp( DoubleImp::stype() ) )
    paramo = parents[0];
  else paramo = parents[1];
  assert( paramo->inherits( Object::ID_DataObject ) );
  assert( paramo->hasimp( DoubleImp::stype() ) );

  static_cast<DataObject*>( paramo )->setImp( new DoubleImp( np ) );
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

static void redefinePoint( RealObject* o, KigDocument& d, KigWidget& w,
                           NormalMode& )
{
  PointRedefineMode pm( o, d, w );
  d.runMode( &pm );
}

void FixedPointType::executeAction(
  int i, RealObject* o, KigDocument& d, KigWidget& w,
  NormalMode& m ) const
{
  switch( i )
  {
  case 0:
  {
    bool ok = true;
    Coordinate oldc;
    if ( o->hasimp( PointImp::stype() ) )
      oldc = static_cast<const PointImp*>( o->imp() )->coordinate();
    Coordinate c = d.coordinateSystem().getCoordFromUser(
      i18n( "Set Coordinate" ), i18n( "Enter the new coordinate: " ),
      d, &w, &ok, &oldc );
    if ( ! ok ) break;

    MonitorDataObjects mon( getAllParents( Objects( o ) ) );
    o->move( c, d );
    KigCommand* kc = new KigCommand( d, PointImp::stype()->moveAStatement() );
    kc->addTask( mon.finish() );

    d.history()->addCommand( kc );
    break;
  };
  case 1:
    redefinePoint( o, d, w, m );
    break;
  default:
    assert( false );
  };
}

void ConstrainedPointType::executeAction(
  int i, RealObject* o, KigDocument& d, KigWidget& w,
  NormalMode& m ) const
{
  switch( i )
  {
  case 1:
    redefinePoint( o, d, w, m );
    break;
  case 0:
  {
    Objects parents = margsparser.parse( o->parents() );
    if ( parents[0]->inherits( Object::ID_DataObject ) &&
         parents[0]->hasimp( DoubleImp::stype() ) )
    {
      DataObject* po = static_cast<DataObject*>( parents[0] );
      double oldp = static_cast<const DoubleImp*>( po->imp() )->data();

      bool ok = true;
      double newp = getDoubleFromUser(
        i18n( "Set Point Parameter" ), i18n( "Choose the new parameter: " ),
        oldp, &w, &ok, 0, 1, 4 );
      if ( ! ok ) return;

      Objects monos( po );
      MonitorDataObjects mon( monos );
      po->setImp( new DoubleImp( newp ) );
      KigCommand* kc = new KigCommand( d, i18n( "Change Parameter of Constrained Point" ) );
      kc->addTask( mon.finish() );
      d.history()->addCommand( kc );
    };
    break;
  };
  default:
    assert( false );
  };
}

const Coordinate FixedPointType::moveReferencePoint( const RealObject* ourobj ) const
{
  if ( ourobj->hasimp( PointImp::stype() ) )
    return static_cast<const PointImp*>( ourobj->imp() )->coordinate();
  else return Coordinate::invalidCoord();
}

const Coordinate ConstrainedPointType::moveReferencePoint( const RealObject* ourobj ) const
{
  if ( ourobj->hasimp( PointImp::stype() ) )
    return static_cast<const PointImp*>( ourobj->imp() )->coordinate();
  else return Coordinate::invalidCoord();
}
