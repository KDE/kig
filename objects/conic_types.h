// conic_types.h
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

#ifndef KIG_OBJECTS_CONIC_TYPES_H
#define KIG_OBJECTS_CONIC_TYPES_H

#include "base_type.h"

class ConicB5PType
  : public ObjectType
{
public:
  ConicB5PType();
  ~ConicB5PType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents ) const;
};

class ConicBAAPType
  : public ObjectType
{
public:
  ConicBAAPType();
  ~ConicBAAPType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents ) const;
};

class ConicBFFPType
  : public ObjectABCType
{
protected:
  ConicBFFPType( const char* fullname );
  ~ConicBFFPType();
public:
  ObjectImp* calc( const Args& parents ) const;

  // -1 for hyperbola, 1 for ellipse..
  virtual int type() const = 0;
};

class EllipseBFFPType
  : public ConicBFFPType
{
public:
  EllipseBFFPType();
  ~EllipseBFFPType();
  ObjectType* copy() const;
  int type() const;
};

class HyperbolaBFFPType
  : public ConicBFFPType
{
public:
  HyperbolaBFFPType();
  ~HyperbolaBFFPType();
  ObjectType* copy() const;
  int type() const;
};

class ConicBDFPType
  : public ObjectType
{
public:
  ConicBDFPType();
  ~ConicBDFPType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents ) const;
};

class ParabolaBTPType
  : public ObjectABCType
{
public:
  ParabolaBTPType();
  ~ParabolaBTPType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents ) const;
};

class EquilateralHyperbolaB4PType
  : public ObjectType
{
public:
  EquilateralHyperbolaB4PType();
  ~EquilateralHyperbolaB4PType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents ) const;
};

class ConicPolarPointType
  : public ObjectType
{
public:
  ConicPolarPointType();
  ~ConicPolarPointType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents ) const;
};

class ConicPolarLineType
  : public ObjectType
{
public:
  ConicPolarLineType();
  ~ConicPolarLineType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents ) const;
};

class ConicDirectrixType
  : public ObjectType
{
public:
  ConicDirectrixType();
  ~ConicDirectrixType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents ) const;
};

class ParabolaBDPType
  : public ObjectLPType
{
public:
  ParabolaBDPType();
  ~ParabolaBDPType();
  ObjectType* copy() const;
  ObjectImp* calc( const LineData& l, const Coordinate& c ) const;
};

class ConicAsymptoteType
  : public ObjectType
{
public:
  ConicAsymptoteType();
  ~ConicAsymptoteType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents ) const;
};

class ConicRadicalType
  : public ObjectType
{
public:
  ConicRadicalType();
  ~ConicRadicalType();
  ObjectType* copy() const;
  ObjectImp* calc( const Args& parents ) const;
};

#endif

