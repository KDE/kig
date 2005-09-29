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

#include <kuniqueapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <klibloader.h>
#include <kdebug.h>

#include "aboutdata.h"

static KCmdLineOptions options[] =
  {
    { "c", 0, 0 },
    { "convert-to-native", I18N_NOOP( "Do not show a GUI. Convert the specified file to the native Kig format. Output goes to stdout unless --outfile is specified." ), 0 },
    { "o", 0, 0 },
    { "outfile <file>", I18N_NOOP( "File to output the created native file to. '-' means output to stdout. Default is stdout as well." ), 0 },
    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
  };

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
  if (isRestored() && first)
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

static int convertToNative( const KURL& file, const QCString& outfile )
{
  KigApplication app( false );
  KLibrary* library = KLibLoader::self()->globalLibrary( "libkigpart" );
  int ( *converterfunction )( const KURL&, const QCString& );
  converterfunction = ( int ( * )( const KURL&, const QCString& ) ) library->symbol( "convertToNative" );
  if ( !converterfunction )
  {
    kdError() << "Error: broken Kig installation: different library and application version !" << endl;
    return -1;
  }
  return (*converterfunction)( file, outfile );
}

int main(int argc, char **argv)
{
  KAboutData *about = kigAboutData( "kig", I18N_NOOP("Kig") );

  KCmdLineArgs::init(argc, argv, about);
  KCmdLineArgs::addCmdLineOptions( options );
  KigApplication::addCmdLineOptions();

  KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
  if ( args->isSet( "convert-to-native" ) )
  {
    QCString outfile = args->getOption( "outfile" );
    if ( outfile.isNull() )
      outfile = "-";

    if ( args->count() == 0 )
    {
      kdError() << "Error: --convert-to-native specified without a file to convert." << endl;
      return -1;
    }
    if ( args->count() > 1 )
    {
      kdError() << "Error: --convert-to-native specified with more than one file to convert." << endl;
      return -1;
    }
    return convertToNative( args->url( 0 ), outfile );
  }
  else
  {
    if ( args->isSet( "outfile" ) )
    {
      kdError() << "Error: --outfile specified without convert-to-native." << endl;
      return -1;
    }
    KigApplication app;

    // see if we are starting with session management
    if (app.isRestored()) RESTORE(Kig)
      return app.exec();
  }
}
