// point_type.h
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

#ifndef POINT_TYPE_H
#define POINT_TYPE_H

#include "base_type.h"
#include "common.h"

class FixedPointType
  : public ObjectType
{
  static const FixedPointType sself;

  static const ArgParser::spec argsspec[1];
public:
  FixedPointType();
  ~FixedPointType();

  static const FixedPointType* instance();

  ObjectImp* calc( const Args& parents, const KigWidget& w ) const;
  bool canMove() const;
  void move( Object* ourobj, const Coordinate& from,
             const Coordinate& dist ) const;
};

class ConstrainedPointType
  : public ObjectType
{
  static const ConstrainedPointType sself;

  static const ArgParser::spec argsspec[2];
public:
  ConstrainedPointType();
  ~ConstrainedPointType();

  static const ConstrainedPointType* instance();

  ObjectImp* calc( const Args& parents, const KigWidget& w ) const;
  bool canMove() const;
  void move( Object* ourobj, const Coordinate& from,
             const Coordinate& dist ) const;
};

class MidPointType
  : public ObjectABType
{
  MidPointType();
  ~MidPointType();
public:
  static const MidPointType* instance();
  ObjectImp* calc( const Coordinate& a, const Coordinate& b ) const;
};

#endif
