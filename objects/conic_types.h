// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

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
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ConicBAAPType
  : public ArgsParserObjectType
{
  ConicBAAPType();
  ~ConicBAAPType();
public:
  static const ConicBAAPType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ConicBFFPType
  : public ArgsParserObjectType
{
protected:
  ConicBFFPType( const char* fullname, const ArgsParser::spec*, int n );
  ~ConicBFFPType();
public:
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;

  /**
   * -1 for hyperbola, 1 for ellipse.
   */
  virtual int type() const = 0;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class EllipseBFFPType
  : public ConicBFFPType
{
  EllipseBFFPType();
  ~EllipseBFFPType();
public:
  static const EllipseBFFPType* instance();
  int type() const Q_DECL_OVERRIDE;
};

class HyperbolaBFFPType
  : public ConicBFFPType
{
  HyperbolaBFFPType();
  ~HyperbolaBFFPType();
public:
  static const HyperbolaBFFPType* instance();
  int type() const Q_DECL_OVERRIDE;
};

class ConicBDFPType
  : public ArgsParserObjectType
{
  ConicBDFPType();
  ~ConicBDFPType();
public:
  static const ConicBDFPType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ParabolaBTPType
  : public ArgsParserObjectType
{
  ParabolaBTPType();
  ~ParabolaBTPType();
public:
  static const ParabolaBTPType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class EquilateralHyperbolaB4PType
  : public ArgsParserObjectType
{
  EquilateralHyperbolaB4PType();
  ~EquilateralHyperbolaB4PType();
public:
  static const EquilateralHyperbolaB4PType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ConicPolarPointType
  : public ArgsParserObjectType
{
  ConicPolarPointType();
  ~ConicPolarPointType();
public:
  static const ConicPolarPointType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ConicPolarLineType
  : public ArgsParserObjectType
{
  ConicPolarLineType();
  ~ConicPolarLineType();
public:
  static const ConicPolarLineType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ConicDirectrixType
  : public ArgsParserObjectType
{
  ConicDirectrixType();
  ~ConicDirectrixType();
public:
  static const ConicDirectrixType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ParabolaBDPType
  : public ObjectLPType
{
  ParabolaBDPType();
  ~ParabolaBDPType();
public:
  static const ParabolaBDPType* instance();
  using ObjectLPType::calc;
  ObjectImp* calc( const LineData& l, const Coordinate& c ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ConicAsymptoteType
  : public ArgsParserObjectType
{
  ConicAsymptoteType();
  ~ConicAsymptoteType();
public:
  static const ConicAsymptoteType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
};

class ConicRadicalType
  : public ArgsParserObjectType
{
  ConicRadicalType();
  ~ConicRadicalType();
public:
  static const ConicRadicalType* instance();
  ObjectImp* calc( const Args& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
  QStringList specialActions() const Q_DECL_OVERRIDE;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& t,
                      KigPart& d, KigWidget& w, NormalMode& m ) const Q_DECL_OVERRIDE;
};

#endif

