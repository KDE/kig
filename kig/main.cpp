/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#include "kig.h"

#include <kuniqueapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include "aboutdata.h"

static KCmdLineOptions options[] =
  {
    { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
    { 0, 0, 0 }
  };

class KigApplication
  : public KUniqueApplication
{
public:
  int newInstance()
  {
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    handleArgs(args);
    args->clear();
    return 0;
  };
  void handleArgs(KCmdLineArgs* args)
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
  };
};

int main(int argc, char **argv)
{
  KAboutData* about = kigAboutData( "kig", I18N_NOOP("Kig") );

  KCmdLineArgs::init(argc, argv, about);
  KCmdLineArgs::addCmdLineOptions( options );
  KigApplication::addCmdLineOptions();
  KigApplication app;

  // see if we are starting with session management
  if (app.isRestored()) RESTORE(Kig)
  return app.exec();
}
