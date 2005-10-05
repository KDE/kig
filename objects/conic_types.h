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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_OBJECTS_CONIC_TYPES_H
#define KIG_OBJECTS_CONIC_TYPES_H

#include "base_type.h"

class ConicB5PType
  : public ArgsParserObjectType
{
  ConicB5PType();
  ~ConicB5PType();
public:
  static const ConicB5PType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicBAAPType
  : public ArgsParserObjectType
{
  ConicBAAPType();
  ~ConicBAAPType();
public:
  static const ConicBAAPType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicBFFPType
  : public ArgsParserObjectType
{
protected:
  ConicBFFPType( const char* fullname, const ArgsParser::spec*, int n );
  ~ConicBFFPType();
public:
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;

  /**
   * -1 for hyperbola, 1 for ellipse..
   */
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
  : public ArgsParserObjectType
{
  ConicBDFPType();
  ~ConicBDFPType();
public:
  static const ConicBDFPType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ParabolaBTPType
  : public ArgsParserObjectType
{
  ParabolaBTPType();
  ~ParabolaBTPType();
public:
  static const ParabolaBTPType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class EquilateralHyperbolaB4PType
  : public ArgsParserObjectType
{
  EquilateralHyperbolaB4PType();
  ~EquilateralHyperbolaB4PType();
public:
  static const EquilateralHyperbolaB4PType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicPolarPointType
  : public ArgsParserObjectType
{
  ConicPolarPointType();
  ~ConicPolarPointType();
public:
  static const ConicPolarPointType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicPolarLineType
  : public ArgsParserObjectType
{
  ConicPolarLineType();
  ~ConicPolarLineType();
public:
  static const ConicPolarLineType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicDirectrixType
  : public ArgsParserObjectType
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
  : public ArgsParserObjectType
{
  ConicAsymptoteType();
  ~ConicAsymptoteType();
public:
  static const ConicAsymptoteType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

class ConicRadicalType
  : public ArgsParserObjectType
{
  ConicRadicalType();
  ~ConicRadicalType();
public:
  static const ConicRadicalType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
  QStringList specialActions() const;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& t,
                      KigPart& d, KigWidget& w, NormalMode& m ) const;
};

#endif

