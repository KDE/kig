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

class AbstractLineImp
  : public CurveImp
{
protected:
  LineData mdata;

public:
  typedef CurveImp Parent;
  static const ObjectImpType* stype();

  AbstractLineImp( const LineData& d );
  AbstractLineImp( const Coordinate& a, const Coordinate& b );
  ~AbstractLineImp();

  bool inRect( const Rect& r, int width, const KigWidget& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& d ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;

  double slope() const;
  const QString equationString() const;
  LineData data() const;

  bool equals( const ObjectImp& rhs ) const;
};

class SegmentImp
  : public AbstractLineImp
{
public:
  typedef AbstractLineImp Parent;
  static const ObjectImpType* stype();

  SegmentImp( const Coordinate& a, const Coordinate& b );
  SegmentImp( const LineData& d );

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& si ) const;

  ObjectImp* transform( const Transformation& ) const;

  const Coordinate getPoint( double param, bool& valid, const KigDocument& ) const;
  double getParam( const Coordinate&, const KigDocument& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& d ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;

  SegmentImp* copy() const;

  double length() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;
};

class RayImp
  : public AbstractLineImp
{
public:
  typedef AbstractLineImp Parent;
  static const ObjectImpType* stype();

  RayImp( const Coordinate& a, const Coordinate& b );
  RayImp( const LineData& d );

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& si ) const;

  ObjectImp* transform( const Transformation& ) const;

  const Coordinate getPoint( double param, bool& valid, const KigDocument& ) const;
  double getParam( const Coordinate&, const KigDocument& ) const;

  RayImp* copy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;
};

class LineImp
  : public AbstractLineImp
{
public:
  typedef AbstractLineImp Parent;
  static const ObjectImpType* stype();

  LineImp( const Coordinate& a, const Coordinate& b );
  LineImp( const LineData& d );
  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& si ) const;

  ObjectImp* transform( const Transformation& ) const;

  const Coordinate getPoint( double param, bool& valid, const KigDocument& ) const;
  double getParam( const Coordinate&, const KigDocument& ) const;

  LineImp* copy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;
};

#endif
