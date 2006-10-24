// Copyright (C) 2004       Pino Toscano <toscano.pino@tiscali.it>

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
