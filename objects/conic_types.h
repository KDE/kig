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
  : public ArgparserObjectType
{
  ConicB5PType();
  ~ConicB5PType();
public:
  static const ConicB5PType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicBAAPType
  : public ArgparserObjectType
{
  ConicBAAPType();
  ~ConicBAAPType();
public:
  static const ConicBAAPType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicBFFPType
  : public ArgparserObjectType
{
protected:
  ConicBFFPType( const char* fullname, const ArgParser::spec*, int n );
  ~ConicBFFPType();
public:
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;

  // -1 for hyperbola, 1 for ellipse..
  virtual int type() const = 0;
  const ObjectImpType* resultId() const;
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
  : public ArgparserObjectType
{
  ConicBDFPType();
  ~ConicBDFPType();
public:
  static const ConicBDFPType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ParabolaBTPType
  : public ArgparserObjectType
{
  ParabolaBTPType();
  ~ParabolaBTPType();
public:
  static const ParabolaBTPType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class EquilateralHyperbolaB4PType
  : public ArgparserObjectType
{
  EquilateralHyperbolaB4PType();
  ~EquilateralHyperbolaB4PType();
public:
  static const EquilateralHyperbolaB4PType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicPolarPointType
  : public ArgparserObjectType
{
  ConicPolarPointType();
  ~ConicPolarPointType();
public:
  static const ConicPolarPointType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicPolarLineType
  : public ArgparserObjectType
{
  ConicPolarLineType();
  ~ConicPolarLineType();
public:
  static const ConicPolarLineType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicDirectrixType
  : public ArgparserObjectType
{
  ConicDirectrixType();
  ~ConicDirectrixType();
public:
  static const ConicDirectrixType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ParabolaBDPType
  : public ObjectLPType
{
  ParabolaBDPType();
  ~ParabolaBDPType();
public:
  static const ParabolaBDPType* instance();
  ObjectImp* calc( const LineData& l, const Coordinate& c ) const;
  const ObjectImpType* resultId() const;
};

class ConicAsymptoteType
  : public ArgparserObjectType
{
  ConicAsymptoteType();
  ~ConicAsymptoteType();
public:
  static const ConicAsymptoteType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicRadicalType
  : public ArgparserObjectType
{
  ConicRadicalType();
  ~ConicRadicalType();
public:
  static const ConicRadicalType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif

