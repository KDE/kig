#include "kig_factory.h"

#include <kinstance.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "kig_part.h"

KInstance*  KigPartFactory::s_instance = 0L;
KAboutData* KigPartFactory::s_about = 0L;

KigPartFactory::KigPartFactory()
    : KParts::Factory()
{
}

KigPartFactory::~KigPartFactory()
{
  delete s_instance;
  delete s_about;

  s_instance = 0L;
}

KParts::Part* KigPartFactory::createPartObject
( QWidget *parentWidget, const char *widgetName,
  QObject *parent, const char *name,
  const char *classname, const QStringList & /* args */)
{
  // Create an instance of our Part
  KigDocument* obj = new KigDocument( parentWidget, widgetName, parent, name );

  // See if we are to be read-write or not
  if (QCString(classname) == "KParts::ReadOnlyPart")
    obj->setReadWrite(false);

  return obj;
}

KInstance* KigPartFactory::instance()
{
  if( !s_instance )
    {
      // the non-i18n name here must be the same as the directory in
      // which the part's rc file is installed ('partrcdir' in the
      // Makefile)
      s_about = new KAboutData("kigpart", I18N_NOOP("KigDocument"), "2.0", "Kig Document", "(c) 2002, Dominique Devriese", 0, 0, "submit@bugs.kde.org");
      s_about->addAuthor("Dominique Devriese", i18n("Coding"), "fritmebufstek@pandora.be");
      s_about->addCredit("Marc Bartsch", i18n("Author of KGeo, where i got inspiration from, some source, and the artwork", "marc.bartsch@web.de"));
      s_about->addCredit("Ilya Baran", i18n("Author of KSeg, another program that has been a source of inspiration for Kig", "ibaran@mit.edu"));
      s_instance = new KInstance(s_about);
    }
    return s_instance;
}

extern "C"
{
    void* init_libkigpart()
    {
        return new KigPartFactory;
    }
};
