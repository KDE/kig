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
  : MacroWizard( inDoc->widget(), "Macro Wizard", false, WType_TopLevel | WStyle_DialogBorder | WStyle_Title | WStyle_Dialog | WDestructiveClose ),
    document(inDoc)
{
  setNextEnabled(m_pGivenObjsPage, false);
  setNextEnabled(m_pFinalObjsPage, false);
  setFinishEnabled(m_pNameTypePage, false);
  connect (document, SIGNAL (selectionChanged()), this, SLOT(updateNexts()));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MacroWizardImpl::~MacroWizardImpl()
{
    // no need to delete child widgets, Qt does it all for us
}
void MacroWizardImpl::accept()
{
  // finish the macro
  // get the name and desc:
  QString name = KLineEdit1->text();
  QString description = DescriptionEdit->text();
  // construct a hierarchy
  ObjectHierarchy* tmpH =
    new ObjectHierarchy( givenObjs, finalObjs, document );

#undef FINISH_MACRO_SHOW_HIER
#ifdef FINISH_MACRO_SHOW_HIER
  // show the hierarchy on cerr for debugging...
  QDomDocument doc("KigDocument");
  QDomElement elem = doc.createElement( "KigDocument" );
  elem.setAttribute( "Version", "2.0.000" );
  tmpH->saveXML(doc, elem);
  doc.appendChild(elem);
  kdDebug() << k_funcinfo << " at line no. " << __LINE__ << endl;
  kdDebug() << doc.toCString() << endl;
#endif // FINISH_MACRO_SHOW_HIER

  // create the new Type
  Type* tmp = new MTypeOne(tmpH, name, description, document);
  // install it...
  document->addType(tmp);

  MacroWizard::accept();
}

void MacroWizardImpl::next()
{
  if (currentPage() == m_pGivenObjsPage)
    {
      // save the given objects...
      givenObjs = document->getSos();
      // and show the final objects...
      document->clearSelection();
      document->selectObjects(finalObjs);
    }
  else
    {
      // this means currentPage() == m_pFinalObjsPage...
      // save the final objects...
      finalObjs = document->getSos();
      // and clear the selection...
      document->clearSelection();
    };
  // let the default implementation change the page...
  MacroWizard::next();
}

void MacroWizardImpl::back()
{
  if (currentPage() == m_pFinalObjsPage)
    {
      // save the final objects...
      finalObjs = document->getSos();
      // and show the final objects...
      document->clearSelection();
      document->selectObjects(givenObjs);
    }
  else
    {
      // this means currentPage() == m_pNameTypePage...
      // select the final objects (there should be nothing selected
      // now...
      document->selectObjects(finalObjs);
    };
  // let the default implementation change the page...
  MacroWizard::back();
}

void MacroWizardImpl::reject()
{
}

void MacroWizardImpl::updateNexts()
{
  if (currentPage() == m_pGivenObjsPage)
    setNextEnabled(m_pGivenObjsPage,!document->getSos().isEmpty());
  else
    setNextEnabled(m_pGivenObjsPage,!givenObjs.isEmpty());
  if (currentPage() == m_pFinalObjsPage)
    setNextEnabled(m_pFinalObjsPage,!document->getSos().isEmpty());
  else setNextEnabled(m_pFinalObjsPage,!finalObjs.isEmpty());
  setFinishEnabled( m_pNameTypePage, !KLineEdit1->text().isEmpty() );
};
