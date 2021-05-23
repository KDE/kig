// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_SCRIPTING_PYTHON_SCRIPTER_H
#define KIG_SCRIPTING_PYTHON_SCRIPTER_H

#include "../objects/common.h"

#include <string>

class KigDocument;
class ObjectImp;

class CompiledPythonScript
{
  friend class PythonScripter;
  class Private;
  Private* const d;
  CompiledPythonScript( Private* );
public:
  CompiledPythonScript( const CompiledPythonScript& s );
  ~CompiledPythonScript();
  ObjectImp* calc( const Args& a, const KigDocument& doc );

  bool valid();
};

class PythonScripter
{
  friend class CompiledPythonScript;
  class Private;
  Private* d;
  PythonScripter();
  ~PythonScripter();

  void clearErrors();
  void saveErrors();

  bool erroroccurred;
  std::string lastexceptiontype;
  std::string lastexceptionvalue;
  std::string lastexceptiontraceback;
public:
  static PythonScripter* instance();

  bool errorOccurred() const;
  std::string lastErrorExceptionType() const;
  std::string lastErrorExceptionValue() const;
  std::string lastErrorExceptionTraceback() const;

  CompiledPythonScript compile( const char* code );
  ObjectImp* calc( CompiledPythonScript& script, const Args& args );
};

#endif
