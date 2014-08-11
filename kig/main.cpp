/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "kig.h"

#include <qbytearray.h>

#include <kuniqueapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <klibloader.h>
#include <QDebug>

#include "aboutdata.h"

class KigApplication
  : public KUniqueApplication
{
public:
  KigApplication( bool gui = true );
  int newInstance();
  void handleArgs( KCmdLineArgs* args );
};

KigApplication::KigApplication( bool gui )
  : KUniqueApplication( gui, gui )
{
}

int KigApplication::newInstance()
{
  static bool first = true;
  if (isSessionRestored() && first)
  {
    first = false;
    return 0;
  }
  first = false;

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  handleArgs(args);
  args->clear();
  return 0;
}

void KigApplication::handleArgs( KCmdLineArgs* args )
{
  if ( args->count() == 0 )
  {
    Kig *widget = new Kig;
    widget->show();
  }
  else
  {
    for (int i = 0; i < args->count(); i++ )
    {
      Kig *widget = new Kig;
      widget->show();
      widget->load( args->url( i ) );
    }
  }
}

static int convertToNative( const QUrl &file, const QByteArray& outfile )
{
  KComponentData maindata( KCmdLineArgs::aboutData() );
  KPluginLoader libraryLoader( "kigpart" );
  QLibrary library( libraryLoader.fileName() );
  int ( *converterfunction )( const KUrl&, const QByteArray& );
  converterfunction = ( int ( * )( const KUrl&, const QByteArray& ) ) library.resolve( "convertToNative" );
  if ( !converterfunction )
  {
    qCritical() << "Error: broken Kig installation: different library and application version !" << endl;
    return -1;
  }
  return (*converterfunction)( file, outfile );
}

int main(int argc, char **argv)
{
  KAboutData about = kigAboutData( "kig", I18N_NOOP("Kig") );

  KCmdLineArgs::init(argc, argv, &about);

  KCmdLineOptions options;
  options.add("c");
  options.add("convert-to-native", ki18n( "Do not show a GUI. Convert the specified file to the native Kig format. Output goes to stdout unless --outfile is specified." ));
  options.add("o");
  options.add("outfile <file>", ki18n( "File to output the created native file to. '-' means output to stdout. Default is stdout as well." ));
  options.add("+[URL]", ki18n( "Document to open" ));
  KCmdLineArgs::addCmdLineOptions( options );
  KigApplication::addCmdLineOptions();

  KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
  if ( args->isSet( "convert-to-native" ) )
  {
    QString outfile = args->getOption( "outfile" );
    if ( outfile.isEmpty() )
      outfile = "-";

    if ( args->count() == 0 )
    {
      qCritical() << "Error: --convert-to-native specified without a file to convert." << endl;
      return -1;
    }
    if ( args->count() > 1 )
    {
      qCritical() << "Error: --convert-to-native specified with more than one file to convert." << endl;
      return -1;
    }
    return convertToNative( args->url( 0 ), outfile.toLocal8Bit() );
  }
  else
  {
    if ( args->isSet( "outfile" ) )
    {
      qCritical() << "Error: --outfile specified without convert-to-native." << endl;
      return -1;
    }
    KigApplication app;

    // see if we are starting with session management
    if (app.isSessionRestored()) RESTORE(Kig)
      return app.exec();
  }
}
