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

#include "object_factory.h"

#include "bogus_imp.h"
#include "curve_imp.h"
#include "intersection_types.h"
#include "line_imp.h"
#include "object_drawer.h"
#include "object_holder.h"
#include "other_type.h"
#include "point_imp.h"
#include "point_type.h"
#include "text_type.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate.h"
#include "../misc/object_hierarchy.h"

#include <algorithm>
#include <functional>

ObjectHolder* ObjectFactory::fixedPoint( const Coordinate& c ) const
{
  ObjectHolder* o = new ObjectHolder( fixedPointCalcer( c ) );
  return o;
}

ObjectTypeCalcer* ObjectFactory::fixedPointCalcer( const Coordinate& c ) const
{
  std::vector<ObjectCalcer*> args;
  args.push_back( new ObjectConstCalcer( new DoubleImp( c.x ) ) );
  args.push_back( new ObjectConstCalcer( new DoubleImp( c.y ) ) );
  ObjectTypeCalcer* oc = new ObjectTypeCalcer( FixedPointType::instance(), args );
  return oc;
}

ObjectTypeCalcer* ObjectFactory::cursorPointCalcer( const Coordinate& c ) const
{
  std::vector<ObjectCalcer*> args;
  args.push_back( new ObjectConstCalcer( new DoubleImp( c.x ) ) );
  args.push_back( new ObjectConstCalcer( new DoubleImp( c.y ) ) );
  ObjectTypeCalcer* oc = new ObjectTypeCalcer( CursorPointType::instance(), args );
  return oc;
}

const ObjectFactory* ObjectFactory::instance()
{
  static ObjectFactory f;
  return &f;
}

ObjectTypeCalcer* ObjectFactory::sensiblePointCalcer(
  const Coordinate& c, const KigDocument& d, const KigWidget& w ) const
{
  std::vector<ObjectHolder*> os = d.whatAmIOn( c, w );
  if ( os.size() == 2 )
  {
    // we can calc intersection point *olny* between two objects...
    std::vector<ObjectCalcer*> args;
    args.push_back( os[0]->calcer() );
    args.push_back( os[1]->calcer() );
    // the simpliest case: two lines...
    if ( ( os[0]->imp()->inherits( AbstractLineImp::stype() ) ) &&
         ( os[1]->imp()->inherits( AbstractLineImp::stype() ) ) )
      return new ObjectTypeCalcer( LineLineIntersectionType::instance(), args );
    // other cases will follow...
  }
  for ( std::vector<ObjectHolder*>::iterator i = os.begin(); i != os.end(); ++i )
    if ( (*i)->imp()->inherits( CurveImp::stype() ) )
      return constrainedPointCalcer( (*i)->calcer(), c, d );
  return fixedPointCalcer( c );
}

ObjectHolder* ObjectFactory::sensiblePoint(
  const Coordinate& c, const KigDocument& d, const KigWidget& w ) const
{
  return new ObjectHolder( sensiblePointCalcer( c, d, w ) );
}

ObjectTypeCalcer* ObjectFactory::relativePointCalcer(
  ObjectCalcer* o, const Coordinate& loc ) const
{
  Coordinate reference = 
      static_cast<const ObjectImp*>( o->imp() )->attachPoint();
  assert( reference.valid() );

  double x = 0.0;
  double y = 0.0;
  if ( loc.valid() )
  {
    x = loc.x - reference.x;
    y = loc.y - reference.y;
  }
  std::vector<ObjectCalcer*> parents;
  parents.push_back( new ObjectConstCalcer( new DoubleImp( x ) ) );
  parents.push_back( new ObjectConstCalcer( new DoubleImp( y ) ) );
  parents.push_back( o );
  return new ObjectTypeCalcer( RelativePointType::instance(), parents );
}

ObjectTypeCalcer* ObjectFactory::constrainedPointCalcer(
  ObjectCalcer* curve, double param ) const
{
  assert( curve->imp()->inherits( CurveImp::stype() ) );
  std::vector<ObjectCalcer*> parents;
  parents.push_back( new ObjectConstCalcer( new DoubleImp( param ) ) );
  parents.push_back( curve );
  return new ObjectTypeCalcer( ConstrainedPointType::instance(), parents );
}

ObjectHolder* ObjectFactory::constrainedPoint(
  ObjectCalcer* curve, double param ) const
{
  return new ObjectHolder( constrainedPointCalcer( curve, param ) );
}

ObjectTypeCalcer* ObjectFactory::constrainedPointCalcer(
  ObjectCalcer* curve, const Coordinate& c, const KigDocument& d ) const
{
  assert( curve->imp()->inherits( CurveImp::stype() ) );
  double param = static_cast<const CurveImp*>( curve->imp() )->getParam( c, d );
  return constrainedPointCalcer( curve, param );
}

ObjectHolder* ObjectFactory::constrainedPoint(
  ObjectCalcer* curve, const Coordinate& c, const KigDocument& d ) const
{
  return new ObjectHolder( constrainedPointCalcer( curve, c, d ) );
}

ObjectTypeCalcer* ObjectFactory::locusCalcer(
  ObjectCalcer* a, ObjectCalcer* b ) const
{
  assert( dynamic_cast<const ObjectTypeCalcer*>( a ) );
  ObjectTypeCalcer* constrained = static_cast<ObjectTypeCalcer*>( a );
  assert( constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) );
  assert( constrained->parents().size() == 2 );
  ObjectCalcer* curve = a->parents().back();

  const ObjectCalcer* moving = b;

  std::vector<ObjectCalcer*> hierparents;
  hierparents.push_back( constrained );
  std::vector<ObjectCalcer*> sideOfTree = sideOfTreePath( hierparents, moving );
  std::copy( sideOfTree.begin(), sideOfTree.end(), std::back_inserter( hierparents ) );

  ObjectHierarchy hier( hierparents, moving );

  std::vector<ObjectCalcer*> realparents( 2 + sideOfTree.size(), 0 );
  realparents[0] = new ObjectConstCalcer( new HierarchyImp( hier ) );
  realparents[1] = curve;
  copy( sideOfTree.begin(), sideOfTree.end(), realparents.begin() + 2 );

  return new ObjectTypeCalcer( LocusType::instance(), realparents );
}

ObjectHolder* ObjectFactory::locus( ObjectCalcer* a, ObjectCalcer* b ) const
{
  return new ObjectHolder( locusCalcer( a, b ) );
}

ObjectHolder* ObjectFactory::label(
  const QString& s, const Coordinate& loc,
  bool needframe, const std::vector<ObjectCalcer*>& parents,
  const KigDocument& doc ) const
{
  return new ObjectHolder( labelCalcer( s, loc, needframe, parents, doc ) );
}

ObjectTypeCalcer* ObjectFactory::labelCalcer(
 const QString& s, const Coordinate& loc,
 bool needframe, const std::vector<ObjectCalcer*>& parents,
 const KigDocument& doc ) const
{
  return attachedLabelCalcer( s, 0, loc, needframe, parents, doc );
}

ObjectTypeCalcer* ObjectFactory::attachedLabelCalcer(
  const QString& s, ObjectCalcer* p,
  const Coordinate& loc, bool needframe,
  const std::vector<ObjectCalcer*>& nparents,
  const KigDocument& doc ) const
{
  std::vector<ObjectCalcer*> parents;
  parents.reserve( nparents.size() + 3 );
  parents.push_back( new ObjectConstCalcer( new IntImp( needframe ? 1 : 0 ) ) );

  parents.push_back( getAttachPoint( p, loc, doc ) );

  parents.push_back( new ObjectConstCalcer( new StringImp( s ) ) );
  std::copy( nparents.begin(), nparents.end(), std::back_inserter( parents ) );
  ObjectTypeCalcer* ret = new ObjectTypeCalcer( TextType::instance(), parents );
  ret->calc( doc );
  return ret;
}

ObjectCalcer* ObjectFactory::getAttachPoint(
  ObjectCalcer* p,
  const Coordinate& loc,
  const KigDocument& doc ) const
{
/*
 * mp: (changes to add relative-attachment).  Now an object is tested
 * as follows:
 * - if attachPoint() returns a valid coordinate, then we use the new method
 * - if it is a point: 'old-style' treatment (we can change this shortly)
 * - if it is a curve: 'old-style' treatment (we could use the new approach,
 *   which can be better/worse depending on personal taste, I think)
 *
 * the first condition that matches determines the behaviour.
 * the new method works similarly to the curve case, but we generate a new
 * RelativePointType instead of a ConstrainedPointType; this will in turn make use
 * of the new attachPoint() method for objects.
 *
 * changed the preference order 2005/01/21 (now attachPoint has preference over points)
 *
 * NOTE: changes in the tests performed should be matched also in
 * modes/popup.cc (addNameLabel) and in label.cc (TextLabelModeBase::mouseMoved)
 */

  if ( p && p->imp()->attachPoint().valid() )
  {
    ObjectCalcer* o = relativePointCalcer( p, loc );
    o->calc( doc );
    return o;
  }
  else if ( p && p->imp()->inherits( PointImp::stype() ) )
  {
    return p;
  }
  else if ( p && p->imp()->inherits( CurveImp::stype() ) )
  {
    double param = 0.5;
    if ( loc.valid() )
      param = static_cast<const CurveImp*>( p->imp() )->getParam( loc, doc );

    ObjectCalcer* o = constrainedPointCalcer( p, param );
    o->calc( doc );
    return o;
  }
  else
  {
    if ( loc.valid() )
      return new ObjectConstCalcer( new PointImp( loc ) );
    else
      return new ObjectConstCalcer( new PointImp( Coordinate( 0, 0 ) ) );
  }
}

ObjectHolder* ObjectFactory::attachedLabel(
  const QString& s, ObjectCalcer* locationparent,
  const Coordinate& loc, bool needframe,
  const std::vector<ObjectCalcer*>& parents,
  const KigDocument& doc ) const
{
  return new ObjectHolder( attachedLabelCalcer( s, locationparent, loc, needframe, parents, doc ) );
}

ObjectPropertyCalcer* ObjectFactory::propertyObjectCalcer(
  ObjectCalcer* o, const char* p ) const
{
  int wp = o->imp()->propertiesInternalNames().findIndex( p );
  if ( wp == -1 ) return 0;
  return new ObjectPropertyCalcer( o, wp );
}

ObjectHolder* ObjectFactory::propertyObject(
  ObjectCalcer* o, const char* p ) const
{
  return new ObjectHolder( propertyObjectCalcer( o, p ) );
}

void ObjectFactory::redefinePoint(
  ObjectTypeCalcer* point, const Coordinate& c,
  KigDocument& doc, const KigWidget& w ) const
{
  std::vector<ObjectHolder*> hos = doc.whatAmIOn( c, w );
  std::vector<ObjectCalcer*> os;
  ObjectCalcer* (ObjectHolder::*calcmeth)() = &ObjectHolder::calcer;
  std::transform( hos.begin(), hos.end(), std::back_inserter( os ),
                  std::mem_fun( calcmeth ) );
  ObjectCalcer* v = 0;

  // we don't want one of our children as a parent...
  std::set<ObjectCalcer*> children = getAllChildren( point );
  for ( std::vector<ObjectCalcer*>::iterator i = os.begin();
        i != os.end(); ++i )
    if ( (*i)->imp()->inherits( CurveImp::stype() ) &&
         children.find( *i ) == children.end() )
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
      ObjectCalcer* dataobj = 0;
      std::vector<ObjectCalcer*> parents = point->parents();
      assert( parents.size() == 2 );
      assert ( parents[0]->imp()->inherits( DoubleImp::stype() ) );
      dataobj = parents[0];

      parents.clear();
      parents.push_back( dataobj );
      parents.push_back( v );
      point->setParents( parents );

      assert( dynamic_cast<ObjectConstCalcer*>( dataobj ) );
      static_cast<ObjectConstCalcer*>( dataobj )->setImp(
        new DoubleImp( newparam ) );
    }
    else
    {
      // point used to be fixed -> add a new DataObject etc.
      std::vector<ObjectCalcer*> args;
      args.push_back( new ObjectConstCalcer( new DoubleImp( newparam ) ) );
      args.push_back( v );
      point->setType( ConstrainedPointType::instance() );
      point->setParents( args );
    }
  }
  else
  {
    // a fixed point...
    if ( point->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
    {
      // point used to be constrained..
      std::vector<ObjectCalcer*> a;
      a.push_back( new ObjectConstCalcer( new DoubleImp( c.x ) ) );
      a.push_back( new ObjectConstCalcer( new DoubleImp( c.y ) ) );

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

