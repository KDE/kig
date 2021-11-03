// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_CUBIC_IMP_H
#define KIG_OBJECTS_CUBIC_IMP_H

#include "curve_imp.h"

#include "../misc/cubic-common.h"


/**
 * An ObjectImp representing a cubic.
 */
class CubicImp
  : public CurveImp
{
  const CubicCartesianData mdata;
public:
  typedef CurveImp Parent;
  static const ObjectImpType* stype();

  explicit CubicImp( const CubicCartesianData& data );
  ~CubicImp();

  ObjectImp* transform( const Transformation& ) const override;
  void draw( KigPainter& p ) const override;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const override;
  bool inRect( const Rect& r, int width, const KigWidget& ) const override;
  Rect surroundingRect() const override;
  QString cartesianEquationString( const KigDocument& ) const;

  int numberOfProperties() const override;
  const QByteArrayList properties() const override;
  const QByteArrayList propertiesInternalNames() const override;
  ObjectImp* property( int which, const KigDocument& w ) const override;
  const char* iconForProperty( int which ) const override;
  const ObjectImpType* impRequirementForProperty( int which ) const override;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const override;

  CubicImp* copy() const override;

  double getParam( const Coordinate& point, const KigDocument& ) const override;

  // The getPoint function doesn't need the KigDocument argument, the
  // first getPoint function is identical to the other one.  It is
  // only provided for implementing the CurveImp interface.
  const Coordinate getPoint( double param, const KigDocument& ) const override;
  const Coordinate getPoint( double param ) const;

public:
  /**
   * Return the cartesian representation of this cubic.
   */
  const CubicCartesianData data() const;

  const ObjectImpType* type() const override;
  void visit( ObjectImpVisitor* vtor ) const override;

  bool equals( const ObjectImp& rhs ) const override;

  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const override;
  bool internalContainsPoint( const Coordinate& p, double threshold ) const;
  bool isVerticalCubic() const;
};

#endif
