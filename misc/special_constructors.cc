// special_constructors.cc
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#include "special_constructors.h"

#include "kigpainter.h"
#include "calcpaths.h"
#include "guiaction.h"
#include "conic-common.h"
#include "common.h"

#include "../objects/object_factory.h"
#include "../objects/object_type.h"
#include "../objects/object_holder.h"
#include "../objects/object_drawer.h"
#include "../objects/object_calcer.h"
#include "../objects/conic_types.h"
#include "../objects/line_type.h"
#include "../objects/intersection_types.h"
#include "../objects/object_imp.h"
#include "../objects/point_imp.h"
#include "../objects/bogus_imp.h"
#include "../objects/conic_imp.h"
#include "../objects/cubic_imp.h"
#include "../objects/other_type.h"
#include "../objects/locus_imp.h"
#include "../objects/circle_imp.h"
#include "../objects/line_imp.h"

#include "../kig/kig_part.h"

#include "../modes/construct_mode.h"

#include <qpen.h>

#include <klocale.h>

#include <algorithm>
#include <functional>

ConicRadicalConstructor::ConicRadicalConstructor()
  : StandardConstructorBase(
    I18N_NOOP( "Radical Lines for Conics" ),
    I18N_NOOP( "The lines constructed through the intersections "
               "of two conics.  This is also defined for "
               "non-intersecting conics." ),
    "conicsradicalline", mparser ),
    mtype( ConicRadicalType::instance() ),
    mparser( mtype->argsParser().without( IntImp::stype() ) )
{
}

ConicRadicalConstructor::~ConicRadicalConstructor()
{
}

void ConicRadicalConstructor::drawprelim(
  KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& doc ) const
{
  if ( parents.size() == 2 && parents[0]->imp()->inherits( ConicImp::stype() ) &&
       parents[1]->imp()->inherits( ConicImp::stype() ) )
  {
    Args args;
    std::transform( parents.begin(), parents.end(),
                    std::back_inserter( args ), std::mem_fun( &ObjectCalcer::imp ) );
    for ( int i = -1; i < 2; i += 2 )
    {
      IntImp root( i );
      IntImp zeroindex( 1 );
      args.push_back( &root );
      args.push_back( &zeroindex );
      ObjectImp* data = mtype->calc( args, doc );
      data->draw( p );
      delete data; data = 0;
      args.pop_back();
      args.pop_back();
    };
  };
}

std::vector<ObjectHolder*> ConicRadicalConstructor::build( const std::vector<ObjectCalcer*>& os, KigDocument&, KigWidget& ) const
{
  using namespace std;
  std::vector<ObjectHolder*> ret;
  for ( int i = -1; i < 2; i += 2 )
  {
    std::vector<ObjectCalcer*> args;
    std::copy( os.begin(), os.end(), back_inserter( args ) );
    args.push_back( new ObjectConstCalcer( new IntImp( i ) ) );
    // TODO ? use only one zeroindex dataobject, so that if you switch
    // one radical line around, then the other switches along..
    args.push_back( new ObjectConstCalcer( new IntImp( 1 ) ) );
    ret.push_back(
      new ObjectHolder( new ObjectTypeCalcer( mtype, args ) ) );
  };
  return ret;
}

static const struct ArgsParser::spec argsspecpp[] =
{
  { PointImp::stype(), I18N_NOOP( "Moving Point" ), false },
  { PointImp::stype(), I18N_NOOP( "Following Point" ), true }
};

LocusConstructor::LocusConstructor()
  : StandardConstructorBase( I18N_NOOP( "Locus" ), I18N_NOOP( "A locus" ),
                             "locus", margsparser ),
    margsparser( argsspecpp, 2 )
{
}

LocusConstructor::~LocusConstructor()
{
}

void LocusConstructor::drawprelim( KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                   const KigDocument& ) const
{
  // this function is rather ugly, but it is necessary to do it this
  // way in order to play nice with Kig's design..

  if ( parents.size() != 2 ) return;
  const ObjectTypeCalcer* constrained = dynamic_cast<ObjectTypeCalcer*>( parents.front() );
  const ObjectCalcer* moving = parents.back();
  if ( ! constrained || ! constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    // moving is in fact the constrained point.. swap them..
    moving = parents.front();
    constrained = dynamic_cast<const ObjectTypeCalcer*>( parents.back() );
    assert( constrained );
  };
  assert( constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) );

  const ObjectImp* oimp = constrained->parents().back()->imp();
  if( !oimp->inherits( CurveImp::stype() ) )
    oimp = constrained->parents().front()->imp();
  assert( oimp->inherits( CurveImp::stype() ) );
  const CurveImp* cimp = static_cast<const CurveImp*>( oimp );

  ObjectHierarchy hier( constrained, moving );

  LocusImp limp( cimp->copy(), hier );
  limp.draw( p );
}

const int LocusConstructor::wantArgs(
 const std::vector<ObjectCalcer*>& os, const KigDocument&, const KigWidget&
 ) const
{
  int ret = margsparser.check( os );
  if ( ret == ArgsParser::Invalid ) return ret;
  else if ( os.size() != 2 ) return ret;
  if ( dynamic_cast<ObjectTypeCalcer*>( os.front() ) &&
       static_cast<ObjectTypeCalcer*>( os.front() )->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    std::set<ObjectCalcer*> children = getAllChildren( os.front() );
    return children.find( os.back() ) != children.end() ? ret : ArgsParser::Invalid;
  }
  if ( dynamic_cast<ObjectTypeCalcer*>( os.back() ) &&
       static_cast<ObjectTypeCalcer*>( os.back() )->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    std::set<ObjectCalcer*> children = getAllChildren( os.back() );
    return children.find( os.front() ) != children.end() ? ret : ArgsParser::Invalid;
  }
  return ArgsParser::Invalid;
}

std::vector<ObjectHolder*> LocusConstructor::build( const std::vector<ObjectCalcer*>& parents, KigDocument&, KigWidget& ) const
{
  std::vector<ObjectHolder*> ret;
  assert( parents.size() == 2 );

  ObjectTypeCalcer* constrained = dynamic_cast<ObjectTypeCalcer*>( parents.front() );
  ObjectCalcer* moving = parents.back();
  if ( ! constrained || ! constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    // moving is in fact the constrained point.. swap them..
    moving = parents.front();
    constrained = dynamic_cast<ObjectTypeCalcer*>( parents.back() );
    assert( constrained );
  };
  assert( constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) );

  ret.push_back( ObjectFactory::instance()->locus( constrained, moving ) );
  return ret;
}

QString LocusConstructor::useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& os,
                                   const KigDocument&, const KigWidget& ) const
{
  if ( dynamic_cast<const ObjectTypeCalcer*>( &o ) &&
       static_cast<const ObjectTypeCalcer&>( o ).type()->inherits( ObjectType::ID_ConstrainedPointType ) &&
       ( os.empty() || !dynamic_cast<ObjectTypeCalcer*>( os[0] ) ||
         !static_cast<const ObjectTypeCalcer*>( os[0] )->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
    ) return i18n( "Moving Point" );
  else return i18n( "Dependent Point" );
}

void ConicRadicalConstructor::plug( KigDocument*, KigGUIAction* )
{
}

void LocusConstructor::plug( KigDocument*, KigGUIAction* )
{
}

bool ConicRadicalConstructor::isTransform() const
{
  return mtype->isTransform();
}

bool LocusConstructor::isTransform() const
{
  return false;
}

static const ArgsParser::spec argsspectc[] = {
  { ConicImp::stype(), "", true },
  { ConicImp::stype(), "", true }
};

ConicConicIntersectionConstructor::ConicConicIntersectionConstructor()
  : StandardConstructorBase( "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
                             "curvelineintersection", mparser ),
    mparser( argsspectc, 2 )
{
}

ConicConicIntersectionConstructor::~ConicConicIntersectionConstructor()
{
}

void ConicConicIntersectionConstructor::drawprelim( KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                                    const KigDocument& ) const
{
  if ( parents.size() != 2 ) return;
  assert ( parents[0]->imp()->inherits( ConicImp::stype() ) &&
           parents[1]->imp()->inherits( ConicImp::stype() ) );
  const ConicCartesianData conica =
    static_cast<const ConicImp*>( parents[0]->imp() )->cartesianData();
  const ConicCartesianData conicb =
    static_cast<const ConicImp*>( parents[1]->imp() )->cartesianData();
  bool ok = true;
  for ( int wr = -1; wr < 2; wr += 2 )
  {
    LineData radical = calcConicRadical( conica, conicb, wr, 1, ok );
    if ( ok )
    {
      for ( int wi = -1; wi < 2; wi += 2 )
      {
        bool ok2 = true;
        Coordinate c = calcConicLineIntersect( conica, radical, 0.0, wi, ok2 );
        if ( ok2 ) p.drawFatPoint( c );
      };
    };
  };
}

std::vector<ObjectHolder*> ConicConicIntersectionConstructor::build(
  const std::vector<ObjectCalcer*>& os, KigDocument& doc, KigWidget& ) const
{
  assert( os.size() == 2 );
  std::vector<ObjectHolder*> ret;
  ObjectCalcer* conica = os[0];
  ObjectConstCalcer* zeroindexdo = new ObjectConstCalcer( new IntImp( 1 ) );

  for ( int wr = -1; wr < 2; wr += 2 )
  {
    std::vector<ObjectCalcer*> args = os;
    args.push_back( new ObjectConstCalcer( new IntImp( wr ) ) );
    args.push_back( zeroindexdo );
    ObjectTypeCalcer* radical =
      new ObjectTypeCalcer( ConicRadicalType::instance(), args );
    radical->calc( doc );
    for ( int wi = -1; wi < 2; wi += 2 )
    {
      args.clear();
      args.push_back( conica );
      args.push_back( radical );
      args.push_back( new ObjectConstCalcer( new IntImp( wi ) ) );
      ret.push_back(
        new ObjectHolder(
          new ObjectTypeCalcer(
            ConicLineIntersectionType::instance(), args ) ) );
    };
  };
  return ret;
}

void ConicConicIntersectionConstructor::plug( KigDocument*, KigGUIAction* )
{
}

bool ConicConicIntersectionConstructor::isTransform() const
{
  return false;
}

ConicLineIntersectionConstructor::ConicLineIntersectionConstructor()
  : MultiObjectTypeConstructor(
    ConicLineIntersectionType::instance(),
    "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
    "curvelineintersection", -1, 1 )
{
}

ConicLineIntersectionConstructor::~ConicLineIntersectionConstructor()
{
}

QString ConicRadicalConstructor::useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>&,
                                          const KigDocument&, const KigWidget& ) const
{
  if ( o.imp()->inherits( CircleImp::stype() ) )
    return i18n( "Construct the Radical Lines of This Circle" );
  else
    return i18n( "Construct the Radical Lines of This Conic" );
}

GenericIntersectionConstructor::GenericIntersectionConstructor()
  : MergeObjectConstructor(
    I18N_NOOP( "Intersect" ),
    I18N_NOOP( "The intersection of two objects" ),
    "curvelineintersection" )
{
  // intersection type..
  // There is one "toplevel" object_constructor, that is composed
  // of multiple subconstructors..  First we build the
  // subconstructors:
  SimpleObjectTypeConstructor* lineline =
    new SimpleObjectTypeConstructor(
      LineLineIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "curvelineintersection" );

  ObjectConstructor* lineconic =
    new ConicLineIntersectionConstructor();

  MultiObjectTypeConstructor* linecubic =
    new MultiObjectTypeConstructor(
      LineCubicIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "curvelineintersection", 1, 2, 3 );

  ObjectConstructor* conicconic =
    new ConicConicIntersectionConstructor();

  MultiObjectTypeConstructor* circlecircle =
    new MultiObjectTypeConstructor(
      CircleCircleIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "circlecircleintersection", -1, 1 );

  merge( lineline );
  merge( circlecircle );
  merge( lineconic );
  merge( linecubic );
  merge( conicconic );
}

GenericIntersectionConstructor::~GenericIntersectionConstructor()
{
}

bool GenericIntersectionConstructor::isIntersection() const
{
  return true;
}

QString GenericIntersectionConstructor::useText(
  const ObjectCalcer& o, const std::vector<ObjectCalcer*>&,
  const KigDocument&, const KigWidget& ) const
{
  if ( o.imp()->inherits( CircleImp::stype() ) )
    return i18n( "Intersect with This Circle" );
  else if ( o.imp()->inherits( ConicImp::stype() ) )
    return i18n( "Intersect with This Conic" );
  else if ( o.imp()->inherits( AbstractLineImp::stype() ) )
    return i18n( "Intersect with This Line" );
  else if ( o.imp()->inherits( CubicImp::stype() ) )
    return i18n( "Intersect with This Cubic Curve" );
  else assert( false );
  return QString::null;
}

static const ArgsParser::spec argsspecMidPointOfTwoPoints[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct the midpoint of this point" ), false },
  { PointImp::stype(), I18N_NOOP( "Construct the midpoint of this point" ), false }
};

MidPointOfTwoPointsConstructor::MidPointOfTwoPointsConstructor()
  : StandardConstructorBase( "Mid Point",
                             "Construct the midpoint of two points",
                             "bisection", mparser ),
    mparser( argsspecMidPointOfTwoPoints, 2 )
{
}

MidPointOfTwoPointsConstructor::~MidPointOfTwoPointsConstructor()
{
}

void MidPointOfTwoPointsConstructor::drawprelim(
  KigPainter& p, const std::vector<ObjectCalcer*>& parents,
  const KigDocument& ) const
{
  if ( parents.size() != 2 ) return;
  assert( parents[0]->imp()->inherits( PointImp::stype() ) );
  assert( parents[1]->imp()->inherits( PointImp::stype() ) );
  const Coordinate m =
    ( static_cast<const PointImp*>( parents[0]->imp() )->coordinate() +
      static_cast<const PointImp*>( parents[1]->imp() )->coordinate() ) / 2;
  PointImp( m ).draw( p );
}

std::vector<ObjectHolder*> MidPointOfTwoPointsConstructor::build(
  const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& ) const
{
  ObjectTypeCalcer* seg = new ObjectTypeCalcer( SegmentABType::instance(), os );
  seg->calc( d );
  int index = seg->imp()->propertiesInternalNames().findIndex( "mid-point" );
  assert( index != -1 );
  ObjectPropertyCalcer* prop = new ObjectPropertyCalcer( seg, index );
  prop->calc( d );
  std::vector<ObjectHolder*> ret;
  ret.push_back( new ObjectHolder( prop ) );
  return ret;
}

void MidPointOfTwoPointsConstructor::plug( KigDocument*, KigGUIAction* )
{
}

bool MidPointOfTwoPointsConstructor::isTransform() const
{
  return false;
}

TestConstructor::TestConstructor( const ArgsParserObjectType* type, const char* descname,
                                  const char* desc, const char* iconfile )
  : StandardConstructorBase( descname, desc, iconfile, type->argsParser() ),
    mtype( type )
{
}

TestConstructor::~TestConstructor()
{
}

void TestConstructor::drawprelim( KigPainter&, const std::vector<ObjectCalcer*>&,
                                  const KigDocument& ) const
{
  // not used, only here because of the wrong
  // ObjectConstructor-GUIAction design.  See the TODO
}

std::vector<ObjectHolder*> TestConstructor::build( const std::vector<ObjectCalcer*>&, KigDocument&,
                                                   KigWidget& ) const
{
  // not used, only here because of the wrong
  // ObjectConstructor-GUIAction design.  See the TODO
  std::vector<ObjectHolder*> ret;
  return ret;
}

void TestConstructor::plug( KigDocument*, KigGUIAction* )
{
}

bool TestConstructor::isTransform() const
{
  return false;
}

bool TestConstructor::isTest() const
{
  return true;
}

BaseConstructMode* TestConstructor::constructMode( KigDocument& doc )
{
  return new TestConstructMode( doc, mtype );
}

const int TestConstructor::wantArgs( const std::vector<ObjectCalcer*>& os,
                                     const KigDocument& d, const KigWidget& v ) const
{
  int ret = StandardConstructorBase::wantArgs( os, d, v );
  if ( ret == ArgsParser::Complete ) ret = ArgsParser::Valid;
  return ret;
}

