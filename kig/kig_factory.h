#ifndef KIG_FACTORY_H
#define KIG_FACTORY_H

#include <kparts/factory.h>

class KInstance;
class KAboutData;

class KigPartFactory : public KParts::Factory
{
  Q_OBJECT
public:
  KigPartFactory();
  virtual ~KigPartFactory();
  virtual KParts::Part* createPartObject (QWidget *parentWidget,
					  const char *widgetName,
					  QObject *parent, const char *name,
					  const char *classname,
					  const QStringList &args );
  static KInstance* instance();
  
private:
  static KInstance* s_instance;
  static KAboutData* s_about;
};

#endif
