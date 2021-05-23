// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

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
#include "conic_types.h"

#include "../kig/kig_document.h"
#include "../kig/kig_view.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate.h"
#include "../misc/object_hierarchy.h"
#include "../misc/special_constructors.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <iterator>

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

ObjectHolder* ObjectFactory::numericValue(
  const double value, const Coordinate& loc, const KigDocument& doc ) const
{
  return new ObjectHolder( numericValueCalcer( value, loc, doc ) );
}

ObjectTypeCalcer* ObjectFactory::numericValueCalcer(
  const double value, const Coordinate& loc, const KigDocument& doc ) const
{
  std::vector<ObjectCalcer*> parents;
  parents.reserve( 4 );
  const bool needframe = false;
  parents.push_back( new ObjectConstCalcer( new IntImp( needframe ? 1 : 0 ) ) );
  parents.push_back( getAttachPoint( 0, loc, doc ) );
  parents.push_back( new ObjectConstCalcer( new StringImp( QStringLiteral("%1") ) ) );
  parents.push_back( new ObjectConstCalcer( new DoubleImp( value ) ) );

  ObjectTypeCalcer* ret = new ObjectTypeCalcer( NumericTextType::instance(), parents );
  ret->calc( doc );
  return ret;
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
    // we can calc intersection point *only* between two objects...
    std::vector<ObjectCalcer*> args;
    int linecount = 0;
    int coniccount = 0;
    int circlecount = 0;  // Note: a circle is a conic
//    int cubiccount = 0;  // not yet implemented
    int lineid = -1;
    int conicid = -1;
//    int cubicid = -1;
    for (int i = 0; i < 2; i++)
    {
      if ( os[i]->imp()->inherits( AbstractLineImp::stype() ) ) {linecount++; lineid = i;}
      if ( os[i]->imp()->inherits( ConicImp::stype() ) ) {coniccount++; conicid = i;}
      if ( os[i]->imp()->inherits( CircleImp::stype() ) ) circlecount++;
//      if ( os[i]->imp()->inherits( CubicImp::stype() ) ) {cubiccount++; cubicid = i;}
    }
    if ( linecount == 2 )
    {
      // the simplest case: two lines...
      args.push_back( os[0]->calcer() );
      args.push_back( os[1]->calcer() );
      return new ObjectTypeCalcer( LineLineIntersectionType::instance(), args );
    }
    if ( circlecount == 2 || ( coniccount == 1 && linecount == 1 ) )
    {
      // in this case there are generally two intersections, we need
      // to check if one of these is already present, in which case
      // we must use the "Other" variant of the intersection types
      std::vector<ObjectCalcer*> points =
        d.findIntersectionPoints( os[0]->calcer(), os[1]->calcer() );
      std::vector<ObjectCalcer*> uniquepoints = removeDuplicatedPoints( points );
      if ( uniquepoints.size() == 1 )
      {
        bool doother = true;
        std::vector<ObjectCalcer*> parents = uniquepoints[0]->parents();
        if ( parents.size() == 3 )
        {
          if ( ( parents[0] == os[0]->calcer() && parents[1] == os[1]->calcer() )
            || ( parents[0] == os[1]->calcer() && parents[1] == os[0]->calcer() ) )
          {
            if ( parents[2]->imp()->inherits( IntImp::stype() ) )
              doother = false;  // we should test if the type is
                                // ConicLineIntersectionType or
                                // CircleCircleIntersectionType
          }
        }
        if ( doother )
        {
          // in this case we construct an OtherIntersection
printf ("*** trovata altra intersezione!\n");
          if ( circlecount == 2 )
          {
            args.push_back( os[0]->calcer() );
            args.push_back( os[1]->calcer() );
            args.push_back( uniquepoints[0] );
            return new ObjectTypeCalcer(
              CircleCircleOtherIntersectionType::instance(), args );
          } else {
            args.push_back( os[conicid]->calcer() );
            args.push_back( os[lineid]->calcer() );
            args.push_back( uniquepoints[0] );
            return new ObjectTypeCalcer(
              ConicLineOtherIntersectionType::instance(), args );
          }
        }
      }
    }
    if ( coniccount == 1 && linecount == 1 )
    {
      // conic-line intersection...
      const ConicImp* conic = static_cast<const ConicImp*>( os[conicid]->imp() );
      const AbstractLineImp* line = static_cast<const AbstractLineImp*>( os[lineid]->imp() );

      // we have two intersections, select the nearest one
      Coordinate p1, p2;
      if ( circlecount ) // in this case we cannot use the ConicLine computation
      {                  // because "which" behaves differently when "calc"-ing
        const CircleImp* c = static_cast<const CircleImp*>( conic );
        p1 = calcCircleLineIntersect(
          c->center(), c->squareRadius(), line->data(), -1 );
        p2 = calcCircleLineIntersect(
          c->center(), c->squareRadius(), line->data(), 1 );
      } else {
        p1 = calcConicLineIntersect(
          conic->cartesianData(), line->data(), 0.0, -1 );
        p2 = calcConicLineIntersect(
          conic->cartesianData(), line->data(), 0.0, 1 );
      }
      int which = -1;
      if ( (p2-c).length() < (p1-c).length() ) which = 1;
      args.push_back( os[conicid]->calcer() );
      args.push_back( os[lineid]->calcer() );
      args.push_back( new ObjectConstCalcer( new IntImp( which ) ) );
      return new ObjectTypeCalcer( ConicLineIntersectionType::instance(), args );
    }
    if ( circlecount == 2 )
    {
      // circle-circle intersection...
      const CircleImp* c1 = static_cast<const CircleImp*>( os[0]->imp() );
      const CircleImp* c2 = static_cast<const CircleImp*>( os[1]->imp() );
      const Coordinate o1 = c1->center();
      const Coordinate o2 = c2->center();
      const double r1sq = c1->squareRadius();
      const Coordinate a = calcCircleRadicalStartPoint(
        o1, o2, r1sq, c2->squareRadius()
        );
      const LineData lined = LineData (a, Coordinate ( a.x -o2.y + o1.y, a.y + o2.x - o1.x ));
      Coordinate p1 = calcCircleLineIntersect( o1, r1sq, lined, -1 );
      Coordinate p2 = calcCircleLineIntersect( o1, r1sq, lined, 1 );
      int which = -1;
      if ( (p2-c).length() < (p1-c).length() ) which = 1;
      args.push_back( os[0]->calcer() );
      args.push_back( os[1]->calcer() );
      args.push_back( new ObjectConstCalcer( new IntImp( which ) ) );
      return new ObjectTypeCalcer( CircleCircleIntersectionType::instance(), args );
    }
    if ( coniccount == 2 )
    {
      // conic-conic intersection...
      const ConicImp* conic1 = static_cast<const ConicImp*>( os[0]->imp() );
      const ConicImp* conic2 = static_cast<const ConicImp*>( os[1]->imp() );
      bool valid;
      double d1, d2, d3, d4;
      Coordinate p1, p2, p3, p4;
      d1 = d2 = d3 = d4 = std::numeric_limits<double>::max();
      const LineData l1 = calcConicRadical(
        static_cast<const ConicImp*>( conic1 )->cartesianData(),
        static_cast<const ConicImp*>( conic2 )->cartesianData(),
        -1, 1, valid);
      if ( valid )
      {
        p1 = calcConicLineIntersect(
            conic1->cartesianData(), l1, 0.0, -1 );
        p2 = calcConicLineIntersect(
            conic1->cartesianData(), l1, 0.0, 1 );
        d1 = (p1-c).length();
        d2 = (p2-c).length();
      }
      const LineData l2 = calcConicRadical(
        static_cast<const ConicImp*>( conic1 )->cartesianData(),
        static_cast<const ConicImp*>( conic2 )->cartesianData(),
        1, 1, valid);
      if ( valid )
      {
        p3 = calcConicLineIntersect(
            conic1->cartesianData(), l2, 0.0, -1 );
        p4 = calcConicLineIntersect(
            conic1->cartesianData(), l2, 0.0, 1 );
        d3 = (p3-c).length();
        d4 = (p4-c).length();
      }
      double d12 = fmin( d1, d2 );
      double d34 = fmin( d3, d4 );
      // test which is the right point, by now just choose p1
      int whichline = -1;
      if ( d34 < d12 )
      {
        whichline = 1;
        d1 = d3;
        d2 = d4;
      }
      int whichpoint = -1;
      if ( d2 < d1 ) whichpoint = 1;
      args.push_back( os[0]->calcer() );
      args.push_back( os[1]->calcer() );
      args.push_back( new ObjectConstCalcer( new IntImp( whichline ) ) );
      args.push_back( new ObjectConstCalcer( new IntImp( 1 ) ) );
      ObjectTypeCalcer* radical =
          new ObjectTypeCalcer( ConicRadicalType::instance(), args );
      radical->calc( d );
      args.clear();
      args.push_back( os[0]->calcer() );
      args.push_back( radical );
      args.push_back( new ObjectConstCalcer( new IntImp( whichpoint ) ) );
      return new ObjectTypeCalcer( ConicLineIntersectionType::instance(), args );
    }
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

ObjectTypeCalcer* ObjectFactory::constrainedRelativePointCalcer(
  ObjectCalcer* curve, double param ) const
{
  assert( curve->imp()->inherits( CurveImp::stype() ) );
  std::vector<ObjectCalcer*> parents;
  parents.push_back( new ObjectConstCalcer( new DoubleImp( 0.0 ) ) );
  parents.push_back( new ObjectConstCalcer( new DoubleImp( 0.0 ) ) );
  parents.push_back( new ObjectConstCalcer( new DoubleImp( param ) ) );
  parents.push_back( curve );
  return new ObjectTypeCalcer( ConstrainedRelativePointType::instance(), parents );
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
  std::copy( sideOfTree.begin(), sideOfTree.end(), realparents.begin() + 2 );

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
 * - if it is a curve: use the new (nov 2009) ConstrainedRelativePoint 
 *   similar to the RelativePoint
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

    ObjectCalcer* o = constrainedRelativePointCalcer( p, param );
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
  int wp = o->imp()->propertiesInternalNames().indexOf( p );
  if ( wp == -1 ) return 0;
  return new ObjectPropertyCalcer( o, p );
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

