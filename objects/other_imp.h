// other_imp.h
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef KIG_OBJECTS_OTHER_IMP_H
#define KIG_OBJECTS_OTHER_IMP_H

#include "curve_imp.h"
#include "../misc/coordinate.h"

class AngleImp
  : public ObjectImp
{
  const Coordinate mpoint;
  const double mstartangle;
  const double mangle;
public:
  typedef ObjectImp Parent;
  static const ObjectImpType* stype();

  AngleImp( const Coordinate& pt, double start_angle_in_radials,
            double angle_in_radials );
  ~AngleImp();

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;

  ObjectImp* copy() const;

  const double size() const;
  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  const Coordinate point() const { return mpoint; };
  const double startAngle() const { return mstartangle; };
  const double angle() const { return mangle; };

  bool equals( const ObjectImp& rhs ) const;
};

class VectorImp
  : public ObjectImp
{
  const Coordinate ma;
  const Coordinate mb;
public:
  typedef ObjectImp Parent;
  static const ObjectImpType* stype();

  VectorImp( const Coordinate& a, const Coordinate& b );
  ~VectorImp();

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;

  ObjectImp* copy() const;

  const Coordinate dir() const;
  const Coordinate a() const;
  const Coordinate b() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  bool equals( const ObjectImp& rhs ) const;
};

class ArcImp
  : public CurveImp
{
  Coordinate mcenter;
  double mradius;
  double msa;
  double ma;
public:
  typedef CurveImp Parent;
  static const ObjectImpType* stype();

  ArcImp( const Coordinate& center, const double radius,
          const double startangle, const double angle );
  ~ArcImp();
  ArcImp* copy() const;

  ObjectImp* transform( const Transformation& t ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& w ) const;
  bool inRect( const Rect& r, int width, const KigWidget& si ) const;
  bool valid() const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& d ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  double getParam( const Coordinate& c, const KigDocument& d ) const;
  const Coordinate getPoint( double p, bool& valid, const KigDocument& d ) const;

  const Coordinate center() const;
  double radius() const;
  double startAngle() const;
  double angle() const;

  bool equals( const ObjectImp& rhs ) const;
};

#endif
