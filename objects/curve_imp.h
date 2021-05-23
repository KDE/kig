// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_CURVE_IMP_H
#define KIG_OBJECTS_CURVE_IMP_H

#include "object_imp.h"

/**
 * This class represents a curve: something which is composed of
 * points, like a line, a circle, a locus.
 */
class CurveImp
  : public ObjectImp
{
private:
  double revert (int n) const;
protected:
  // following two functions are used by generic getParam()
  double getParamofmin( double a, double b,
                        const Coordinate& p,
                        const KigDocument& doc ) const;
  double getDist(double param, const Coordinate& p, const KigDocument& doc) const;
public:
  typedef ObjectImp Parent;

  /**
   * Returns the ObjectImpType representing the CurveImp type.
   */
  static const ObjectImpType* stype();

  Coordinate attachPoint() const Q_DECL_OVERRIDE;

  // param is between 0 and 1.  Note that 0 and 1 should be the
  // end-points.  E.g. for a Line, getPoint(0) returns a more or less
  // infinite point.  getPoint(0.5) should return the point in the
  // middle.
  virtual double getParam( const Coordinate& point, const KigDocument& ) const;
  // this should be the inverse function of getPoint().
  // Note that it should also do something reasonable when p is not on
  // the curve.  You can return an invalid Coordinate(
  // Coordinate::invalidCoord() ) if you need to in some cases.
  virtual const Coordinate getPoint( double param, const KigDocument& ) const = 0;

  CurveImp* copy() const Q_DECL_OVERRIDE = 0;

  /**
   * Return whether this Curve contains the given point.  This is
   * implemented as a numerical approximation.  Implementations
   * can/should use the value test_threshold in common.h as a
   * threshold value.
   */
  virtual bool containsPoint( const Coordinate& p, const KigDocument& ) const = 0;
  QString cartesianEquationString( const KigDocument& w ) const;
};

#endif
