/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese
 
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

static const char *description = I18N_NOOP("KDE Interactive Geometry");

static const char *version = "v2.0";

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
  KAboutData about("kig", I18N_NOOP("Kig"), version,
		   description, KAboutData::License_GPL,
		   "(C) 2002, Dominique Devriese");

  about.addAuthor("Dominique Devriese", I18N_NOOP("Coding"),
		  "fritmebufstek@pandora.be" );

  about.addCredit("Marc Bartsch",
		  I18N_NOOP("Author of KGeo, where i got inspiration, "
			    "some source, and most of the artwork from"),
		  "marc.bartsch@web.de");

  about.addCredit("Ilya Baran",
		  I18N_NOOP("Author of KSeg, another program that has been a "
			    "source of inspiration for Kig"),
		  "ibaran@mit.edu");

  about.addCredit("Maurizio Paolini",
		 I18N_NOOP("Wrote the radical line as a patch for kgeo, which "
			   "i ported to kig."),
		 "paolini@dmf.bs.unicatt.it");

  about.addCredit("Cabri coders",
		  I18N_NOOP("Cabri is a commercial program like Kig, and "
			    "gave me something to compete against :)"),
		  "www-cabri.imag.fr");

  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions( options );
  KigApplication::addCmdLineOptions();
  KigApplication app;

  // see if we are starting with session management
  if (app.isRestored()) RESTORE(Kig)
  return app.exec();
}
