// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "script-common.h"
#include "../objects/text_imp.h"

#include <QString>

#include <QDebug>

struct script_prop
{
  const char* fillCodeStatement;
  const char* icon;
  const char* highlightStyle;
};

static const script_prop scripts_properties[] =
{
  { I18N_NOOP( "Now fill in the code:" ), "application-x-thoshellscript", 0 },
  { I18N_NOOP( "Now fill in the Python code:" ), "text-x-python", "Python-Kig" }
};

QString ScriptType::fillCodeStatement( ScriptType::Type type )
{
  return i18n( scripts_properties[type].fillCodeStatement );
}

QString ScriptType::templateCode( ScriptType::Type type, std::list<ObjectHolder*> args )
{
  if ( type == Python )
  {
    QString tempcode { ScriptType::scriptFunctionDefinition( type, args ) };
    tempcode +=
      "\n"
      "\t# Calculate whatever you want to show here, and return it.\n";
    if ( args.empty() )
    {
      tempcode +=
        "\t# For example, to return the number pi, you would put\n"
        "\t# this code here:\n"
        "\t#\treturn DoubleObject( 4*atan(1.0) )\n";
    } else {
      if ( ! args.empty() && (*args.begin())->imp()->inherits( NumericTextImp::stype() ) )
      {
        tempcode +=
          "\t# For example, to return one half of the input number,\n"
          "\t# you would put this code here:\n"
          "\t#\treturn DoubleObject( arg1.value()/ 2 )\n";
      } else if ( ! args.empty() && (*args.begin())->imp()->inherits( BoolTextImp::stype() ) )
      {
        tempcode +=
          "\t# For example, to return a string based on the test result,\n"
          "\t# you would put this code here:\n"
          "\t#\tif arg1.value():\n"
          "\t#\t\treturn StringObject( \"TRUE!\" )\n"
          "\t#\telse:\n"
          "\t#\t\treturn StringObject( \"FALSE!\" )\n";
      } else {
        tempcode +=
          "\t# For example, to implement a mid point, you would put\n"
          "\t# this code here:\n"
          "\t#\treturn Point( ( arg1.coordinate() + arg2.coordinate() ) / 2 )\n";
      }
    }
    tempcode +=
      "\t# Please refer to the manual for more information.\n"
      "\n";
    return tempcode;
  }

  qDebug() << "No such script type: " << type;
  return QLatin1String("");
}

QString ScriptType::scriptFunctionDefinition( ScriptType::Type type, std::list<ObjectHolder*> args )
{
  if ( type == Python )
  {
    QString newHeader {QStringLiteral( "def calc( " )};
    bool firstarg {true};
    KLocalizedString temparg {ki18nc( "Note to translators: this should be a default "
                                      "name for an argument in a Python function. The "
                                      "default is \"arg%1\" which would become arg1, "
                                      "arg2, etc. Give something which seems "
                                      "appropriate for your language.", "arg%1" )};

    uint id { 1 };
    for ( auto i { args.begin() }; i != args.end(); ++i )
    {
      if ( !firstarg ) newHeader += QLatin1String( ", " );
      else firstarg = false;
      QString n = ( *i )->name();
      newHeader += n.isEmpty() ? temparg.subs( id ).toString() : n;
      id++;
    };
    newHeader += " ):";

    return newHeader;
  }

  qDebug() << "No such script type: " << type;
  return QLatin1String("");
}

void ScriptType::updateCodeFunction( ScriptType::Type type, std::list<ObjectHolder*> args, QString & script )
{
  if ( type == Python )
  {
    QString newHeader { ScriptType::scriptFunctionDefinition( type, args ) };
    size_t newlinePos = script.toStdString().find_first_of( '\n', 0 );

    script.remove( 0, newlinePos );
    script.insert( 0, newHeader );
  }
}

const char* ScriptType::icon( ScriptType::Type type )
{
  return scripts_properties[type].icon;
}

QString ScriptType::highlightStyle( ScriptType::Type type )
{
  return scripts_properties[type].highlightStyle
         ? QString::fromLatin1( scripts_properties[type].highlightStyle )
         : QString();
}

ScriptType::Type ScriptType::intToType( int type )
{
  if ( type == 1 )
    return Python;

  return Unknown;
}
