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
  ConicB5PType();
  ~ConicB5PType();
public:
  static const ConicB5PType* instance();
  ObjectImp* calc( const Args& parents, const KigWidget& w ) const;
};

class ConicBAAPType
  : public ObjectType
{
  ConicBAAPType();
  ~ConicBAAPType();
public:
  static const ConicBAAPType* instance();
  ObjectImp* calc( const Args& parents, const KigWidget& w ) const;
};

class ConicBFFPType
  : public ObjectABCType
{
protected:
  ConicBFFPType( const char* fullname );
  ~ConicBFFPType();
public:
  ObjectImp* calc( const Args& parents, const KigWidget& w ) const;

  // -1 for hyperbola, 1 for ellipse..
  virtual int type() const = 0;
};

class EllipseBFFPType
  : public ConicBFFPType
{
  EllipseBFFPType();
  ~EllipseBFFPType();
public:
  static const EllipseBFFPType* instance();
  int type() const;
};

class HyperbolaBFFPType
  : public ConicBFFPType
{
  HyperbolaBFFPType();
  ~HyperbolaBFFPType();
public:
  static const HyperbolaBFFPType* instance();
  int type() const;
};

class ConicBDFPType
  : public ObjectType
{
  ConicBDFPType();
  ~ConicBDFPType();
public:
  static const ConicBDFPType* instance();
  ObjectImp* calc( const Args& parents, const KigWidget& w ) const;
};

class ParabolaBTPType
  : public ObjectABCType
{
  ParabolaBTPType();
  ~ParabolaBTPType();
public:
  static const ParabolaBTPType* instance();
  ObjectImp* calc( const Args& parents, const KigWidget& w ) const;
};

class ConicPolarPointType
  : public ObjectType
{
  ConicPolarPointType();
  ~ConicPolarPointType();
public:
  static const ConicPolarPointType* instance();
  ObjectImp* calc( const Args& parents, const KigWidget& ) const;
};

class ConicPolarLineType
  : public ObjectType
{
  ConicPolarLineType();
  ~ConicPolarLineType();
public:
  static const ConicPolarLineType* instance();
  ObjectImp* calc( const Args& parents, const KigWidget& ) const;
};

#endif
