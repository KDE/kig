#include "kig.h"

#include <kapp.h>
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

int main(int argc, char **argv)
{
  KAboutData about("kig", I18N_NOOP("Kig"), version, description, KAboutData::License_GPL, "(C) 2002, Dominique Devriese");
  about.addAuthor("Dominique Devriese", I18N_NOOP("Coding"), "fritmebufstek@pandora.be" );
  about.addCredit("Marc Bartsch", I18N_NOOP("Author of KGeo, where i got inspiration, some source, and most of the artwork from"), "marc.bartsch@web.de");
  about.addCredit("Ilya Baran", I18N_NOOP("Author of KSeg, another program that has been a source of inspiration for Kig"), "ibaran@mit.edu");
  about.addCredit("Cabri coders", I18N_NOOP("Cabri is a commercial program like Kig, and gave me something to compete against."), "www-cabri.imag.fr");

  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions( options );
  KApplication app;

  // see if we are starting with session management
  if (app.isRestored())
    RESTORE(Kig)
  else
    {
      // no session.. just start up normally
      KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

      if ( args->count() == 0 )
	{
	  Kig *widget = new Kig;
	  widget->show();
	}
      else
	{
	  int i = 0;
	  for (; i < args->count(); i++ )
	    {
	      Kig *widget = new Kig;
	      widget->show();
	      widget->load( args->url( i ) );
	    }
	}
      args->clear();
    }

  return app.exec();
}
