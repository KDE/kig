#ifndef MACROWIZARDIMPL_H
#define MACROWIZARDIMPL_H
#include "macrowizard.h"

#include "../misc/objects.h"

#include <klineedit.h>
class KigDocument;

class MacroWizardImpl : public MacroWizard
{ 
  Q_OBJECT

protected:
  KigDocument* document;

public:
  MacroWizardImpl( KigDocument* parent);
  ~MacroWizardImpl();

  Objects givenObjs;
  Objects finalObjs;
  
public slots:
  void setFinishable(const QString&) { updateNexts(); };

  /** 
   * en-/disable the next and finish buttons...
   * 
   */
  void updateNexts();

  virtual void reject();
  virtual void next();
  virtual void back();
  virtual void accept();

};

#endif // MACROWIZARDIMPL_H
