// line_imp.h
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

#ifndef KIG_OBJECTS_LINE_IMP_H
#define KIG_OBJECTS_LINE_IMP_H

#include "curve_imp.h"

#include "../misc/common.h"

class LineData;

/**
 * An ObjectImp class that is the base of the line-like ObjectImp's:
 * SegmentImp, LineImp and RayImp..
 */
class AbstractLineImp
  : public CurveImp
{
protected:
  LineData mdata;
  AbstractLineImp( const LineData& d );
  AbstractLineImp( const Coordinate& a, const Coordinate& b );

public:
  typedef CurveImp Parent;
  /**
   * Returns the ObjectImpType representing the AbstractLineImp
   * type..
   */
  static const ObjectImpType* stype();

  ~AbstractLineImp();

  bool inRect( const Rect& r, int width, const KigWidget& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& d ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;

  /**
   * Get the slope of this AbstractLineImp..  For a line through
   * points a( xa, ya ) and b ( xb, yb ), this means the value ( yb -
   * ya ) / ( xb - xa ).
   */
  double slope() const;
  /**
   * Get a string containing the equation of this line in the form "y
   * = a * x + b ".
   */
  const QString equationString() const;
  /**
   * Get the LineData for this AbstractLineImp.
   */
  LineData data() const;

  bool equals( const ObjectImp& rhs ) const;
};

/**
 * An ObjectImp representing a segment
 */
class SegmentImp
  : public AbstractLineImp
{
public:
  typedef AbstractLineImp Parent;
  /**
   * Returns the ObjectImpType representing the SegmentImp
   * type..
   */
  static const ObjectImpType* stype();

  /**
   * Construct a new segment from point a to point b.
   */
  SegmentImp( const Coordinate& a, const Coordinate& b );
  /**
   * Construct a new segment from a LineData.
   */
  SegmentImp( const LineData& d );

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& si ) const;

  ObjectImp* transform( const Transformation& ) const;

  const Coordinate getPoint( double param, const KigDocument& ) const;
  double getParam( const Coordinate&, const KigDocument& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& d ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;

  SegmentImp* copy() const;

  /**
   * Get the length of this segment.
   */
  double length() const;


  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;
};

/**
 * An ObjectImp representing a ray. This means half of a line, it is
 * infinite in one direction, but ends at a certain point in the other
 * direction..
 */
class RayImp
  : public AbstractLineImp
{
public:
  typedef AbstractLineImp Parent;
  /**
   * Returns the ObjectImpType representing the RayImp
   * type..
   */
  static const ObjectImpType* stype();

  /**
   * Construct a ray, starting at a, and going through b.
   */
  RayImp( const Coordinate& a, const Coordinate& b );
  /**
   * Construct a ray from a LineData.
   */
  RayImp( const LineData& d );

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& si ) const;

  ObjectImp* transform( const Transformation& ) const;

  const Coordinate getPoint( double param, const KigDocument& ) const;
  double getParam( const Coordinate&, const KigDocument& ) const;

  RayImp* copy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;
};

/**
 * An ObjectImp representing a line.
 */
class LineImp
  : public AbstractLineImp
{
public:
  typedef AbstractLineImp Parent;

  /**
   * Returns the ObjectImpType representing the LineImp
   * type..
   */
  static const ObjectImpType* stype();

  /**
   * Construct a LineImp going through points a and b.
   */
  LineImp( const Coordinate& a, const Coordinate& b );
  /**
   * Construct a LineImp from a LineData.
   */
  LineImp( const LineData& d );
  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& si ) const;

  ObjectImp* transform( const Transformation& ) const;

  const Coordinate getPoint( double param, const KigDocument& ) const;
  double getParam( const Coordinate&, const KigDocument& ) const;

  LineImp* copy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;
};

#endif
