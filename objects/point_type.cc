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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "point_type.h"

#include "point_imp.h"
#include "curve_imp.h"
#include "line_imp.h"
#include "other_imp.h"
#include "bogus_imp.h"

#include "../modes/moving.h"
#include "../misc/coordinate_system.h"
#include "../misc/common.h"
#include "../misc/calcpaths.h"
#include "../misc/kiginputdialog.h"
#include "../kig/kig_part.h"
#include "../kig/kig_document.h"
#include "../kig/kig_view.h"
#include "../kig/kig_commands.h"

#include <klocale.h>

static const ArgsParser::spec argsspecFixedPoint[] =
{
  { DoubleImp::stype(), "x", "SHOULD NOT BE SEEN", false },
  { DoubleImp::stype(), "y", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( FixedPointType )

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

static const ArgsParser::spec argsspecRelativePoint[] =
{
  { DoubleImp::stype(), "relative-x", "SHOULD NOT BE SEEN", false },
  { DoubleImp::stype(), "relative-y", "SHOULD NOT BE SEEN", false },
  { ObjectImp::stype(), "object", "SHOULD NOT BE SEEN", false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( RelativePointType )

RelativePointType::RelativePointType()
  : ArgsParserObjectType( "RelativePoint", argsspecRelativePoint, 3 )
{
}

RelativePointType::~RelativePointType()
{
}

ObjectImp* RelativePointType::calc( const Args& parents, const KigDocument& ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;
  if ( ! parents[2]->attachPoint().valid() ) return new InvalidImp;

  Coordinate reference = static_cast<const ObjectImp*>( parents[2] )->attachPoint();
  double a = static_cast<const DoubleImp*>( parents[0] )->data();
  double b = static_cast<const DoubleImp*>( parents[1] )->data();

  return new PointImp( reference + Coordinate( a, b ) );
}

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CursorPointType )

CursorPointType::CursorPointType()
  : ObjectType( "CursorPoint" )
{
}

CursorPointType::~CursorPointType()
{
}

const CursorPointType* CursorPointType::instance()
{
  static const CursorPointType t;
  return &t;
}

ObjectImp* CursorPointType::calc( const Args& parents, const KigDocument& ) const
{
  assert ( parents[0]->inherits( DoubleImp::stype() ) );
  assert ( parents[1]->inherits( DoubleImp::stype() ) );
  double a = static_cast<const DoubleImp*>( parents[0] )->data();
  double b = static_cast<const DoubleImp*>( parents[1] )->data();

  return new BogusPointImp( Coordinate( a, b ) );
}

const ObjectImpType* CursorPointType::resultId() const
{
  return BogusPointImp::stype();
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
  { DoubleImp::stype(), "parameter", "SHOULD NOT BE SEEN", false },
  { CurveImp::stype(), "Constrain the point to this curve", "SHOULD NOT BE SEEN", true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( ConstrainedPointType )

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

void RelativePointType::move( ObjectTypeCalcer& ourobj, const Coordinate& to,
                           const KigDocument& ) const
{
  // fetch the attach point..;
  // this routine is tightly paired with what moveReferencePoint returns!
  // right now moveReferencePoint always returns the origin
  std::vector<ObjectCalcer*> pa = ourobj.parents();
  assert( margsparser.checkArgs( pa ) );
  assert( dynamic_cast<ObjectConstCalcer*>( pa[0] ) );
  assert( dynamic_cast<ObjectConstCalcer*>( pa[1] ) );

  ObjectConstCalcer* ox = static_cast<ObjectConstCalcer*>( pa[0] );
  ObjectConstCalcer* oy = static_cast<ObjectConstCalcer*>( pa[1] );
  ObjectCalcer* ob = static_cast<ObjectCalcer*>( pa[2] );

  Coordinate attach = ob->imp()->attachPoint();
  ox->setImp( new DoubleImp( to.x - attach.x ) );
  oy->setImp( new DoubleImp( to.y - attach.y ) );
}

void CursorPointType::move( ObjectTypeCalcer& ourobj, const Coordinate& to,
                           const KigDocument& ) const
{
  // fetch the old coord..;

  std::vector<ObjectCalcer*> pa = ourobj.parents();
  assert( pa.size() == 2 );
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

bool ConstrainedPointType::canMove( const ObjectTypeCalcer& ) const
{
  return true;
}

bool ConstrainedPointType::isFreelyTranslatable( const ObjectTypeCalcer& ) const
{
  return false;
}

bool FixedPointType::canMove( const ObjectTypeCalcer& ) const
{
  return true;
}

bool FixedPointType::isFreelyTranslatable( const ObjectTypeCalcer& ) const
{
  return true;
}

bool RelativePointType::canMove( const ObjectTypeCalcer& ) const
{
  return true;
}

bool RelativePointType::isFreelyTranslatable( const ObjectTypeCalcer& ) const
{
  return true;
}

bool CursorPointType::canMove( const ObjectTypeCalcer& ) const
{
  return true;
}

static const ArgsParser::spec argsspecMidPoint[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct the midpoint of this point and another point" ),
    I18N_NOOP( "Select the first of the two points of which you want to construct the midpoint..." ), false },
  { PointImp::stype(), I18N_NOOP( "Construct the midpoint of this point and another point" ),
    I18N_NOOP( "Select the other of the two points of which you want to construct the midpoint..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( MidPointType )

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

bool FixedPointType::inherits( int type ) const
{
  return type == ID_FixedPointType;
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

const RelativePointType* RelativePointType::instance()
{
  static const RelativePointType t;
  return &t;
}

const ObjectImpType* RelativePointType::resultId() const
{
  return PointImp::stype();
}

const ObjectImpType* ConstrainedPointType::resultId() const
{
  return PointImp::stype();
}

const ObjectImpType* CursorPointType::impRequirement( const ObjectImp* o, const Args& ) const
{
  if ( o->inherits( DoubleImp::stype() ) )
    return DoubleImp::stype();

  if ( o->inherits( PointImp::stype() ) )
    return PointImp::stype();

  return 0;
}

bool CursorPointType::isDefinedOnOrThrough( const ObjectImp*, const Args& ) const
{
  return false;
}

std::vector<ObjectCalcer*> CursorPointType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  return args;
}

Args CursorPointType::sortArgs( const Args& args ) const
{
  return args;
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

static void redefinePoint( ObjectHolder* o, KigPart& d, KigWidget& w )
{
  PointRedefineMode pm( o, d, w );
  d.runMode( &pm );
}

void FixedPointType::executeAction(
  int i, ObjectHolder& oh, ObjectTypeCalcer& o,
  KigPart& d, KigWidget& w, NormalMode& ) const
{
  switch( i )
  {
  case 0:
  {
    bool ok = true;
    assert ( o.imp()->inherits( PointImp::stype() ) );
    Coordinate oldc = static_cast<const PointImp*>( o.imp() )->coordinate();
    KigInputDialog::getCoordinate(
      i18n( "Set Coordinate" ),
      i18n( "Enter the new coordinate." ) + QString::fromLatin1( "<br>" ) +
      d.document().coordinateSystem().coordinateFormatNoticeMarkup(),
      &w, &ok, d.document(), &oldc );
    if ( ! ok ) break;

    MonitorDataObjects mon( getAllParents( &o ) );
    o.move( oldc, d.document() );
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
  int i, ObjectHolder& oh, ObjectTypeCalcer& o, KigPart& d, KigWidget& w,
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

const Coordinate RelativePointType::moveReferencePoint( const ObjectTypeCalcer& ourobj ) const
{
  assert( ourobj.imp()->inherits( PointImp::stype() ) );
//  return static_cast<const PointImp*>( ourobj.imp() )->coordinate();
  return Coordinate( 0., 0. );
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

std::vector<ObjectCalcer*> RelativePointType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> ret;
  ret.push_back( ourobj.parents()[0] );
  ret.push_back( ourobj.parents()[1] );
  return ret;
}

std::vector<ObjectCalcer*> ConstrainedPointType::movableParents( const ObjectTypeCalcer& ourobj ) const
{
  std::vector<ObjectCalcer*> ret;
  ret.push_back( ourobj.parents()[0] );
  return ret;
}

/* ----------------- Transport of measure ------------------------------ */

ObjectImp* MeasureTransportType::calc( const Args& parents, const KigDocument& doc ) const
{
  double measure;

  if ( parents.size() != 3 ) return new InvalidImp;

  if ( parents[0]->inherits (SegmentImp::stype()) )
  {
    const SegmentImp* s = static_cast<const SegmentImp*>( parents[0] );
    measure = s->length();
  } else if ( parents[0]->inherits (ArcImp::stype()) )
  {
    const ArcImp* s = static_cast<const ArcImp*>( parents[0] );
    measure = s->radius()*s->angle();
  } else return new InvalidImp;

  const Coordinate& p = static_cast<const PointImp*>( parents[2] )->coordinate();
  if ( parents[1]->inherits (LineImp::stype()) )
  {
    const LineImp* c = static_cast<const LineImp*>( parents[1] );

    if ( !c->containsPoint( p, doc ) )
      return new InvalidImp;

    const LineData line = c->data();
    const Coordinate dir = line.dir()/line.length();
    const Coordinate nc = p + measure*dir;

    if ( nc.valid() ) return new PointImp( nc );
    else return new InvalidImp;
  } else if ( parents[1]->inherits (CircleImp::stype()) )
  {
    const CircleImp* c = static_cast<const CircleImp*>( parents[1] );
    if ( !c->containsPoint( p, doc ) )
      return new InvalidImp;

    double param = c->getParam( p, doc );
    measure /= 2*c->radius()*M_PI;
    param += measure;
    while (param > 1) param -= 1;

    const Coordinate nc = c->getPoint( param, doc );
    if ( nc.valid() ) return new PointImp( nc );
    else return new InvalidImp;
  }

  return new InvalidImp;
}

//    I18N_NOOP( "Select the segment/arc to transport on the circle/line..." ), false },
//    I18N_NOOP( "Select the circle/line on which to transport a measure..." ), true },
//    I18N_NOOP( "Select a point on the circle/line..." ), false }

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( MeasureTransportType )

MeasureTransportType::MeasureTransportType()
  : ObjectType( "TransportOfMeasure" )
{
}

MeasureTransportType::~MeasureTransportType()
{
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

const ObjectImpType* MeasureTransportType::impRequirement( const ObjectImp* obj, const Args& ) const
{
  if ( obj->inherits( PointImp::stype () )  )
    return PointImp::stype ();

  if ( obj->inherits( LineImp::stype () ) )
    return LineImp::stype ();

  if ( obj->inherits( CircleImp::stype () ) )
    return CircleImp::stype ();

  if ( obj->inherits( SegmentImp::stype () ) )
    return SegmentImp::stype ();

  if ( obj->inherits( ArcImp::stype () ) )
    return ArcImp::stype ();

  return 0;
}

bool MeasureTransportType::isDefinedOnOrThrough( const ObjectImp* o, const Args& ) const
{
  if ( o->inherits( LineImp::stype() ) ) return true;
  if ( o->inherits( CircleImp::stype() ) ) return true;
  return false;
}

std::vector<ObjectCalcer*> MeasureTransportType::sortArgs( const std::vector<ObjectCalcer*>& args ) const
{
  return args;  /* should already be in correct order */
}

Args MeasureTransportType::sortArgs( const Args& args ) const
{
  return args;
}

/* - transport of measure (old, for compatibility with prev. kig files) - */

ObjectImp* MeasureTransportTypeOld::calc( const Args& parents, const KigDocument& doc ) const
{
  if ( ! margsparser.checkArgs( parents ) ) return new InvalidImp;

  const CircleImp* c = static_cast<const CircleImp*>( parents[0] );
  const Coordinate& p = static_cast<const PointImp*>( parents[1] )->coordinate();

  if ( !c->containsPoint( p, doc ) )
    return new InvalidImp;

  const SegmentImp* s = static_cast<const SegmentImp*>( parents[2] );
  double param = c->getParam( p, doc );
  double measure = s->length();
  measure /= 2*c->radius()*M_PI;
  param += measure;
  while (param > 1) param -= 1;

  const Coordinate nc = c->getPoint( param, doc );
  if ( nc.valid() ) return new PointImp( nc );
  else return new InvalidImp;
}

static const ArgsParser::spec argsspecMeasureTransportOld[] =
{
  { CircleImp::stype(), "Transport a measure on this circle",
    I18N_NOOP( "Select the circle on which to transport a measure..." ), true },
  { PointImp::stype(), "Start transport from this point of the circle",
    I18N_NOOP( "Select a point on the circle..." ), false },
  { SegmentImp::stype(), "Segment to transport",
    I18N_NOOP( "Select the segment to transport on the circle..." ), false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( MeasureTransportTypeOld )

MeasureTransportTypeOld::MeasureTransportTypeOld()
  : ArgsParserObjectType( "MeasureTransport", argsspecMeasureTransportOld, 3 )
{
}

MeasureTransportTypeOld::~MeasureTransportTypeOld()
{
}

const MeasureTransportTypeOld* MeasureTransportTypeOld::instance()
{
  static const MeasureTransportTypeOld t;
  return &t;
}

const ObjectImpType* MeasureTransportTypeOld::resultId() const
{
  return PointImp::stype();
}

/* ----------------- end transport of measure ------------------------- */

