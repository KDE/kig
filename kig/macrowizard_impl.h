#ifndef MACROWIZARDIMPL_H
#define MACROWIZARDIMPL_H
#include "macrowizard.h"

class KigDocument;

class MacroWizardImpl : public MacroWizard
{ 
    Q_OBJECT

protected:
    KigDocument* document;
public:
    MacroWizardImpl( KigDocument* parent);
    ~MacroWizardImpl();
  
public slots:
  void setFinishable(const QString&);
  
  // we reimplement the QDialog reject() to emit the canceled() signal
  virtual void reject() { emit canceled(); MacroWizard::reject(); };
  // we also reimplement the QWizard::next() and back() functions
  virtual void next() { emit stepMacro(); MacroWizard::next(); };
  virtual void back() { emit stepBackMacro(); MacroWizard::back();};
  // we also reimplement QDialog::accept() to emit the stepMacro() signal
  virtual void accept() { emit finishMacro(); MacroWizard::accept(); };

signals:
  void canceled();
  void stepMacro();
  void stepBackMacro();
  void finishMacro();

};

#endif // MACROWIZARDIMPL_H
