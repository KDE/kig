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

#include "i18n.h"
#include "objects.h"
#include "kigpainter.h"
#include "calcpaths.h"
#include "guiaction.h"
#include "conic-common.h"
#include "common.h"

#include "../objects/object.h"
#include "../objects/object_factory.h"
#include "../objects/object_type.h"
#include "../objects/conic_types.h"
#include "../objects/intersection_types.h"
#include "../objects/object_imp.h"
#include "../objects/bogus_imp.h"
#include "../objects/conic_imp.h"
#include "../objects/other_type.h"
#include "../objects/locus_imp.h"

#include "../kig/kig_part.h"

#include <qpen.h>

#include <algorithm>
#include <functional>

ConicRadicalConstructor::ConicRadicalConstructor()
  : StandardConstructorBase(
    I18N_NOOP( "Radical line for conics" ),
    I18N_NOOP( "The lines constructed through the intersections of two conics.  This is also defined for non-intersecting conics." ),
    "conicsradicalline", mparser ),
    mtype( ConicRadicalType::instance() ),
    mparser( mtype->argParser().without( ObjectImp::ID_IntImp ) )
{
}

ConicRadicalConstructor::~ConicRadicalConstructor()
{
}

void ConicRadicalConstructor::drawprelim(
  KigPainter& p, const Objects& parents, const KigDocument& doc ) const
{
  if ( parents.size() == 2 && parents[0]->hasimp( ObjectImp::ID_ConicImp ) &&
       parents[1]->hasimp( ObjectImp::ID_ConicImp ) )
  {
    Args args;
    using namespace std;
    transform( parents.begin(), parents.end(),
               back_inserter( args ), mem_fun( &Object::imp ) );
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

Objects ConicRadicalConstructor::build( const Objects& os, KigDocument&, KigWidget& ) const
{
  using namespace std;
  Objects ret;
  for ( int i = -1; i < 2; i += 2 )
  {
    Objects args;
    args.push_back( new DataObject( new IntImp( i ) ) );
    // TODO ? use only one zeroindex dataobject, so that if you switch
    // one radical line around, then the other switches along..
    args.push_back( new DataObject( new IntImp( 1 ) ) );
    copy( args.begin(), args.end(), back_inserter( ret ) );
    copy( os.begin(), os.end(), back_inserter( args ) );
    ret.push_back( new RealObject( mtype, args ) );
  };
  return ret;
}

static const struct ArgParser::spec argsspecpp[] =
{
  { ObjectImp::ID_PointImp, I18N_NOOP( "Moving point" ) },
  { ObjectImp::ID_PointImp, I18N_NOOP( "following" ) }
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

void LocusConstructor::drawprelim( KigPainter& p, const Objects& parents,
                                   const KigDocument& ) const
{
  // this function is rather ugly, but it is necessary to do it this
  // way in order to play nice with Kig's design..

  if ( parents.size() != 2 ) return;
  assert( parents.front()->inherits( Object::ID_RealObject ) );
  const RealObject* constrained = static_cast<RealObject*>( parents.front() );
  const Object* moving = parents.back();
  if ( ! constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    // moving is in fact the constrained point.. swap them..
    moving = constrained;
    assert( parents.back()->inherits( Object::ID_RealObject ) );
    constrained = static_cast<RealObject*>( parents.back() );
  };
  assert( constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) );

  const ObjectImp* oimp = constrained->parents().back()->imp();
  if( !oimp->inherits( ObjectImp::ID_CurveImp ) )
    oimp = constrained->parents().front()->imp();
  assert( oimp->inherits( ObjectImp::ID_CurveImp ) );
  const CurveImp* cimp = static_cast<const CurveImp*>( oimp );

  ObjectHierarchy hier( Objects( const_cast<RealObject*>( constrained ) ),
                        moving );

  LocusImp limp( cimp->copy(), hier );
  limp.draw( p );
}

const int LocusConstructor::wantArgs(
 const Objects& os, const KigDocument&, const KigWidget&
 ) const
{
  int ret = margsparser.check( os );
  if ( ret == ArgsChecker::Invalid ) return ret;
  else if ( os.size() != 2 ) return ret;
  if ( os[0]->inherits( Object::ID_RealObject ) &&
       static_cast<RealObject*>( os.front() )->type()->inherits( ObjectType::ID_ConstrainedPointType ) &&
       os.front()->getAllChildren().contains( os.back() ) )
    return ret;
  if ( os[1]->inherits( Object::ID_RealObject ) &&
       static_cast<RealObject*>( os.back() )->type()->inherits( ObjectType::ID_ConstrainedPointType ) &&
       os.back()->getAllChildren().contains( os.front() ) )
    return ret;
  return ArgsChecker::Invalid;
}

Objects LocusConstructor::build( const Objects& parents, KigDocument&, KigWidget& ) const
{
  Objects ret = ObjectFactory::instance()->locus( parents );
  assert( ret.size() == 2 );
  return ret;
}

QString LocusConstructor::useText( const Object& o, const Objects& os,
                                   const KigDocument&, const KigWidget& ) const
{
  if ( o.inherits( Object::ID_RealObject ) &&
       static_cast<const RealObject&>( o ).type()->inherits( ObjectType::ID_ConstrainedPointType ) &&
       ( os.empty() || !os[0]->inherits( Object::ID_RealObject ) ||
         !static_cast<const RealObject*>( os[0] )->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
    ) return i18n( "Moving point" );
  else return i18n( "Dependent point" );
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

static const ArgParser::spec argsspectc[] = {
  { ObjectImp::ID_ConicImp, I18N_NOOP( "Intersect with this circle" ) },
  { ObjectImp::ID_ConicImp, I18N_NOOP( "Intersect with this circle" ) }
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

void ConicConicIntersectionConstructor::drawprelim( KigPainter& p, const Objects& parents,
                                                    const KigDocument& ) const
{
  if ( parents.size() != 2 ) return;
  assert ( parents[0]->hasimp( ObjectImp::ID_ConicImp ) &&
           parents[1]->hasimp( ObjectImp::ID_ConicImp ) );
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
        Coordinate c = calcConicLineIntersect( conica, radical, wi, ok2 );
        if ( ok2 ) p.drawFatPoint( c );
      };
    };
  };
}

Objects ConicConicIntersectionConstructor::build( const Objects& os, KigDocument&,
                                                  KigWidget& ) const
{
  assert( os.size() == 2 );
  Objects ret;
  Object* conica = os[0];
  DataObject* zeroindexdo = new DataObject( new IntImp( 1 ) );
  ret.push_back( zeroindexdo );

  for ( int wr = -1; wr < 2; wr += 2 )
  {
    ret.push_back( new DataObject( new IntImp( wr ) ) );
    Objects parents = os;
    parents.push_back( ret.back() );
    parents.push_back( zeroindexdo );
    RealObject* radical =
      new RealObject( ConicRadicalType::instance(), parents );
    radical->setShown( false );
    ret.push_back( radical );
    for ( int wi = -1; wi < 2; wi += 2 )
    {
      Objects args;
      args.push_back( conica );
      args.push_back( radical );
      ret.push_back( new DataObject( new IntImp( wi ) ) );
      args.push_back( ret.back() );
      ret.push_back(
        new RealObject(
          ConicLineIntersectionType::instance(), args ) );
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

QString ConicConicIntersectionConstructor::useText(
  const Object& o, const Objects&,
  const KigDocument&, const KigWidget& ) const
{
  if ( o.hasimp( ObjectImp::ID_CircleImp ) )
    return i18n( "Intersect with this circle" );
  else if ( o.hasimp( ObjectImp::ID_ConicImp ) )
    return i18n( "Intersect with this conic" );
  else assert( false );
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

QString ConicLineIntersectionConstructor::useText(
  const Object& o, const Objects&,
  const KigDocument&, const KigWidget& ) const
{
  if ( o.hasimp( ObjectImp::ID_CircleImp ) )
    return i18n( "Intersect with this circle" );
  else if ( o.hasimp( ObjectImp::ID_ConicImp ) )
    return i18n( "Intersect with this conic" );
  else if ( o.hasimp( ObjectImp::ID_LineImp ) )
    return i18n( "Intersect with this line" );
  else assert( false );
}

QString ConicRadicalConstructor::useText( const Object& o, const Objects&,
                                          const KigDocument&, const KigWidget& ) const
{
  if ( o.hasimp( ObjectImp::ID_CircleImp ) )
    return i18n( "Construct the radical lines of this circle" );
  else
    return i18n( "Construct the radical lines of this conic" );
}
