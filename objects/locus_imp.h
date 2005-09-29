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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_OBJECTS_LOCUS_IMP_H
#define KIG_OBJECTS_LOCUS_IMP_H

#include "curve_imp.h"
#include "../misc/object_hierarchy.h"

/**
 * LocusImp is an imp that consists of a copy of the curveimp that the
 * moving point moves over, and an ObjectHierarchy that can calc (
 * given a point, and optionally some more parent objects the position
 * of the moving point.  The hierarchy should take the moving point as
 * its *first* argument and all others after that.  The others are
 * used to make it possible for Locus to be updated when some of the
 * other objects that appear in the path from the moving point to the
 * dependent point change.
 *
 * This may seem rather complicated, but I think it is absolutely
 * necessary to support locuses using Kig's object system.  It would
 * be very bad for LocusImp to have to keep references to its parents
 * as Objects ( since only the objects know how they are related to
 * their parents ).  This is how we used to do it, but the current
 * method is far superior.  First and foremost because the separation
 * between ObjectImp and Object is something that Kig depends on very
 * much, and because every ObjectImp should contain all the data it
 * needs itself.  ObjectImp's are entirely independent objects.
 * That's also why we don't keep a pointer to the old CurveImp, but a
 * copy of it..
 *
 * i hope this is a bit clear, if not, feel free to ask for
 * explanation of what you don't understand..
 */
class LocusImp
  : public CurveImp
{
  CurveImp* mcurve;
  const ObjectHierarchy mhier;

  double getDist(double param, const Coordinate& p, const KigDocument& doc) const;
  void getInterval(double& x1,double& x2,double incr,const Coordinate& p, const KigDocument& doc) const;
  double getParamofmin(double a, double b, const Coordinate& p, const KigDocument& doc) const;
public:
  typedef CurveImp Parent;
  static const ObjectImpType* stype();

  LocusImp( CurveImp*, const ObjectHierarchy& );
  ~LocusImp();
  LocusImp* copy() const;

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  Rect surroundingRect() const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;
  double getParam( const Coordinate& point, const KigDocument& ) const;
  const Coordinate getPoint( double param, const KigDocument& ) const;

  // TODO ?
  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( uint which ) const;

  const CurveImp* curve() const;
  const ObjectHierarchy& hierarchy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool equals( const ObjectImp& rhs ) const;

  bool containsPoint( const Coordinate& p, const KigDocument& d ) const;
  bool internalContainsPoint( const Coordinate& p, double threshold, const KigDocument& doc ) const;
};

#endif
