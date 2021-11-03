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
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const override;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const override;
  const ObjectImpType* resultId() const override;

  ObjectImp* calc( const Args& parents, const KigDocument& d ) const override;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& os ) const override;
  Args sortArgs( const Args& args ) const override;

  bool canMove( const ObjectTypeCalcer& ourobj ) const override;
  bool isFreelyTranslatable( const ObjectTypeCalcer& ourobj ) const override;
  std::vector<ObjectCalcer*> movableParents( const ObjectTypeCalcer& ourobj ) const override;
  const Coordinate moveReferencePoint( const ObjectTypeCalcer& ourobj ) const override;
  void move( ObjectTypeCalcer& ourobj, const Coordinate& to,
             const KigDocument& ) const override;

  QStringList specialActions() const override;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const override;

  const ArgsParser& argParser() const;
};

class TextType
  : public GenericTextType
{
  TextType();
  ~TextType();
public:
  static const TextType* instance();

  QStringList specialActions() const override;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const override;
};

class NumericTextType
  : public GenericTextType
{
  NumericTextType();
  ~NumericTextType();
public:
  static const NumericTextType* instance();

  QStringList specialActions() const override;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const override;
};
#endif
