// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>

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

#include "script-common.h"

#include <qstring.h>

#include <kdebug.h>
#include <klocale.h>

struct script_prop
{
  const char* fillCodeStatement;
  const char* icon;
  const char* highlightStyle;
};

static const script_prop scripts_properties[] =
{
  { I18N_NOOP( "Now fill in the code:" ), "script", "" },
  { I18N_NOOP( "Now fill in the Python code:" ), "source_py", "Python-Kig" }
};

QString ScriptType::fillCodeStatement( ScriptType::Type type )
{
  return i18n( scripts_properties[type].fillCodeStatement );
}

QString ScriptType::templateCode( ScriptType::Type type, uint nargs )
{
  if ( type == Python )
  {
    QString tempcode = QString::fromLatin1( "def calc( " );
    bool firstarg = true;
    QString temparg = i18n( "Note to translators: this should be a default "
                            "name for an argument in a Python function. The "
                            "default is \"arg%1\" which would become arg1, "
                            "arg2, etc. Give something which seems "
                            "appropriate for your language.", "arg%1" );

    for ( uint i = 0; i < nargs; ++i )
    {
      if ( !firstarg ) tempcode += ", ";
      firstarg = false;
      tempcode += temparg.arg( i + 1 );
    };
    tempcode +=
      " ):\n"
      "\t# Calculate whatever you want to show here, and return it.\n"
      "\t# For example, to implement a mid point, you would put\n"
      "\t# this code here:\n"
      "\t#\treturn Point( ( arg1.coordinate() + arg2.coordinate() ) / 2 )\n"
      "\t# Please refer to the manual for more information.\n"
      "\n";
    return tempcode;
  }

  kdDebug() << "No such script type: " << type << endl;
  return "";
}

const char* ScriptType::icon( ScriptType::Type type )
{
  return scripts_properties[type].icon;
}

QString ScriptType::highlightStyle( ScriptType::Type type )
{
  return QString( scripts_properties[type].highlightStyle );
}

ScriptType::Type ScriptType::intToType( int type )
{
  if ( type == 1 )
    return Python;

  return Unknown;
}
