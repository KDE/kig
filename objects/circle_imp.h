// circle_imp.h
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

#ifndef KIG_NW_OBJECTS_CIRCLE_IMP_H
#define KIG_NW_OBJECTS_CIRCLE_IMP_H

#include "conic_imp.h"

class CircleImp
  : public ConicImp
{
  typedef ConicImp Parent;

  Coordinate mcenter;
  double mradius;
public:
  CircleImp( const Coordinate& center, double radius );
  ~CircleImp();
  CircleImp* copy() const;
  bool inherits( int typeID ) const;
  const char* baseName() const;

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;
  bool valid() const;

  double getParam( const Coordinate& point, const KigDocument& ) const;
  const Coordinate getPoint( double param, const KigDocument& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;

  int id() const;

  // various information about us:
  const Coordinate center() const;
  double radius() const;
  // not currently used, but i might some day switch to storing the
  // square radius internally to avoid losing precision when doing
  // sqr( sqrt( ... ) )... so if you need the square radius, use this
  // func instead..
  double squareRadius() const;
  double surface() const;
  double circumference() const;

  // trivial versions of the conic information functions..
  int conicType() const;
  const ConicCartesianData cartesianData() const;
  const ConicPolarData polarData() const;
  Coordinate focus1() const;
  Coordinate focus2() const;

  QString cartesianEquationString( const KigDocument& w ) const;
  QString polarEquationString( const KigDocument& w ) const;
};

#endif
