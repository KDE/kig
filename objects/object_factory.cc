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
#include "text_type.h"
#include "object.h"

#include "../misc/coordinate.h"
#include "../misc/calcpaths.h"
#include "../misc/object_hierarchy.h"
#include "../kig/kig_view.h"
#include "../kig/kig_part.h"

#include <algorithm>
#include <functional>

Object* ObjectFactory::fixedPoint( const Coordinate& c )
{
  Objects args;
  args.push_back( new DataObject( new DoubleImp( c.x ) ) );
  args.push_back( new DataObject( new DoubleImp( c.y ) ) );
  RealObject* o = new RealObject( FixedPointType::instance(), args );
  return o;
}

ObjectFactory* ObjectFactory::instance()
{
  static ObjectFactory f;
  return &f;
}

Object* ObjectFactory::sensiblePoint( const Coordinate& c, const KigDocument& d, const KigWidget& w )
{
  Objects os = d.whatAmIOn( c, w );
  for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
    if ( (*i)->hasimp( CurveImp::stype() ) )
      return constrainedPoint( *i, c, d );
  return fixedPoint( c );
}

void ObjectFactory::redefinePoint( Object* tpoint, const Coordinate& c,
                                   KigDocument& doc, const KigWidget& w )
{
  assert( tpoint->inherits( Object::ID_RealObject ) );
  RealObject* point = static_cast<RealObject*>( tpoint );

  Objects os = doc.whatAmIOn( c, w );
  Object* v = 0;

  // we don't want one of our children as a parent...
  Objects children = point->getAllChildren();
  for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
    if ( (*i)->hasimp( CurveImp::stype() ) && ! children.contains( *i ) )
    {
      v = *i;
      break;
    };

  if ( v )
  {
    // we want a constrained point...
    const CurveImp* curveimp = static_cast<const CurveImp*>( v->imp() );
    double newparam = curveimp->getParam( c, doc );

    if ( point->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
    {
      // point already was constrained -> simply update the param
      // DataObject and make sure point is on the right curve...
      Object* dataobj = 0;
      Objects parents = point->parents();
      assert( parents.size() == 2 );
      if ( parents[0]->hasimp( DoubleImp::stype() ) )
        dataobj = parents[0];
      else dataobj = parents[1];

      parents.clear();
      parents.push_back( dataobj );
      parents.push_back( v );
      point->setParents( parents );

      assert( dataobj->inherits( Object::ID_DataObject ) );
      static_cast<DataObject*>( dataobj )->setImp( new DoubleImp( newparam ) );
    }
    else
    {
      // point used to be fixed -> add a new DataObject etc.
      Objects args;
      args.push_back( new DataObject( new DoubleImp( newparam ) ) );
      args.push_back( v );
      point->setParents( args );
      point->setType( ConstrainedPointType::instance() );
    }
  }
  else
  {
    // a fixed point...
    if ( point->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
    {
      // point used to be constrained..
      Objects a;
      a.push_back( new DataObject( new DoubleImp( c.x ) ) );
      a.push_back( new DataObject( new DoubleImp( c.y ) ) );

      point->setType( FixedPointType::instance() );
      point->setParents( a );
    }
    else
    {
      // point used to be fixed -> simply update the DataObject's
      // we can use the point's move function for that..
      point->move( c, doc );
    };
  }
}

Object* ObjectFactory::locus( const Objects& parents )
{
  using namespace std;

  assert( parents.size() == 2 );
  assert( parents.front()->inherits( Object::ID_RealObject ) );
  RealObject* constrained = static_cast<RealObject*>( parents.front() );
  const Object* moving = parents.back();
  if ( ! constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    // moving is in fact the constrained point.. swap them..
    moving = constrained;
    assert( parents.back()->inherits( Object::ID_RealObject ) );
    constrained = static_cast<RealObject*>( parents.back() );
  };
  assert( constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) );

  Objects hierparents( constrained );
  Objects sideOfTree = sideOfTreePath( hierparents, moving );
  copy( sideOfTree.begin(), sideOfTree.end(), back_inserter( hierparents ) );

  ObjectHierarchy hier( hierparents, moving );

  Object* curve = const_cast<Object*>( constrained->parents().back() );
  if ( ! curve->hasimp( CurveImp::stype() ) )
    curve = const_cast<Object*>( constrained->parents().front() );
  assert( curve->hasimp( CurveImp::stype() ) );

  Objects realparents( 2 + sideOfTree.size(), 0 );
  realparents[0] = curve;
  realparents[1] = new DataObject( new HierarchyImp( hier ) );
  copy( sideOfTree.begin(), sideOfTree.end(), realparents.begin() + 2 );

  return new RealObject( LocusType::instance(), realparents );
}

Object* ObjectFactory::label( const QString& s, const Coordinate& loc,
                              bool needframe, const Objects& nparents,
                              const KigDocument& doc )
{
  return attachedLabel( s, 0, loc, needframe, nparents, doc );
}

Object* ObjectFactory::attachedLabel( const QString& s,
                                      Object* p,
                                      const Coordinate& loc,
                                      bool needframe,
                                      const Objects& nparents,
                                      const KigDocument& doc )
{
  using namespace std;
  Objects parents;
  parents.reserve( nparents.size() + 3 );
  parents.push_back( new DataObject( new IntImp( needframe ? 1 : 0 ) ) );
  parents.push_back( new DataObject( new StringImp( s ) ) );

  if ( p && p->hasimp( PointImp::stype() ) )
    parents.push_back( p );
  else if ( p && p->hasimp( CurveImp::stype() ) )
  {
    Object* o = constrainedPoint( p, static_cast<const CurveImp*>( p->imp() )->getParam( loc, doc ) );
    o->calc( doc );
    o->setShown( false );
    parents.push_back( o );
  }
  else
    parents.push_back( new DataObject( new PointImp( loc ) ) );

  copy( nparents.begin(), nparents.end(), back_inserter( parents ) );
  RealObject* ret = new RealObject( TextType::instance(), parents );
  ret->calc( doc );
  return ret;
}

Object* ObjectFactory::constrainedPoint( Object* curve, double param )
{
  assert( curve->hasimp( CurveImp::stype() ) );
  Objects parents;
  parents.push_back( curve );
  parents.push_back( new DataObject( new DoubleImp( param ) ) );
  return new RealObject( ConstrainedPointType::instance(), parents );
}

Object* ObjectFactory::constrainedPoint( Object* curve, const Coordinate& c, const KigDocument& d )
{
  assert( curve->hasimp( CurveImp::stype() ) );
  double param = static_cast<const CurveImp*>( curve->imp() )->getParam( c, d );
  return constrainedPoint( curve, param );
}

Object* ObjectFactory::propertyObject( Object* o, const char* p )
{
  int wp = o->propertiesInternalNames().findIndex( p );
  if ( wp == -1 ) return 0;
  PropertyObject* po = new PropertyObject( o, wp );
  return po;
}
