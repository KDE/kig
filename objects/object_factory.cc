// object_factory.cc
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

#include "object_factory.h"

#include "bogus_imp.h"
#include "curve_imp.h"
#include "point_imp.h"
#include "point_type.h"
#include "other_type.h"
#include "custom_types.h"
#include "text_type.h"
#include "object.h"

#include "../misc/coordinate.h"
#include "../misc/calcpaths.h"
#include "../misc/object_hierarchy.h"
#include "../kig/kig_view.h"
#include "../kig/kig_part.h"

#include <algorithm>
#include <functional>

ObjectFactory* ObjectFactory::s = 0;

Objects ObjectFactory::fixedPoint( const Coordinate& c )
{
  DataObject* x = new DataObject( new DoubleImp( c.x ) );
  DataObject* y = new DataObject( new DoubleImp( c.y ) );
  Objects args;
  args.push_back( x );
  args.push_back( y );
  RealObject* o = new RealObject( FixedPointType::instance(), args );
  args.push_back( o );
  return args;
}

ObjectFactory* ObjectFactory::instance()
{
  if ( s == 0 ) s = new ObjectFactory();
  return s;
}

Objects ObjectFactory::sensiblePoint( const Coordinate& c, const KigDocument& d, const KigWidget& w )
{
  Objects os = fixedPoint( c );
  redefinePoint( static_cast<RealObject*>( os[2] ), c, d, w );
  return os;
}

void ObjectFactory::redefinePoint( Object* tpoint, const Coordinate& c,
                                   const KigDocument& d, const KigWidget& w )
{
  assert( tpoint->inherits( Object::ID_RealObject ) );
  RealObject* point = static_cast<RealObject*>( tpoint );
  Objects o = d.whatAmIOn( c, w.screenInfo() );
  Object* v = 0;
  // we don't want one of our children as a parent...
  Objects children = point->getAllChildren();
  for ( Objects::iterator i = o.begin(); i != o.end(); ++i )
  {
    if ( (*i)->hasimp( ObjectImp::ID_CurveImp ) && ! children.contains( *i ) )
    {
      v = *i;
      break;
    };
  };
  if ( v )
  {
    // a constrained point...
    DataObject* d = new DataObject( new DoubleImp( static_cast<const CurveImp*>( v->imp() )->getParam( c ) ) );
    Objects args;
    args.push_back( d );
    args.push_back( v );
    point->reset( ConstrainedPointType::instance(), args );
  }
  else
  {
    // a fixed point...
    Objects a;
    a.push_back( new DataObject( new DoubleImp( c.x ) ) );
    a.push_back( new DataObject( new DoubleImp( c.y ) ) );
    point->reset( FixedPointType::instance(), a );
  }
}

RealObject* ObjectFactory::locus( const Objects& parents )
{
  using namespace std;

  assert( parents.size() == 2 );
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

  Objects locusparents( const_cast<RealObject*>( constrained ) );
  Objects sideOfTree = sideOfTreePath( Objects( const_cast<RealObject*>( constrained ) ), moving );
  copy( sideOfTree.begin(), sideOfTree.end(), back_inserter( locusparents ) );

  ObjectHierarchy hier( locusparents, moving );

  Object* curve = const_cast<Object*>( constrained->parents().back() );
  if ( ! curve->hasimp( ObjectImp::ID_CurveImp ) )
    curve = const_cast<Object*>( constrained->parents().front() );
  assert( curve->hasimp( ObjectImp::ID_CurveImp ) );
  // a locus object does not depend on the constrained point, but on
  // the curve it is on..
  locusparents[0] = curve;

  LocusType* t = new LocusType( hier );
  CustomTypes::instance()->add( t );

  return new RealObject( t, locusparents );
}

Objects ObjectFactory::label( const QString& s, const Coordinate& loc, const Objects& nparents )
{
  using namespace std;

  Objects ret;
  Objects parents;
  parents.reserve( nparents.size() + 2 );
  parents.push_back( new DataObject( new StringImp( s ) ) );
  parents.push_back( new DataObject( new PointImp( loc ) ) );
  ret.push_back( parents[0] );
  ret.push_back( parents[1] );
  copy( nparents.begin(), nparents.end(), back_inserter( parents ) );

  RealObject* r = new RealObject( TextType::instance(), parents );
  ret.push_back( r );
  return ret;
}
