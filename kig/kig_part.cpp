#include "kig_part.h"
#include "kig_part.moc"

#include "kig_factory.h"
#include "kig_view.h"
#include "kig_commands.h"
#include "type_edit_impl.h"
#include "macrowizard_impl.h"
#include "../objects/circle.h"
#include "../objects/segment.h"
#include "../objects/point.h"
#include "../objects/line.h"
#include "../objects/macro.h"
#include "../objects/intersection.h"
#include "../objects/locus.h"
#include "../misc/hierarchy.h"

#include <kinstance.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kaboutdata.h>
#include <kfiledialog.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <klineeditdlg.h>
#include <kglobal.h>
#include <klineedit.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qpen.h>

KigDocument::KigDocument( QWidget *parentWidget, const char *widgetName,
			    QObject *parent, const char *name )
  : KParts::ReadWritePart(parent, name),
    obc(0),
    _changed(false),
    macroWAW(macroDN),
    m_pMacroWizard(0),
    numViews(0)
{
  // we need an instance
  setInstance( KigPartFactory::instance() );

  // we need a widget, to actually show the document
  m_widget = new KigView(this, parentWidget, widgetName);

  // give m_widget control of the status bar.
  connect(m_widget, SIGNAL(setStatusBarText (const QString&)), this, SIGNAL(setStatusBarText(const QString&)));

  // notify the part that this is our internal widget
  setWidget(m_widget);

  // construct our command history
  history = new KCommandHistory(actionCollection());
  connect(history, SIGNAL (commandExecuted()), this, SLOT(updateActions()));

  // create our actions
  KAFileOpen = KStdAction::open(this, SLOT(fileOpen()), actionCollection());
  KAFileSaveAs = KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
  KAFileSave = KStdAction::save(this, SLOT(save()), actionCollection());

  KIconLoader l;
  QPixmap tmp;
  tmp = l.loadIcon( "delete", KIcon::User);
  KADeleteObjects = new KAction(i18n("Delete objects"), "editdelete", Key_Delete, this, SLOT(deleteSelected()), actionCollection(), "delete_objects");
  tmp = l.loadIcon( "cancel", KIcon::User);
  KACancelConstruction = new KAction(i18n("Cancel construction"), tmp, Key_Escape, this, SLOT(delObc()), actionCollection(), "cancel_construction");
  KACancelConstruction->setEnabled(false);

  tmp = l.loadIcon("window_fullscreen", KIcon::User);
  KAStartKioskMode = new KAction (i18n("Full screen"), tmp, 0, m_widget, SLOT(startKioskMode()), actionCollection(), "full_screen");

  tmp = l.loadIcon("macro", KIcon::User);
  KANewMacro = new KAction (i18n("New macro"), tmp, 0, this, SLOT(newMacro()), actionCollection(), "macro_action");

  KAConfigureTypes = new KAction (i18n("Edit Types"), 0, this, SLOT(editTypes()), actionCollection(), "types_edit");

  // first we create our KMenuActions
  tmp = l.loadIcon( "line", KIcon::User);
  KANewLine = new KActionMenu (i18n("New Line"), tmp, actionCollection(), "new_line");
  tmp = l.loadIcon( "circle", KIcon::User );
  KANewCircle = new KActionMenu(i18n("New Circle"), tmp, actionCollection(), "new_circle");
  tmp = l.loadIcon( "point4", KIcon::User );
  KANewPoint = new KActionMenu(i18n("New Point"), tmp, actionCollection(), "new_point");
  tmp = l.loadIcon( "segment", KIcon::User );
  KANewSegment = new KActionMenu(i18n("New Segment"), tmp, actionCollection(), "new_segment");
  KANewOther = new KActionMenu(i18n("New Other"), 0, actionCollection(), "new_other");

  // next we add our predefined types:
  // segments
  addType(new TType<Segment>
	  (this,
	   "segment",
	   i18n("Segment"),
	   CTRL+Key_S));
  // lines
  addType(new TType<LineTTP>
	  (this,
	   "line",
	   i18n("Line by two points"),
	   CTRL+Key_L));
  addType(new TType<LinePerpend>
	  (this,
	   "perpendicular",
	   i18n("Perpendicular"),
	   0));
  addType(new TType<LineParallel>
	  (this,
	   "parallel",
	   i18n("Parallel"),
	   0));
  // circles
  addType(new TType<CircleBCP>
	  (this,
	   "circle",
	   i18n("Circle by center and point"),
	   CTRL+Key_C));
  addType(new TType<CircleBTP>
	  (this,
	   "circle",
	   i18n("Circle by three points"),
	   0));

  // points
  addType(new TType<MidPoint>
	  (this,
	   "bisection",
	   i18n("Midpoint"),
	   0));
  addType(new TType<IntersectionPoint>
	  (this,
	   "intersection",
	   i18n("Intersection point"),
	   CTRL+Key_I));
  // and, brand new: locuses !
  addType(new TType<Locus>
	  (this,
	   "locus",
	   i18n("Locus"),
	   0));

  // we reload the types file we saved the last time, if it exists...
  QString typesFile = KGlobal::dirs()->findResource("config", "KigPart/types.kigt");
  if (typesFile) {
    kdDebug() << k_funcinfo << " : loading types from: " << typesFile << endl;
    addTypes(Types(this, typesFile));
  };

  // set our XML-UI resource file
  setXMLFile("kig_part.rc");

  // we are read-write by default
  setReadWrite(true);

  // we are not modified since we haven't done anything yet
  setModified(false);

  updateActions();
}

KigDocument::~KigDocument()
{
  QString typesFile = KGlobal::instance()->dirs()->saveLocation("config", "KigPart");
  typesFile += "/types.kigt";
  kdDebug() << k_funcinfo << " : saving types to: " << typesFile << endl;
  Types mtypes;
  for (Types::iterator i = types.begin(); i != types.end(); ++i)
    {
      if ((*i)->toMTypeOne()) mtypes.add(*i);
    };
  mtypes.saveToFile(typesFile);
  objects.deleteAll();
  delMacro();
  delObc();
}

void KigDocument::setReadWrite(bool rw)
{
  readOnly = !rw;
  if (readOnly)
    {
      delObc();
      clearSelection();
    }
  else {};
  // notify your internal widget of the read-write state
  m_widget->setReadOnly(!rw);
  if (rw)
    connect(m_widget, SIGNAL(changed()),
    	    this,     SLOT(setModified()));
  else
    {
      disconnect(m_widget, SIGNAL(changed()),
		 this,     SLOT(setModified()));
    }
  ReadWritePart::setReadWrite(rw);
}

void KigDocument::setModified(bool modified)
{
  // if so, we either enable or disable it based on the current
  // state
  if (modified) KAFileSave->setEnabled(true);
  else KAFileSave->setEnabled(false);

  // in any event, we want our parent to do it's thing
  ReadWritePart::setModified(modified);
}

bool KigDocument::openFile()
{
  // m_file is always local so we can use QFile on it
  QFile file(m_file);
  if (!file.open(IO_ReadOnly)) return false;

  objects.deleteAll();
  // TODO: more error detection
  QDomDocument doc ("KigDocument");
  doc.setContent(&file);
  QDomElement main = doc.documentElement();
  if ( main.attribute("Version") != "2.0.000")
    {
      KMessageBox::detailedSorry(m_widget, "Unsupported KigDocument version", QString("You tried to open a KigDocument with version number %1").arg(main.attribute("Version")));
      return false;
    };
  QDomNode n;
  for ( n = main.firstChild(); !n.isNull(); n=n.nextSibling())
    { 
      QDomElement e = n.toElement();
      assert (!e.isNull());
      if (e.tagName() == "ObjectHierarchy") break;
      // TODO: make this more generic somehow, error detection
      if (e.tagName() == "Point") _addObject(new Point(e.attribute("x").toInt(), e.attribute("y").toInt()));
    };
  QDomElement e = n.toElement();
  assert (!e.isNull());
  ObjectHierarchy hier(e, this);
  Objects tmpOs = hier.fillUp(objects);
  tmpOs.calc();
  _addObjects (tmpOs);
  file.close();

  emit allChanged();
  return true;
}

bool KigDocument::saveFile()
{
  // if we aren't read-write, return immediately
  if (!isReadWrite())
    return false;

  // m_file is always local, so we use QFile
  QFile file(m_file);
  if (file.open(IO_WriteOnly) == false)
    return false;

  QTextStream stream(&file);

  QDomDocument doc("KigDocument");
  QDomElement elem = doc.createElement( "KigDocument" );
  elem.setAttribute( "Version", "2.0.000" );

  // saving is done like this:
  // we put the data of all independent objects in the xml file
  // then we create a ObjectHierarchy of all other objects, which we
  // save to the xml-file, and next we close up.
  Objects gegObjs, finObjs;
  for (Object* i = objects.first(); i; i = objects.next())
    {
      if (i->getParents().isEmpty())
	{
	  i->saveXML( doc, elem );
	  gegObjs.append(i);
	}
      else finObjs.append(i);
    };
  ObjectHierarchy hier(gegObjs, finObjs, this);
  hier.saveXML( doc, elem );
  
  doc.appendChild(elem);

  stream << doc.toCString();

  file.close();

  setModified ( false );
  return true;
}

void KigDocument::fileOpen()
{
  // this slot is called whenever the File->Open menu is selected,
  // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
  // button is clicked
  QString file_name = KFileDialog::getOpenFileName(":document", "*.kig");

  if (!file_name.isEmpty()) openURL(file_name);
}

void KigDocument::fileSaveAs()
{
  // this slot is called whenever the File->Save As menu is selected,
  QString file_name = KFileDialog::getSaveFileName(":document", "*.kig\n*");
  if (!file_name.isEmpty()) saveAs(file_name);
}

// KPopupMenu* KigDocument::getPopup (const QCString& ot)
// {
//   return static_cast<KPopupMenu*>(hostContainer("popup_menu_for_a_" + ot));
// };

void KigDocument::addObject(Object* o)
{
  KigCommand* tmp = new AddObjectsCommand(this, o);
  connect( tmp, SIGNAL( executed() ), this, SIGNAL( allChanged() ) );
  connect( tmp, SIGNAL( unexecuted()),this, SIGNAL( allChanged() ) );
  history->addCommand(tmp );
};

void KigDocument::_addObject(Object* o)
{
  objects.append( o );
  emit objectAdded( o );
  emit changed();
};

void KigDocument::delObject(Object* o)
{
  Objects all = o->getChildren();
  all.add(o);
  KigCommand* tmp = new RemoveObjectsCommand(this, all);
  connect( tmp, SIGNAL( executed() ), this, SIGNAL( allChanged() ) );
  connect( tmp, SIGNAL( unexecuted() ), this, SIGNAL( allChanged() ) );
  history->addCommand(tmp );
};

void KigDocument::_delObject(Object* o)
{
  objects.remove( o );
  delObc();
  cos.remove( o );
  stos.remove( o );
  sos.remove(o);
  o->setSelected(false);
  emit changed();
};

void KigDocument::obcSelectArg(Object* o)
{
  selectObject( o );
  if (obc->selectArg(o))
    {
      obc->calc();
      addObject(obc);
      clearSelection();
      obc = 0;
      emit modeChanged();
      updateActions();
    };
  emit changed();
};

void KigDocument::unselect(Object* o)
{
  sos.remove(o);
  o->setSelected( false );
  emit selectionChanged( o );
  emit changed();
};

void KigDocument::deleteSelected()
{
  if (!sos.isEmpty())
    {
      KigCommand* tmp = new RemoveObjectsCommand(this, sos);
      // TODO: check if it is necessary to redraw everything...
      connect( tmp, SIGNAL( unexecuted() ), this, SIGNAL( allChanged() ) );
      connect( tmp, SIGNAL( executed() ),this, SIGNAL( allChanged() ) );
      history->addCommand( tmp );
    };
};

Objects KigDocument::whatAmIOn(const QPoint& p)
{
  Objects tmp;
  for ( Object* i = objects.first(); i; i = objects.next())
    {
      if(!i->contains(p, false)) continue;
      // points go in front of the list
      if (Object::toPoint(i)) tmp.prepend(i);
      else tmp.append(i);
    };
  return tmp;
}

void KigDocument::selectObject(Object* o)
{
  if (!o->getSelected())
    {
      sos.append(o);
      o->setSelected(true);
    };
  emit selectionChanged( o );
  updateActions();
}

void KigDocument::selectObjects(const QRect & r)
{
  for (Object* i = objects.first(); i; i = objects.next())
    if(i->getShown() && i->inRect(r))
      selectObject(i);
}

void KigDocument::clearSelection()
{
  for (Object* i = objects.first(); i; i = objects.next())
    {
      i->setSelected(false);
      emit selectionChanged(i);
    }
  sos.clear();
  updateActions();
};

void KigDocument::startMovingSos(const QPoint& p)
{
//   kdDebug() << k_funcinfo << " at line no. " << __LINE__ << endl;
  for (Object* i = sos.first(); i; i = sos.next())
    i->startMove(p);
  cos.clear();
  stos.clear();
  Objects tmp = sos;
  Objects tmp2;
  while (!tmp.isEmpty())
    {
      tmp2.clear();
      // sos and their children/parents and their children's
      // children...: these will be changing
      for (Object* i = tmp.first(); i; i = tmp.next())
	{
	  if (!cos.contains(i))
	    {
	      cos.append(i);
	      tmp2 = i->getChildren();
	      Objects ___tmp = i->getParents();
	      tmp2 |= ___tmp;
	    };
	};
      tmp = tmp2;
    };
  // all the rest is still
  for ( Object* i = objects.first(); i; i = objects.next() )
    {
      if (!cos.contains( i ) )
	stos.add(i);
    };
  // inform others that we started moving
  emit startedMoving();
  emit modeChanged();
  emit changed();
  updateActions();
};

void KigDocument::moveSosTo(const QPoint& p)
{
  for (Object* i = sos.first(); i; i = sos.next())
    i->moveTo(p);
  cos.calc();
  // recalc, since some stuff should be calc'd after other stuff, and i haven't got the order right yet
  cos.calc();
  emit cosMoved();
  emit changed();
};

void KigDocument::stopMovingSos()
{
  cos.calc();
  for (Object* i = sos.first(); i; i = sos.next())
    i->stopMove();
  cos.clear();
  stos.clear();
  emit stoppedMoving();
  emit modeChanged();
  emit changed();
  updateActions();
};

// void KigDocument::cancelMovingSos()
// {
//   for (Object* i = sos.first(); i; i++)
//     (i)->cancelMove();
//   emit weChanged();
// };

void KigDocument::editShowHidden()
{
  for (Object* i = objects.first(); i; i = objects.next())
    i->setShown(true);
};

void KigDocument::newObc(Object* o)
{
  delMacro();
  delObc();
  obc=o;
  clearSelection();
  emit modeChanged();
  updateActions();
};

void KigDocument::delObc()
{
  delete obc;
  obc = 0;
  emit modeChanged();
  updateActions();
};

void KigDocument::updateActions()
{
  if (obc)
    {
      KADeleteObjects->setEnabled(false);
      KANewCircle->setEnabled(false);
      KANewPoint->setEnabled(false);
      KANewSegment->setEnabled(false);
      KANewLine->setEnabled(false);
      KACancelConstruction->setEnabled(true);
    }
  else
    {
      if (!sos.isEmpty())
	{
	  KADeleteObjects->setEnabled(true && !readOnly);
	}
      else KADeleteObjects->setEnabled(false);
      KANewSegment->setEnabled(true && !readOnly);
      KANewCircle->setEnabled(true && !readOnly);
      KANewPoint->setEnabled(true);
      KANewLine->setEnabled(true);
      KACancelConstruction->setEnabled(false);
    };
}

void KigDocument::addType(Type* t)
{
  QPixmap tmp;
  KIconLoader l;
  tmp = l.loadIcon( t->getPicName(), KIcon::User );
  KAction* appel = new KAction( t->getActionName(),
				tmp,
				t->getShortCutKey(),
				t->getSlotWrapper(),
				SLOT( newObc() ),
				actionCollection(),
				t->getActionName()
				);
  t->setAction(appel);
//   if (o->getPoint()) KANewPoint->insert(appel);
//   else if (o->getLine()) KANewLine->insert(appel);
//   else if (o->getCircle()) KANewCircle->insert(appel);
//   else if (o->getSegment()) KANewSegment->insert(appel);
//   else kdError() << "new type is not a point, line, circle or segment" << endl;
  if (t->baseTypeName()==Point::sBaseTypeName()) KANewPoint->insert(appel);
  else if (t->baseTypeName() == Line::sBaseTypeName()) KANewLine->insert(appel);
  else if (t->baseTypeName() == Circle::sBaseTypeName()) KANewCircle->insert(appel);
  else if (t->baseTypeName() == Segment::sBaseTypeName()) KANewSegment->insert(appel);
  else KANewOther->insert(appel);
  types.add(t);
}

void KigDocument::newMacro()
{
  delObc();
  delMacro();
  clearSelection();
  delete m_pMacroWizard;
  m_pMacroWizard = new MacroWizardImpl(this);
  connect(m_pMacroWizard, SIGNAL(canceled()), this, SLOT(delMacro()));
  connect(m_pMacroWizard, SIGNAL(stepMacro()), this, SLOT(stepMacro()));
  connect(m_pMacroWizard, SIGNAL(stepBackMacro()), this, SLOT(stepBackMacro()));
  connect(m_pMacroWizard, SIGNAL(finishMacro()), this, SLOT(finishMacro()));
  m_pMacroWizard->show();
  macroWAW = macroSGO;
  emit modeChanged();  
}

void KigDocument::finishMacro()
{
  // finish the macro
  // get the name: 
  QString appel = m_pMacroWizard->KLineEdit1->text();
  ObjectHierarchy* tmpH = new ObjectHierarchy(macroGegObjs, macroFinObjs, this);
  // // show the hierarchy on cerr for debugging...
  //       QDomDocument doc("KigDocument");
  //       QDomElement elem = doc.createElement( "KigDocument" );
  //       elem.setAttribute( "Version", "2.0.000" );
  //       tmpH->saveXML(doc, elem);
  //       doc.appendChild(elem);
  //       kdDebug() << k_funcinfo << " at line no. " << __LINE__ << endl;
  //       kdDebug() << doc.toCString() << endl;

  // create the new Type
  Type* tmp = new MTypeOne(tmpH, appel, this);
  // install it...
  addType(tmp);
  // clear what we've done
  delMacro();
  emit modeChanged();
};

void KigDocument::stepMacro()
{
  assert(macroWAW != macroDN && macroWAW != macroSN );
  Objects* curObjs, *oldObjs;
  switch (macroWAW)
    {
    case macroSFO:
      macroWAW = macroSN;
      curObjs = 0;
      oldObjs = &macroFinObjs;
      break;
    case macroSGO:
      macroWAW = macroSFO;
      curObjs = &macroFinObjs;
      oldObjs = &macroGegObjs;
      break;
    default:
      curObjs = 0;
      oldObjs = 0;
      break;
    };
  // update the selected state of all objects...
  if (oldObjs)
    for (Object* i = oldObjs->first(); i; i = oldObjs->next())
      {
	i->setSelected(false);
	emit selectionChanged(i);
      };
  if (curObjs)
    for (Object* i = curObjs->first(); i; i = curObjs->next())
      {
	i->setSelected(true);
	emit selectionChanged(i);
      };
}

void KigDocument::delMacro()
{
  macroGegObjs.clear();
  macroFinObjs.clear();
  macroWAW = macroDN;
}

void KigDocument::macroSelect(Object* o)
{
  assert(macroWAW == macroSGO || macroWAW == macroSFO);
  Objects* a = (macroWAW == macroSFO ? &macroFinObjs : &macroGegObjs);
  assert(o);
  if (o->getSelected())
    {
      a->remove(o);
      o->setSelected(false);
      emit selectionChanged(o);
    }
  else
    {
      a->append(o);
      o->setSelected(true);
      emit selectionChanged(o);
    };
  m_pMacroWizard->setNextEnabled(m_pMacroWizard->m_pGivenObjsPage, !macroGegObjs.isEmpty());
  m_pMacroWizard->setNextEnabled(m_pMacroWizard->m_pFinalObjsPage, !macroFinObjs.isEmpty());
}

void KigDocument::macroSelect( Objects& os)
{
  for (Object* i = os.first(); i; i = os.next())
    macroSelect(i);
}
void KigDocument::macroSelect(const QRect& r)
{
  for (Object* i = objects.first(); i; i = objects.next())
    if(i->getShown() && i->inRect(r))
      macroSelect(i);
}

void KigDocument::editTypes()
{
  KigTypeEditImpl d(this);
  d.exec();
};

void KigDocument::removeType(Type* t)
{
  if (!t->getAction()) return;
  if (t->baseTypeName()==Point::sBaseTypeName()) KANewPoint->remove(t->getAction());
  else if (t->baseTypeName() == Line::sBaseTypeName()) KANewLine->remove(t->getAction());
  else if (t->baseTypeName() == Circle::sBaseTypeName()) KANewCircle->remove(t->getAction());
  else if (t->baseTypeName() == Segment::sBaseTypeName()) KANewSegment->remove(t->getAction());
  else kdError() << "type being deleted is not a point, line, circle or segment..." << endl;
  types.remove(t);
}

void KigDocument::addTypes(const Types& ts)
{
  for (Types::const_iterator i = ts.begin(); i != ts.end(); ++i)
    addType(*i);
}

void KigDocument::stepBackMacro()
{
  Objects* curObjs;
  Objects* oldObjs;
  assert (macroWAW == macroSN || macroWAW == macroSFO);
  switch (macroWAW)
    {
    case macroSN:
      macroWAW = macroSFO;
      curObjs = &macroFinObjs;
      oldObjs = 0;
      break;
    case macroSFO:
      macroWAW = macroSGO;
      curObjs = &macroGegObjs;
      oldObjs = &macroFinObjs;
      break;
    default:
      curObjs = 0;
      oldObjs = 0;
      break;
    };
  // update the selected state of all objects...
  if (oldObjs)
    for (Object* i = oldObjs->first(); i; i = oldObjs->next())
      {
	i->setSelected(false);
	emit selectionChanged(i);
      };
  if (curObjs)
    for (Object* i = curObjs->first(); i; i = curObjs->next())
      {
	i->setSelected(true);
	emit selectionChanged(i);
      };
}
