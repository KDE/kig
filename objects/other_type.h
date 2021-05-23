// SPDX-FileCopyrightText: 2003-2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MISC_OTHER_TYPE_H
#define KIG_MISC_OTHER_TYPE_H

#include "base_type.h"
#include "../misc/object_hierarchy.h"

class LocusType
  : public ArgsParserObjectType
{
  typedef ArgsParserObjectType Parent;
  LocusType();
  ~LocusType();
public:
  static const LocusType* instance();

  ObjectImp* calc( const Args& args, const KigDocument& ) const Q_DECL_OVERRIDE;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;

  bool inherits( int type ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const Q_DECL_OVERRIDE;
  Args sortArgs( const Args& args ) const Q_DECL_OVERRIDE;
};

class CopyObjectType
  : public ObjectType
{
protected:
  CopyObjectType();
  ~CopyObjectType();
public:
  static CopyObjectType* instance();
  bool inherits( int type ) const Q_DECL_OVERRIDE;
  ObjectImp* calc( const Args& parents, const KigDocument& d ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& os ) const Q_DECL_OVERRIDE;
  Args sortArgs( const Args& args ) const Q_DECL_OVERRIDE;
};

#endif
