#include "macrowizard_impl.h"
#include "macrowizard_impl.moc"

#include "kig_part.h"

#include <klineedit.h>

/* 
 *  Constructs a MacroWizardImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The wizard will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal wizard.
 */
MacroWizardImpl::MacroWizardImpl( KigDocument* inDoc)
  : MacroWizard( inDoc->widget(), "Macro Wizard", false, WType_TopLevel | WStyle_DialogBorder | WStyle_Title | WStyle_Dialog ),
    document(inDoc)
{
  setNextEnabled(m_pGivenObjsPage, false);
  setNextEnabled(m_pFinalObjsPage, false);
  setFinishEnabled(m_pNameTypePage, false);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MacroWizardImpl::~MacroWizardImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void MacroWizardImpl::setFinishable(const QString& s)
{
  if (!KLineEdit1->text().isEmpty()) setFinishEnabled( m_pNameTypePage, true );
}
