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

  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;
  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool inRect( const Rect& r, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  Rect surroundingRect() const Q_DECL_OVERRIDE;
  QString cartesianEquationString( const KigDocument& ) const;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  CubicImp* copy() const Q_DECL_OVERRIDE;

  double getParam( const Coordinate& point, const KigDocument& ) const Q_DECL_OVERRIDE;

  // The getPoint function doesn't need the KigDocument argument, the
  // first getPoint function is identical to the other one.  It is
  // only provided for implementing the CurveImp interface.
  const Coordinate getPoint( double param, const KigDocument& ) const Q_DECL_OVERRIDE;
  const Coordinate getPoint( double param ) const;

public:
  /**
   * Return the cartesian representation of this cubic.
   */
  const CubicCartesianData data() const;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  bool equals( const ObjectImp& rhs ) const Q_DECL_OVERRIDE;

  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const Q_DECL_OVERRIDE;
  bool internalContainsPoint( const Coordinate& p, double threshold ) const;
  bool isVerticalCubic() const;
};

#endif
