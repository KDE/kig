// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_TEXT_TYPE_H
#define KIG_OBJECTS_TEXT_TYPE_H

#include "object_type.h"

class GenericTextType
  : public ObjectType
{
  const ArgsParser mparser;
protected:
  GenericTextType( const char* fulltypename );
  ~GenericTextType();
public:
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const Q_DECL_OVERRIDE;
  const ObjectImpType* resultId() const Q_DECL_OVERRIDE;

  ObjectImp* calc( const Args& parents, const KigDocument& d ) const Q_DECL_OVERRIDE;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& os ) const Q_DECL_OVERRIDE;
  Args sortArgs( const Args& args ) const Q_DECL_OVERRIDE;

  bool canMove( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  bool isFreelyTranslatable( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& ourobj ) const Q_DECL_OVERRIDE;
  void move( ObjectTypeCalcer& ourobj, const Coordinate& to,
             const KigDocument& ) const Q_DECL_OVERRIDE;

  QStringList specialActions() const Q_DECL_OVERRIDE;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const Q_DECL_OVERRIDE;

  const ArgsParser& argParser() const;
};

class TextType
  : public GenericTextType
{
  TextType();
  ~TextType();
public:
  static const TextType* instance();

  QStringList specialActions() const Q_DECL_OVERRIDE;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const Q_DECL_OVERRIDE;
};

class NumericTextType
  : public GenericTextType
{
  NumericTextType();
  ~NumericTextType();
public:
  static const NumericTextType* instance();

  QStringList specialActions() const Q_DECL_OVERRIDE;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const Q_DECL_OVERRIDE;
};
#endif
