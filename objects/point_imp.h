// point_imp.h
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

#ifndef KIG_OBJECTS_POINT_IMP_H
#define KIG_OBJECTS_POINT_IMP_H

#include "object_imp.h"
#include "../misc/coordinate.h"

class PointImp
  : public ObjectImp
{
  Coordinate mc;
public:
  typedef ObjectImp Parent;
  static const ObjectImpType* stype();

  PointImp( const Coordinate& c );
  ~PointImp();

  const Coordinate& coordinate() const { return mc; };
  void setCoordinate( const Coordinate& c );

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const;
  bool inRect( const Rect& r, int width, const KigWidget& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& d ) const;
  const char* iconForProperty( uint which ) const;
  const ObjectImpType* impRequirementForProperty( uint which ) const;

  ObjectImp* transform( const Transformation& ) const;

  PointImp* copy() const;

  const ObjectImpType* type() const;
  void visit( ObjectImpVisitor* vtor ) const;

  void fillInNextEscape( QString& s, const KigDocument& ) const;
  bool canFillInNextEscape() const;

  bool equals( const ObjectImp& rhs ) const;
};

#endif
