#ifndef KIGPART_H
#define KIGPART_H

#include <kparts/part.h>
#include <kparts/factory.h>
#include <kcommand.h>

#include "../objects/object.h"
#include "../misc/types.h"
#include "../misc/type.h"

class MacroWizardImpl;
class QWidget;
class QPainter;
class KURL;
class KPopupMenu;
class KigView;
class KActionMenu;

/**
 * This is a "Part".  It that does all the real work in a KPart
 * application.
 * actually, it only holds a view and a document, and acts as an
 * interface to shells
 *
 * @short Main Part
 * @author Dominique Devriese <fritmebufstek@pandora.be>
 */
class KigDocument : public KParts::ReadWritePart
{
  Q_OBJECT

  friend class AddObjectsCommand;
  friend class RemoveObjectsCommand;
public:
  /**
   * Default constructor
   */
  KigDocument(QWidget *parentWidget, const char *widgetName,
	      QObject *parent = 0, const char *name = 0);


  /**
   * Destructor
   */
  virtual ~KigDocument();


/*********************** KPart interface *************************/

protected:
  /**
   * load our internal document from m_file
   */
  virtual bool openFile();

  /**
   * save our internal document to m_file
   */
  virtual bool saveFile();

signals:
  // we control the app's statusbar...
  void setStatusBarText ( const QString & text );


  /****************** communication with KigView **************/
public:
  // if a new view is added
  void addView(KigView*) { numViews++; };
  void delView(KigView*) { numViews--; };

  const Objects& getSos() { return sos;};
  const Objects& getMovingObjects() { return movingObjects; };
  const Objects& getObjects() { return objects;};

  Object* getObc() const { return obc; };

  // what objects are under point p
  Objects whatAmIOn(const QPoint& p);
  Objects whatAmIOn(int x, int y) { return whatAmIOn(QPoint(x,y));};

  // KigView calls this to see if it's allowed to let the user move
  // stuff around.  We decide based on whether we're constructing a
  // macro (-> not allowed), or whether we are readonly (-> not
  // allowed) etc.
  bool canMoveObjects() { if ( obc || m_pMacroWizard ) return false; else return true;};
  bool canSelectObject(Object* o) { if (obc) return obc->wantArg(o); else return true;};
  bool canSelectRect() { if (obc) return false; else return true;};
  bool canSelectObjects(const Objects& ) { if (obc) return false; else return true; };
  bool canUnselect() { if (obc) return false; else return true; };
  bool canAddObjects() { return !m_pMacroWizard && isReadWrite(); };

signals: // these signals are for telling KigView it should do something...
  // emitted if the entire canvas has to be redrawn
  // i try to avoid calling this...
  void allChanged();
  // repaint a single object (it was either added to the document, or
  // its selection flag changed...
  void repaintOneObject(const Object* o);
  // emitted only for the macrowizard to update its next buttons...
  void selectionChanged();

/************** working with our internal document **********/
public:
  // guess what these do...
  // actually, they only add a command object to the history, the real work is
  // done in _addObject() and _delObject()
  void addObject(Object* inObject);
  void delObject(Object* inObject);

  // OBC: object being constructed (i.e. the user is selecting its
  // arguments...
  // sets the obc to obc, deletes an old obc if required
  void newObc(Object* obc);

  // SOS: Selected ObjectS: stuff which is selected...
  // add stuff to the sos (selected objects)
  // TODO: selectObject also unselects if necessary, and wraps
  // macroSelect and obcSelectArg...
  void selectObject (Object* o);
  void selectObjects (const QRect&);
  void selectObjects (const Objects& o);
  // unselect o
  void unselect (Object* o);

  // Movement:
  void startMovingSos(const QPoint&, Objects& still);
  void moveSosTo(const QPoint&);
  void stopMovingSos();
  //   void cancelMovingSos();

 public slots:
  // Defining macros:
  void newMacro();
  // delete the macro currently being defined
  void delMacro();

public slots:
  // delete the selected objects (sos) (actually only adds an action
  // to the history the real work is done in _delObject
  void deleteSelected();

  // delete the obc
  void delObc();

  // unselect everything
  void clearSelection ();

  // hide the selected objects...
  void editHide()
  {
    for (Object* i = sos.first(); i; i = sos.next()) i->setShown(false);
    emit allChanged();
  };

  // unhide all objects...
  void editShowHidden();

/********************* Types ************************/

  Types* getTypes() { return &types;};
public:
  void addType(Type*);
  void addTypes(const Types& ts)
  {
    for (Types::const_iterator i = ts.begin(); i != ts.end(); ++i)
      addType(*i);
  };

  void removeType(Type*);

  Object* newObject(const QCString& type);
protected slots:
  void editTypes(); 

/************* internal stuff *************/
protected:
  void _addObject(Object* inObject);
  void _addObjects( Objects& o) { for (Object* i = o.first(); i; i = o.next()) _addObject(i); };
  void _delObject(Object* inObject);

  void setupActions();
  void setupTypes();
protected slots:
  void updateActions();

protected:
  // the macro wizard
  QGuardedPtr<MacroWizardImpl> m_pMacroWizard;

  // the command history
  KCommandHistory* history;

  KActionMenu* newSegmentAction;
  KActionMenu* newPointAction;
  KActionMenu* newCircleAction;
  KActionMenu* newLineAction;
  KActionMenu* newOtherAction;

  KAction* cancelConstructionAction;
  KAction* deleteObjectsAction;
  KAction* newMacroAction;

  KAction* configureTypesAction;
  KAction* startKioskModeAction;

  int numViews;

  KigView* m_widget;

  // the types are global to the entire application...
  static Types types;

  // our internal document data:
  // first all objects we contain: all of them are in there, except
  // the obc.
  // this is the one that owns all objects, all other object
  // containers only contain pointers to the objects, and don't own
  // them
  Objects objects;
  // obc is the "object being constructed", if non-null, we're
  // constructing something...
  Object* obc;
  // these are the "selected objects"
  Objects sos;
  // these objects are moving...
  Objects movingObjects;
};

#endif // KIGPART_H

