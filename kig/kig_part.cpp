#include "kig_part.h"
#include "kig_part.moc"

#include "kig_part.h"

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

// types are global to the application...
Types KigDocument::types;

KigDocument::KigDocument( QWidget *parentWidget, const char *widgetName,
			    QObject *parent, const char *name )
  : KParts::ReadWritePart(parent, name),
    m_pMacroWizard(0),
    numViews(0),
    obc(0)
{
  // we need an instance
  setInstance( KigPartFactory::instance() );

  // we need a widget, to actually show the document
  m_widget = new KigView(this, parentWidget, widgetName);
  // notify the part that this is our internal widget
  setWidget(m_widget);
  // give m_widget control over the status bar.
  connect(m_widget, SIGNAL(setStatusBarText (const QString&)), this, SIGNAL(setStatusBarText(const QString&)));

  // create our actions...
  setupActions();

  // our types...
  setupTypes();

  // set our XML-UI resource file
  setXMLFile("kigpartui.rc");

  // construct our command history
  history = new KCommandHistory(actionCollection());

  // we are read-write by default
  setReadWrite(true);

  setModified (false);

  updateActions();
}

void KigDocument::setupActions()
{
  // we need icons...
  KIconLoader l;
  QPixmap tmp;

  tmp = l.loadIcon( "delete", KIcon::User);
  deleteObjectsAction = new KAction(i18n("Delete objects"), "editdelete", Key_Delete, this, SLOT(deleteSelected()), actionCollection(), "delete_objects");
  deleteObjectsAction->setWhatsThis(i18n("Delete the selected objects"));

  tmp = l.loadIcon( "cancel", KIcon::User);
  cancelConstructionAction = new KAction(i18n("Cancel construction"), tmp, Key_Escape, this, SLOT(delObc()), actionCollection(), "cancel_construction");
  cancelConstructionAction->setWhatsThis(i18n("Cancel the construction of the object being constructed"));
  cancelConstructionAction->setEnabled(false);

  // FIXME: does this somehow belong in the widget ?
  tmp = l.loadIcon("window_fullscreen", KIcon::User);
  startKioskModeAction = new KAction (i18n("Full screen"), tmp, 0, m_widget, SLOT(startKioskMode()), actionCollection(), "full_screen");
  startKioskModeAction->setWhatsThis(i18n("View this document full-screen."));

  tmp = l.loadIcon("macro", KIcon::User);
  newMacroAction = new KAction (i18n("New macro"), tmp, 0, this, SLOT(newMacro()), actionCollection(), "macro_action");
  newMacroAction->setWhatsThis(i18n("Define a new macro"));

  configureTypesAction = new KAction (i18n("Edit Types"), 0, this, SLOT(editTypes()), actionCollection(), "types_edit");
  configureTypesAction->setWhatsThis(i18n("Edit your defined macro types"));


  tmp = l.loadIcon( "line", KIcon::User);
  newLineAction = new KActionMenu (i18n("New Line"), tmp, actionCollection(), "new_line");
  newLineAction->setWhatsThis(i18n("Construct a new line"));

  tmp = l.loadIcon( "circle", KIcon::User );
  newCircleAction = new KActionMenu(i18n("New Circle"), tmp, actionCollection(), "new_circle");
  newCircleAction->setWhatsThis(i18n("Construct a new circle"));

  tmp = l.loadIcon( "point4", KIcon::User );
  newPointAction = new KActionMenu(i18n("New Point"), tmp, actionCollection(), "new_point");
  newPointAction->setWhatsThis(i18n("Construct a new point"));

  tmp = l.loadIcon( "segment", KIcon::User );
  newSegmentAction = new KActionMenu(i18n("New Segment"), tmp, actionCollection(), "new_segment");
  newSegmentAction->setWhatsThis(i18n("Construct a new segment"));

  newOtherAction = new KActionMenu(i18n("New Other"), 0, actionCollection(), "new_other");
  newOtherAction->setWhatsThis(i18n("Construct a new object of a special type"));

};

void KigDocument::setupTypes()
{
  // here we notify ourselves of our predefined types
  // segments
  addType(new TType<Segment>
	  (this,
	   "segment",
	   i18n("Segment"),
	   i18n("Construct a segment by its start and end point"),
	   CTRL+Key_S
	   ));
  // lines
  addType(new TType<LineTTP>
	  (this,
	   "line",
	   i18n("Line by two points"),
	   i18n("Construct a line through two points"),
	   CTRL+Key_L));
  addType(new TType<LinePerpend>
	  (this,
	   "perpendicular",
	   i18n("Perpendicular"),
	   i18n("Construct a line through a point, and perpendicular to "
		"another line or segment"),
	   0));
  addType(new TType<LineParallel>
	  (this,
	   "parallel",
	   i18n("Parallel"),
	   i18n("Construct a line through a point, and parallel to "
		"another line or segment"),
	   0));
  // circles
  addType(new TType<CircleBCP>
	  (this,
	   "circle",
	   i18n("Circle by center and point"),
	   i18n("Construct a circle by its center and a point "
		"on its border"),
	   CTRL+Key_C));
  addType(new TType<CircleBTP>
	  (this,
	   "circle",
	   i18n("Circle by three points"),
	   i18n("Construct a circle through three points"),
	   0));

  // points
  addType(new TType<MidPoint>
	  (this,
	   "bisection",
	   i18n("Midpoint"),
	   i18n("Construct the midpoint of two other points or a segment"),
	   0));
  addType(new TType<IntersectionPoint>
	  (this,
	   "intersection",
	   i18n("Intersection point"),
	   i18n("Construct the point where two lines or segments "
		"intersect"),
	   CTRL+Key_I));

  // and, brand new: locuses !
  addType(new TType<Locus>
	  (this,
	   "locus",
	   i18n("Locus"),
	   i18n("Construct a locus: let one point move around, and record "
		"the places another object goes through.  Those combined "
		"form a new object: the locus..."),
	   0));

  // we also reload the types file we saved the last time, if it
  // exists...
  QStringList relFiles;
  QStringList dataFiles = KGlobal::dirs()->findAllResources("appdata", "kig-types/*.kigt", true, false, relFiles);
  QStringList::iterator dir = dataFiles.begin();
  while (dir != dataFiles.end())
    {
      kdDebug() << k_funcinfo << " : loading types from: " << *dir << endl;
      addTypes(Types(this, *dir));
      ++dir;
    };
};

KigDocument::~KigDocument()
{
  // save our types...
  QString typesDir = KGlobal::dirs()->saveLocation("appdata", "kig-types");
  if (typesDir[typesDir.length()] != '/') typesDir += '/';
  kdDebug() << k_funcinfo << " : saving types to: " << typesDir << endl;
  Types mtypes;
  for (Types::iterator i = types.begin(); i != types.end(); ++i)
    {
      if ((*i)->toMTypeOne()) mtypes.add(*i);
    };
  mtypes.saveToDir(typesDir);
  objects.deleteAll();
  delMacro();
  delObc();
  // C++ defines delete 0 to do nothing
  delete m_pMacroWizard;
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
  Q_ASSERT (!e.isNull());
  ObjectHierarchy hier(e, this);
  Objects tmpOs = hier.fillUp(objects);
  hier.calc();
  _addObjects (tmpOs);
  file.close();

  setModified(false);
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

  // saving is done very easily:
  // we create an ObjectHierarchy with no "given" objects, and all
  // objects as "final" objects...
  Objects given;
  ObjectHierarchy hier( given, objects, this);
  hier.saveXML( doc, elem );
  
  doc.appendChild(elem);

  stream << doc.toCString();

  file.close();

  setModified ( false );
  return true;
}

void KigDocument::addObject(Object* o)
{
  KigCommand* tmp = new AddObjectsCommand(this, o);
  connect( tmp, SIGNAL( executed() ), this, SIGNAL( allChanged() ) );
  connect( tmp, SIGNAL( unexecuted()),this, SIGNAL( allChanged() ) );
  history->addCommand(tmp );
};

void KigDocument::_addObject(Object* o)
{
  Q_ASSERT (o != 0);
  objects.append( o );
  emit repaintOneObject( o );
  setModified(true);
};

void KigDocument::delObject(Object* o)
{
  // we delete all children and their children etc. too...
  Objects all = o->getAllChildren();
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
  movingObjects.remove( o );
  sos.remove(o);
  o->setSelected(false);
  setModified(true);
  emit selectionChanged();
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

void KigDocument::editShowHidden()
{
  for (Object* i = objects.first(); i; i = objects.next())
    i->setShown(true);
};

void KigDocument::updateActions()
{
  if (obc)
    {
      deleteObjectsAction->setEnabled(false);
      newCircleAction->setEnabled(false);
      newPointAction->setEnabled(false);
      newSegmentAction->setEnabled(false);
      newLineAction->setEnabled(false);
      cancelConstructionAction->setEnabled(true);
    }
  else
    {
      if (!sos.isEmpty())
	{
	  deleteObjectsAction->setEnabled(true && isReadWrite());
	}
      else deleteObjectsAction->setEnabled(false);
      newSegmentAction->setEnabled(isReadWrite());
      newCircleAction->setEnabled(isReadWrite());
      newPointAction->setEnabled(isReadWrite());
      newLineAction->setEnabled(isReadWrite());
      cancelConstructionAction->setEnabled(false);
    };
};

/*************************** Macro's ****************************/

void KigDocument::newMacro()
{
  delObc();
  delMacro();
  clearSelection();
  delete m_pMacroWizard;
  m_pMacroWizard = new MacroWizardImpl(this);
  m_pMacroWizard->show();
}

/*************************** Types ****************************/

void KigDocument::editTypes()
{
  KigTypeEditImpl d(this);
  d.exec();
};

void KigDocument::removeType(Type* t)
{
  if (!t->getAction()) return;
  if (t->baseTypeName()==Point::sBaseTypeName()) newPointAction->remove(t->getAction());
  else if (t->baseTypeName() == Line::sBaseTypeName()) newLineAction->remove(t->getAction());
  else if (t->baseTypeName() == Circle::sBaseTypeName()) newCircleAction->remove(t->getAction());
  else if (t->baseTypeName() == Segment::sBaseTypeName()) newSegmentAction->remove(t->getAction());
  else newOtherAction->remove(t->getAction());
  types.remove(t);
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
  appel->setWhatsThis(t->getDescription());
  t->setAction(appel);
//   if (o->getPoint()) newPointAction->insert(appel);
//   else if (o->getLine()) newLineAction->insert(appel);
//   else if (o->getCircle()) newCircleAction->insert(appel);
//   else if (o->getSegment()) newSegmentAction->insert(appel);
//   else kdError() << "new type is not a point, line, circle or segment" << endl;
  if (t->baseTypeName()==Point::sBaseTypeName()) newPointAction->insert(appel);
  else if (t->baseTypeName() == Line::sBaseTypeName()) newLineAction->insert(appel);
  else if (t->baseTypeName() == Circle::sBaseTypeName()) newCircleAction->insert(appel);
  else if (t->baseTypeName() == Segment::sBaseTypeName()) newSegmentAction->insert(appel);
  else newOtherAction->insert(appel);
  types.add(t);
}

/*************************** Constructing objects ****************************/

void KigDocument::newObc(Object* o)
{
  Q_ASSERT (o != 0);
  delMacro();
  delObc();
  obc=o;
  clearSelection();
  updateActions();
};

void KigDocument::delObc()
{
  kdDebug() << k_funcinfo << endl;
  delete obc;
  obc = 0;
  updateActions();
};

/*************************** Selection ****************************/

void KigDocument::unselect(Object* o)
{
  sos.remove(o);
  o->setSelected( false );
  emit repaintOneObject( o );
  emit selectionChanged();
};

void KigDocument::deleteSelected()
{
  if (!sos.isEmpty())
    {
      KigCommand* tmp = new RemoveObjectsCommand(this, sos);
      // TODO: check if it is necessary to redraw everything...
      connect( tmp, SIGNAL(unexecuted()), this, SIGNAL(allChanged()) );
      connect( tmp, SIGNAL(executed()),this, SIGNAL(allChanged()) );
      history->addCommand( tmp );
      emit selectionChanged();
    };
};

void KigDocument::selectObject(Object* o)
{
  if (!o->getSelected())
    {
      sos.append(o);
      o->setSelected(true);
    };
  emit repaintOneObject( o );
  updateActions();
  if (obc)
    {
      // selects an arg for the obc, takes care of the obc if it says it's
      // finished (i.e. addObject(); obc = 0;)
      if (obc->selectArg(o))
	{
	  // this means obc is finished...
	  obc->calc();
	  addObject(obc);
	  clearSelection();
	  obc = 0;
	  updateActions();
	};
      setModified();
    };
  emit selectionChanged();
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
    }
  emit allChanged();
  sos.clear();
  updateActions();
  emit selectionChanged();
};

/*************************** Moving ****************************/

void KigDocument::startMovingSos(const QPoint& p, Objects& still)
{
  for (Object* i = sos.first(); i; i = sos.next())
    i->startMove(p);
  movingObjects.clear();
  still.clear();
  Objects tmp = sos;
  Objects tmp2;
  while (!tmp.isEmpty())
    {
      tmp2.clear();
      // sos and their children/parents and their children's
      // children...: these will be changing
      for (Object* i = tmp.first(); i; i = tmp.next())
	{
	  if (!movingObjects.contains(i))
	    {
	      movingObjects.append(i);
	      tmp2 = i->getChildren();
	      Objects tmp3 = i->getParents();
	      tmp2 |= tmp3;
	    };
	};
      tmp = tmp2;
    };
  // all the rest is still
  for ( Object* i = objects.first(); i; i = objects.next() )
    {
      if (!movingObjects.contains( i ) )
	still.add(i);
    };
  // update our actions...
  updateActions();
};

void KigDocument::moveSosTo(const QPoint& p)
{
  for (Object* i = sos.first(); i; i = sos.next())
    i->moveTo(p);
  movingObjects.calc();
  // recalc, since some stuff should be calc'd after other stuff, and
  // i haven't got the order right yet
  // (i know i should fix this)
  movingObjects.calc();
};

void KigDocument::stopMovingSos()
{
  movingObjects.calc();
  for (Object* i = sos.first(); i; i = sos.next())
    i->stopMove();
  movingObjects.clear();
  setModified(true);
  updateActions();
};

// void KigDocument::cancelMovingSos()
// {
//   for (Object* i = sos.first(); i; i++)
//     (i)->cancelMove();
//   emit weChanged();
// };

Object* KigDocument::newObject(const QCString& type)
{
  Object* t = types.newObject(type);
  if (t) return t;
  // these are points which we don't add to the types variable because
  // the user can't construct them in the normal way...
  else if (type == "ConstrainedPoint") return new ConstrainedPoint;
  else if (type == "Point") return new Point;
  return 0;
};

void KigDocument::selectObjects(const Objects& os)
{
  Object* i;
  for (Objects::iterator it(os); (i = it.current()); ++it)
    {
      selectObject(i);
    };
}
void KigDocument::delMacro()
{
  delete m_pMacroWizard; m_pMacroWizard = 0;
}
