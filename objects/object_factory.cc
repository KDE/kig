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
  Objects os = d.whatAmIOn( c, w );
  for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
    if ( (*i)->hasimp( ObjectImp::ID_CurveImp ) )
      return constrainedPoint( *i, c, d );
  return fixedPoint( c );
}

Objects ObjectFactory::redefinePoint( Object* tpoint, const Coordinate& c,
                                      KigDocument& doc, const KigWidget& w )
{
  assert( tpoint->inherits( Object::ID_RealObject ) );
  RealObject* point = static_cast<RealObject*>( tpoint );

  Objects os = doc.whatAmIOn( c, w );
  Object* v = 0;

  // we don't want one of our children as a parent...
  Objects children = point->getAllChildren();
  for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
    if ( (*i)->hasimp( ObjectImp::ID_CurveImp ) && ! children.contains( *i ) )
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
      if ( parents[0]->hasimp( ObjectImp::ID_DoubleImp ) )
        dataobj = parents[0];
      else dataobj = parents[1];

      parents.clear();
      parents.push_back( dataobj );
      parents.push_back( v );
      point->setParents( parents, &doc );

      assert( dataobj->inherits( Object::ID_DataObject ) );
      static_cast<DataObject*>( dataobj )->setImp( new DoubleImp( newparam ) );
//       const Coordinate oldcoord = static_cast<const PointImp*>( point->imp() )->coordinate();
//       point->move( oldcoord, c - oldcoord, doc );

      Objects ret;
      ret.push_back( dataobj );
      ret.push_back( point );
      return ret;
    }
    else
    {
      // point used to be fixed -> add a new DataObject etc.
      DataObject* d = new DataObject( new DoubleImp( newparam ) );
      doc._addObject( d );
      Objects args;
      args.push_back( d );
      args.push_back( v );
      point->setType( ConstrainedPointType::instance() );
      point->setParents( args, &doc );
      args[1] = point;
      return args;
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
      doc._addObjects( a );
      point->setType( FixedPointType::instance() );
      point->setParents( a, &doc );
      a.push_back( point );
      return a;
    }
    else
    {
      // point used to be fixed -> simply update the DataObject's
      // we can use the point's move function for that..
      const Coordinate oldcoord = static_cast<const PointImp*>( point->imp() )->coordinate();
      point->move( oldcoord, c - oldcoord, doc );
      return point->parents().with( point );
    };
  }
}

Objects ObjectFactory::locus( const Objects& parents )
{
  Objects ret;
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

  Objects hierparents( const_cast<RealObject*>( constrained ) );
  Objects sideOfTree = sideOfTreePath( Objects( const_cast<RealObject*>( constrained ) ), moving );
  copy( sideOfTree.begin(), sideOfTree.end(), back_inserter( hierparents ) );

  ObjectHierarchy hier( hierparents, moving );

  Object* curve = const_cast<Object*>( constrained->parents().back() );
  if ( ! curve->hasimp( ObjectImp::ID_CurveImp ) )
    curve = const_cast<Object*>( constrained->parents().front() );
  assert( curve->hasimp( ObjectImp::ID_CurveImp ) );

  Objects realparents( 2 + sideOfTree.size(), 0 );
  realparents[0] = curve;
  Object* hierobj = new DataObject( new HierarchyImp( hier ) );
  realparents[1] = hierobj;
  copy( sideOfTree.begin(), sideOfTree.end(), realparents.begin() + 2 );

  ret.push_back( hierobj );
  ret.push_back( new RealObject( LocusType::instance(), realparents ) );

  return ret;
}

Objects ObjectFactory::label( const QString& s, const Coordinate& loc,
                              bool needframe, const Objects& nparents )
{
  using namespace std;

  Objects ret;
  Objects parents;
  parents.reserve( nparents.size() + 3 );
  parents.push_back( new DataObject( new IntImp( needframe ? 1 : 0 ) ) );
  parents.push_back( new DataObject( new StringImp( s ) ) );
  parents.push_back( new DataObject( new PointImp( loc ) ) );
  ret.push_back( parents[0] );
  ret.push_back( parents[1] );
  ret.push_back( parents[2] );
  copy( nparents.begin(), nparents.end(), back_inserter( parents ) );

  RealObject* r = new RealObject( TextType::instance(), parents );
  ret.push_back( r );
  return ret;
}

Objects ObjectFactory::constrainedPoint( Object* curve, double param )
{
  assert( curve->hasimp( ObjectImp::ID_CurveImp ) );
  Objects ret;
  Objects parents;
  parents.push_back( curve );
  ret.push_back( new DataObject( new DoubleImp( param ) ) );
  parents.push_back( ret.back() );
  ret.push_back( new RealObject( ConstrainedPointType::instance(), parents ) );
  return ret;
}

Objects ObjectFactory::constrainedPoint( Object* curve, const Coordinate& c, const KigDocument& d )
{
  assert( curve->hasimp( ObjectImp::ID_CurveImp ) );
  double param = static_cast<const CurveImp*>( curve->imp() )->getParam( c, d );
  return constrainedPoint( curve, param );
}
