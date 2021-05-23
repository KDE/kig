// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_SCRIPTING_SCRIPT_COMMON_H
#define KIG_SCRIPTING_SCRIPT_COMMON_H

#include <algorithm>
#include <list>

#include "../objects/object_holder.h"

class QString;

class ScriptType
{
public:
  /**
   * This enum represents all the script language types actually in
   * Kig. The first type ( Unknown ) can be used if we don't want
   * particular tunings for a script language.
   */
  enum Type { Unknown = 0, Python = 1 };
  /**
   * Returns an i18n'ed statement like 'Now fill in the code:' with
   * the name of the script language.
   */
  static QString fillCodeStatement( ScriptType::Type type );
  /**
   * Returns a template code for a script language.
   */
  static QString templateCode( ScriptType::Type type, std::list<ObjectHolder*> args );
  /**
   * Update the existing script function definition with the new argument selection
   */
  static void updateCodeFunction( ScriptType::Type type, std::list<ObjectHolder*> args, QString & script);
  /**
   * Return the function definition for the script
   */
  static QString scriptFunctionDefinition( ScriptType::Type type, std::list<ObjectHolder*> args );
  /**
   * Returns the icon's name for a script language.
   */
  static const char* icon( ScriptType::Type type );
  /**
   * Returns the Kate highlight stytle name for a script language.
   */
  static QString highlightStyle( ScriptType::Type type );
  /**
   * Converts an int to a ScriptType::Type. Useful when reading script
   * types from files.
   */
  static ScriptType::Type intToType( int type );
};

#endif
