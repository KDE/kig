// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_SCRIPTING_PYTHON_TYPE_H
#define KIG_SCRIPTING_PYTHON_TYPE_H

#include "../objects/object_type.h"

class PythonCompileType
  : public ObjectType
{
  PythonCompileType();
  ~PythonCompileType();
public:
  static const PythonCompileType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& d ) const override;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const override;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const override;
  const ObjectImpType* resultId() const override;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const override;
  Args sortArgs( const Args& args ) const override;
};

class PythonExecuteType
  : public ObjectType
{
  PythonExecuteType();
  ~PythonExecuteType();
public:
  static const PythonExecuteType* instance();

  ObjectImp* calc( const Args& parents, const KigDocument& d ) const override;

  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const override;
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const override;
  const ObjectImpType* resultId() const override;

  std::vector<ObjectCalcer*> sortArgs( const std::vector<ObjectCalcer*>& args ) const override;
  Args sortArgs( const Args& args ) const override;

//   virtual QStringList specialActions() const;
//   virtual void executeAction( int i, RealObject* o, KigDocument& d, KigWidget& w,
//                               NormalMode& m ) const;
};

#endif
