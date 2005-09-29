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

#ifndef KIG_OBJECTS_CUBIC_IMP_H
#define KIG_OBJECTS_CUBIC_IMP_H

#include "curve_imp.h"

#include "../misc/cubic-common.h"

class LineData;

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

  CubicImp( const CubicCartesianData& data );
  ~CubicImp();

  ObjectImp* transform( const Transformation& ) const;
  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;
  Rect surroundingRect() const;
  QString cartesianEquationString( const KigDocument& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;
  bool isPropertyDefinedOnOrThroughThisImp( uint which ) const;

  CubicImp* copy() const;

  double getParam( const Coordinate& point, const KigDocument& ) const;

  // The getPoint function doesn't need the KigDocument argument, the
  // first getPoint function is identical to the other one.  It is
  // only provided for implementing the CurveImp interface.
  const Coordinate getPoint( double param, const KigDocument& ) const;
  const Coordinate getPoint( double param ) const;

public:
  /**
   * Return the cartesian representation of this cubic.
   */
  const CubicCartesianData data() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool equals( const ObjectImp& rhs ) const;

  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const;
  bool internalContainsPoint( const Coordinate& p, double threshold ) const;
};

#endif
