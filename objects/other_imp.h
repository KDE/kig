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

#include "object_imp.h"
#include "../misc/coordinate.h"

class AngleImp
  : public ObjectImp
{
  typedef ObjectImp Parent;
  const Coordinate mpoint;
  const double mstartangle;
  const double mangle;
public:
  AngleImp( const Coordinate& pt, double start_angle_in_radials,
            double angle_in_radials );
  ~AngleImp();

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, const ScreenInfo& si ) const;
  bool inRect( const Rect& r ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const Property property( uint which, const KigWidget& w ) const;

  bool inherits( int typeID ) const;
  ObjectImp* copy() const;
  const char* baseName() const;

  const double size() const;
};

class VectorImp
  : public ObjectImp
{
  typedef ObjectImp Parent;
  const Coordinate ma;
  const Coordinate mb;
public:
  VectorImp( const Coordinate& a, const Coordinate& b );
  ~VectorImp();

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, const ScreenInfo& si ) const;
  bool inRect( const Rect& r ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const Property property( uint which, const KigWidget& w ) const;

  bool inherits( int typeID ) const;
  ObjectImp* copy() const;
  const char* baseName() const;

  const Coordinate a() const;
  const Coordinate b() const;
  const Coordinate dir() const;
};
#endif
