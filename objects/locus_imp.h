// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

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
 * copy of it.
 *
 * i hope this is a bit clear, if not, feel free to ask for
 * explanation of what you don't understand.
 */

/**
 * NOTE: getParam has been moved by Petr Gajdos at the "curve_imp" level
 * which should be fine, since it makes sense more generally for curves
 * and not only for locuses.  In particular it will be useful for the
 * new "bezier" curves, to be added shortly
 */

class LocusImp
  : public CurveImp
{
  CurveImp* mcurve;
  const ObjectHierarchy mhier;

  void getInterval(double& x1,double& x2,double incr,const Coordinate& p, const KigDocument& doc) const;
public:
  typedef CurveImp Parent;
  static const ObjectImpType* stype();

  LocusImp( CurveImp*, const ObjectHierarchy& );
  ~LocusImp();
  LocusImp* copy() const Q_DECL_OVERRIDE;

  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;

  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  Rect surroundingRect() const Q_DECL_OVERRIDE;
  bool inRect( const Rect& r, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  const Coordinate getPoint( double param, const KigDocument& ) const Q_DECL_OVERRIDE;

  // TODO ?
  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  const CurveImp* curve() const;
  const ObjectHierarchy& hierarchy() const;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  bool equals( const ObjectImp& rhs ) const Q_DECL_OVERRIDE;

  bool containsPoint( const Coordinate& p, const KigDocument& d ) const Q_DECL_OVERRIDE;
  bool internalContainsPoint( const Coordinate& p, double threshold, const KigDocument& doc ) const;

  //virtual QString LocusTypeString(const KigDocument& w) const;

};

#endif
