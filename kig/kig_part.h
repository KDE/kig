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
 * // this is old doc, most of it is still correct though...
 * 
 * the KigDocument briefly consists of three modes: it can be
 * constructing a new object ( selecting args ( other objects ) for it
 * ), it can also be moving objects around,  and it can just be doing
 * nothing special ( selecting and deselecting stuff, and waiting to
 * switch to one of the other modes ).  We make the distinction
 * between these modes by:
 * - if ( obc != 0 ),  we're in construction mode
 * - if ( !cos.empty() ),  we're moving stuff
 * - if none of these two, we're in normal mode
 * @short Main Part
 * @author Dominique Devriese <fritmebufstek@pandora.be>
 * @version 2.0
 */
class KigDocument : public KParts::ReadWritePart
{
  Q_OBJECT
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

public:
  /**
   * This is a virtual function inherited from KParts::ReadWritePart.
   * A shell will use this to inform this Part if it should act
   * read-only
   */
  virtual void setReadWrite(bool rw);

  virtual void setModified(bool modified);

  signals:
  void changed();
protected:
  /**
   * This must be implemented by each part
   */
  virtual bool openFile();

  /**
   * This must be implemented by each read-write part
   */
  virtual bool saveFile();

protected slots:
  void fileOpen();
  void fileSaveAs();

  signals:
  void setStatusBarText ( const QString & text );

private:
  KigView* m_widget;

public:
  // if a new view is added
  void addView(KigView*) { numViews++; };
  void delView(KigView*) { numViews--; };

  // above was the kpart interface, here come the other functions
protected:
  // first all objects we contain: all of them are in there, except
  // the obc.
  // this is the one that owns all objects, all other object
  // containers only contain pointers to the objects, and don't own
  // them
  Objects objects;
  // obc is the "object being constructed", if non-null, we're
  // constructing something...
  Object* obc;
  // the next two are important for moving, so we only redraw the
  // objects that have been moved.
  // "still objects", objects that are currently not moving, so they
  // don't have to be redrawn
  Objects stos;
  // "changing objects",  objects that are being moved
  // this actually is: the sos when the user clicked for moving + all
  // the children of those.
  Objects cos;
  // these are the "selected objects"
  Objects sos;

public:
  const Objects& getCos() { return cos; };
  const Objects& getStos() { return stos; };
  const Objects& getSos() { return sos;};
  const Objects& getObjects() { return objects;};

public:
  // guess what these do...
  // actually, they only add a command object to the history, the real work is
  // done in _addObject() and _delObject()
  void addObject(Object* inObject);
  void delObject(Object* inObject);
public slots:
// delete the selected objects (sos) (actually only adds an action to the history
// the real work is done in _delObject
void deleteSelected();

public:
  // what objects are under point p
  Objects whatAmIOn(const QPoint& p);
  Objects whatAmIOn(int x, int y) { return whatAmIOn(QPoint(x,y));};

public:
  // these work on the obc:
  Object* getObc() const { return obc; };
  // sets the obc to obc, deletes an old obc if required
  void newObc(Object* obc);
  // selects an arg for the obc, takes care of the obc if it says it's
  // finished (i.e. addObject(); obc = 0;)
  void obcSelectArg(Object* o);
public slots:
// delete the obc
void delObc();

public:
  // these work on the sos:
  // add stuff to the sos (selected objects)
  void selectObject (Object* o);
  void selectObjects (const QRect&);
  // unselect o
  void unselect (Object* o);
public slots:
// unselect everything
void clearSelection ();

public:
  // these are for moving
  void startMovingSos(const QPoint&);
  void moveSosTo(const QPoint&);
  void stopMovingSos();
  //   void cancelMovingSos();

  // slots from the edit menu
  void editHide() { for (Objects::iterator i = sos.begin(); i != sos.end(); i++) (*i)->setShown(false);};
  void editShowHidden();

  signals:
  // for communication with KigViews:
  // something was added or removed from the sos
  void selectionChanged(const Object* o);
  // we started moving: grab a picture of the stos
  void startedMoving();
  // we are moving: repaint the cos
  void cosMoved();
  // emitted after we stopped moving
  void stoppedMoving();
  // emitted if an object was added
  void objectAdded( const Object* );
  // emitted if the entire canvas has to be redrawn
  void allChanged();
  // emitted if the mode of the view should change ( e.g. a new obc,
  // stopped/started moving,  obc is finished..;
  void modeChanged();

protected:
  friend class AddObjectsCommand;
  friend class RemoveObjectsCommand;
  void _addObject(Object* inObject);
  void _addObjects(const Objects& o) { for (Objects::const_iterator i = o.begin(); i != o.end(); ++i) _addObject(*i); };
  void _delObject(Object* inObject);

  // have we changed since the last save ?
  bool _changed;
  // are we readOnly
  bool readOnly;

  // the command history
  KCommandHistory* history;

  // the actions
  // file menu
  KAction* KAFileSave;
  KAction* KAFileSaveAs;
  KAction* KAFileOpen;
  // edit menu
  KAction* KADeleteObjects;
  // new menu
  KActionMenu* KANewSegment;
  KActionMenu* KANewPoint;
  KActionMenu* KANewCircle;
  KActionMenu* KANewLine;

  KAction* KACancelConstruction;
  KAction* KAStartKioskMode;
  KAction* KANewMacro;

  // types menu:
  KAction* KAConfigureTypes;
protected slots:
  void updateActions();
protected:
  Objects macroGegObjs;
  Objects macroFinObjs;
  // Where Are We in the process of collecting gegObjs and finObjs:
  enum { macroSGO, macroSFO, macroSN, macroDN } macroWAW;
public:
  bool isConstructingMacro() { return macroWAW != macroDN; };
protected:
  MacroWizardImpl* m_pMacroWizard;
public slots:
  // start a new macro definition
  void newMacro();
  // step forward in the macro definition (i.e. start selecting final
  // objects if you were selecting given objects, or finish the macro
  // if you were selecting final objects
  void macroSelect(Object* o);
  void macroSelect(const Objects& os);
  void macroSelect(const QRect& r);
  void stepMacro();
  void stepBackMacro();
  void finishMacro();
  // delete the macro currently being defined
  void delMacro();
public:
  Types* getTypes() { return &types;};
private:
  int numViews;
protected:
  Types types;
  QPtrList<KAction> newObjectActions;
public:
  void addType(Type*);
  void addTypes(const Types&);
  void removeType(Type*);
  Object* newObject(const QCString& type) { return types.newObject(type);};
protected slots:
  void editTypes(); 
};

#endif // KIGPART_H

